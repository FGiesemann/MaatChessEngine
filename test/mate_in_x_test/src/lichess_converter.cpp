/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "lichess_converter.h"

#include <chessgame/san.h>
#include <chessuci/move.h>

#include <iostream>

namespace {

auto split_fields(std::string_view line, std::string_view separator = ",") -> std::vector<std::string> {
    std::vector<std::string> fields{};
    size_t field_begin{0};
    auto separator_index = line.find(separator, field_begin);
    while (separator_index != std::string::npos) {
        fields.emplace_back(line.substr(field_begin, separator_index - field_begin));
        field_begin = separator_index + 1;
        separator_index = line.find(separator, field_begin);
    }
    if (field_begin < line.length()) {
        fields.emplace_back(line.substr(field_begin));
    }
    return fields;
}

auto uci_move_to_move(const std::string &uci_str, const chesscore::Position &position) -> chesscore::Move {
    const auto exp_move = chessuci::parse_uci_move(uci_str);
    if (!exp_move.has_value()) {
        throw chessengine::mate_in_x::Error{std::string{"Failed to parse move "} + uci_str};
    }

    const auto moves = chessuci::match_move(exp_move.value(), position.all_legal_moves());
    if (moves.size() != 1) {
        throw chessengine::mate_in_x::Error{std::string{"Failed to find move "} + uci_str};
    }
    return moves.front();
}

auto convert_to_san_move(const chesscore::Move &move, const chesscore::Position &position) -> std::string {
    const auto legal_moves = position.all_legal_moves();
    const auto opt_san = chessgame::generate_san_move(move, legal_moves);
    if (opt_san.has_value()) {
        return opt_san.value().san_string;
    }
    throw chessengine::mate_in_x::Error{"Failed to convert move to SAN"};
}

auto convert_to_san_moves(const chesscore::MoveList &moves, chesscore::Position position, chesscore::EpdRecord::move_list &list) -> void {
    for (const auto &move : moves) {
        const auto legal_moves = position.all_legal_moves();
        const auto opt_san = chessgame::generate_san_move(move, legal_moves);
        if (opt_san.has_value()) {
            list.push_back(opt_san.value().san_string);
            position.make_move(move);
        } else {
            throw chessengine::mate_in_x::Error{"Failed to convert move to SAN"};
        }
    }
}

} // namespace

namespace chessengine::mate_in_x {

const std::string LichessConverter::ExpectedCSVHeader{"PuzzleId,FEN,Moves,Rating,RatingDeviation,Popularity,NbPlays,Themes,GameUrl,OpeningTags"};

auto LichessConverter::setup_multi_solution_finder(const chessuci::ProcessParams &params) -> void {
    m_multi_solution_finder = std::make_unique<MultiSolutionFinder>(params);
}

auto LichessConverter::process(const fs::path &input) -> void {
    m_puzzle_distrib.clear();
    open_input(input);
    check_header_fields();
    read_puzzles();
    print_puzzle_distrib();
    sort_puzzles();

    write_puzzles();
}

auto LichessConverter::open_input(const fs::path &input) -> void {
    m_input_file.open(input);
    if (!m_input_file.is_open()) {
        throw Error{"Unable to open input file"};
    }
}

auto LichessConverter::check_header_fields() -> void {
    std::string line{};
    std::getline(m_input_file, line);
    if (line != ExpectedCSVHeader) {
        throw Error{"File does not start with expected header"};
    }
}

auto LichessConverter::read_puzzles() -> void {
    std::string line{};
    std::size_t line_count{0};
    std::size_t puzzle_count{0};
    while (std::getline(m_input_file, line)) {
        ++line_count;
        const auto fields = split_fields(line);
        if (fields.size() >= 8) {
            if (is_mate_puzzle(fields[ThemeFieldIndex])) {
                try {
                    const auto puzzle = extract_puzzle(fields);
                    const auto ply = puzzle.mate_plys();
                    ++m_puzzle_distrib[static_cast<int>(ply)];
                    ++puzzle_count;
                    std::cout << "Puzzle " << std::setw(8) << puzzle_count << " [" << puzzle.id << "] @ " << line_count << ": depth " << ply << "\n";
                    auto epd_record = convert_to_epd(puzzle);
                    if (epd_record.pv.size() == 1 && m_multi_solution_finder != nullptr) {
                        m_multi_solution_finder->process(epd_record);
                    }
                    m_puzzles.push_back(epd_record);
                } catch (const Error &e) {
                    std::cout << "Error in line " << line_count << ": " << e.what() << "\n";
                }
            }
        }
    }
}

auto LichessConverter::is_mate_puzzle(std::string_view theme) -> bool {
    return theme.contains("mate");
}

auto LichessConverter::extract_puzzle(const std::vector<std::string> &fields) -> MateInXPuzzle {
    const auto solution = split_fields(fields[2], " ");
    auto position = chesscore::Position{chesscore::FenString{fields[1]}};
    const auto setup_move = uci_move_to_move(solution.front(), position);
    position.make_move(setup_move);

    chesscore::MoveList moves{};
    auto test_position{position};
    for (size_t index = 1; index < solution.size(); ++index) {
        const auto move = uci_move_to_move(solution[index], test_position);
        moves.push_back(move);
        test_position.make_move(move);
    }

    return MateInXPuzzle(fields[0], position, moves);
}

auto LichessConverter::convert_to_epd(const MateInXPuzzle &puzzle) -> chesscore::EpdRecord {
    chesscore::EpdRecord record{};
    record.id = puzzle.id;
    record.position = puzzle.position;
    record.bm.push_back(convert_to_san_move(puzzle.best_move(), puzzle.position));
    convert_to_san_moves(puzzle.moves, puzzle.position, record.pv);
    record.c[0] = "mate_in_" + std::to_string(puzzle.mate_plys());
    return record;
}

auto LichessConverter::print_puzzle_distrib() -> void {
    std::cout << "\nLoaded " << m_puzzles.size() << " from input file:\n";

    std::vector<typename decltype(m_puzzle_distrib)::key_type> depths;
    for (const auto &depth : m_puzzle_distrib) {
        depths.push_back(depth.first);
    }
    std::ranges::sort(depths);
    for (const auto &depth : depths) {
        const auto &count = m_puzzle_distrib[depth];
        std::cout << "  Depth " << std::setw(2) << depth << ": " << std::setw(7) << count << " puzzles\n";
    }
}

auto LichessConverter::sort_puzzles() -> void {
    std::ranges::sort(m_puzzles, [](const chesscore::EpdRecord &a, const chesscore::EpdRecord &b) { return a.pv.size() < b.pv.size(); });
}

auto LichessConverter::write_puzzles() -> void {
    if (fs::is_directory(m_output)) {
        write_puzzle_files();
    } else {
        write_all_puzzles();
    }
}

auto LichessConverter::write_all_puzzles() -> void {
    std::ofstream output(m_output);
    for (const auto &record : m_puzzles) {
        write_epd_record(output, record);
    }
}

auto LichessConverter::write_puzzle_files() -> void {
    std::ofstream output{};
    int current_depth{0};
    for (const auto &record : m_puzzles) {
        int depth = record.pv.size();
        if (depth != current_depth) {
            output.close();
            output.open(m_output / (std::string{"mate_in_"} + std::to_string(depth) + std::string{".epd"}));
            current_depth = depth;
        }
        write_epd_record(output, record);
    }
}

} // namespace chessengine::mate_in_x

namespace fs = std::filesystem;

struct Params {
    fs::path m_input;
    fs::path m_output;
    fs::path m_stockfish_path;
};

auto process_args(int argc, char *argv[]) -> Params {
    Params params{};
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == std::string{"--stockfish"}) {
            ++i;
            params.m_stockfish_path = fs::path{argv[i]};
        } else {
            params.m_input = fs::path{argv[i]};
            ++i;
            params.m_output = fs::path{argv[i]};
        }
    }
    return params;
}

auto main(int argc, char *argv[]) -> int {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [--stockfish <path>] <input_file> <output>\n";
        return 1;
    }

    Params params = process_args(argc, argv);

    chessengine::mate_in_x::LichessConverter converter{};
    converter.set_output(params.m_output);
    if (!params.m_stockfish_path.empty()) {
        converter.setup_multi_solution_finder({.executable = params.m_stockfish_path});
    }
    converter.process(params.m_input);

    return 0;
}
