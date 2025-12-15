/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <chesscore/epd.h>
#include <chesscore/position.h>
#include <chessgame/san.h>
#include <chessuci/move.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;

const std::string ExpectedCSVHeader{"PuzzleId,FEN,Moves,Rating,RatingDeviation,Popularity,NbPlays,Themes,GameUrl,OpeningTags"s};
const size_t ThemeFieldIndex{7};

struct MateInXPuzzle {
    std::string id;
    chesscore::Position position;
    chesscore::MoveList moves;

    [[nodiscard]] auto best_move() const -> const chesscore::Move & { return moves.front(); }
    [[nodiscard]] auto mate_plys() const -> size_t { return moves.size(); }
};

class Error : public std::runtime_error {
public:
    explicit Error(const std::string &message) : std::runtime_error{message} {}
};

auto check_header_fields(std::istream &input) -> void;
auto read_puzzles(std::istream &input) -> std::vector<MateInXPuzzle>;
auto is_mate_puzzle(std::string_view theme) -> bool;
auto extract_puzzle(const std::vector<std::string> &fields, std::vector<MateInXPuzzle> &puzzles) -> void;
auto write_puzzles(std::ostream &output, const std::vector<MateInXPuzzle> &puzzles) -> void;
auto uci_move_to_move(const std::string &uci_str, const chesscore::Position &position) -> chesscore::Move;
auto split_fields(std::string_view line, std::string_view separator = ",") -> std::vector<std::string>;
auto convert_to_san_move(const chesscore::Move &move, const chesscore::Position &position) -> std::string;
auto convert_to_san_moves(const chesscore::MoveList &moves, chesscore::Position position, chesscore::EpdRecord::move_list &list) -> void;

auto main(int argc, const char *argv[]) -> int {
    if (argc < 3) {
        std::cerr << "Usage: "s << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    std::string input_filename{argv[1]};
    std::ifstream input_file{input_filename};
    if (!input_file.is_open()) {
        std::cerr << "Unable to open " << input_filename << "\n";
        return 1;
    }

    check_header_fields(input_file);
    std::vector<MateInXPuzzle> puzzles = read_puzzles(input_file);
    std::cout << "Sorting...\n";
    std::ranges::sort(puzzles, [](const MateInXPuzzle &a, const MateInXPuzzle &b) { return a.mate_plys() < b.mate_plys(); });
    std::cout << "Writing EPD file...\n";
    std::string output_filename{argv[2]};
    std::ofstream epd_file{output_filename};
    write_puzzles(epd_file, puzzles);
    std::cout << "Done.\n";

    return 0;
}

auto check_header_fields(std::istream &input) -> void {
    std::string line{};
    std::getline(input, line);
    if (line != ExpectedCSVHeader) {
        throw Error{"File does not start with expected header"};
    }
}

auto read_puzzles(std::istream &input) -> std::vector<MateInXPuzzle> {
    std::vector<MateInXPuzzle> puzzles{};
    std::string line{};
    std::size_t line_count{0};
    std::cout << "Found 0 puzzles";
    while (std::getline(input, line)) {
        ++line_count;
        const auto fields = split_fields(line);
        if (fields.size() >= 8) {
            if (is_mate_puzzle(fields[ThemeFieldIndex])) {
                try {
                    extract_puzzle(fields, puzzles);
                } catch (const Error &e) {
                    std::cerr << "Error in line " << line_count << ": " << e.what() << "\n";
                }
            }
        }

        if ((line_count + 1) % 100 == 0) {
            std::cout << "\rFound " << puzzles.size() << " puzzles";
        }
    }
    std::cout << "\rFound " << puzzles.size() << " puzzles\n";
    return puzzles;
}

auto is_mate_puzzle(std::string_view theme) -> bool {
    return theme.contains("mate");
}

auto extract_puzzle(const std::vector<std::string> &fields, std::vector<MateInXPuzzle> &puzzles) -> void {
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

    puzzles.emplace_back(fields[0], position, moves);
}

auto write_puzzles(std::ostream &output, const std::vector<MateInXPuzzle> &puzzles) -> void {
    for (const auto &puzzle : puzzles) {
        chesscore::EpdRecord record{};
        record.id = puzzle.id;
        record.position = puzzle.position;
        record.bm.push_back(convert_to_san_move(puzzle.best_move(), puzzle.position));
        convert_to_san_moves(puzzle.moves, puzzle.position, record.pv);
        write_epd_record(output, record);
    }
}

auto uci_move_to_move(const std::string &uci_str, const chesscore::Position &position) -> chesscore::Move {
    const auto exp_move = chessuci::parse_uci_move(uci_str);
    if (!exp_move.has_value()) {
        throw Error{std::string{"Failed to parse move "} + uci_str};
    }

    const auto moves = chessuci::match_move(exp_move.value(), position.all_legal_moves());
    if (moves.size() != 1) {
        throw Error{std::string{"Failed to find move "} + uci_str};
    }
    return moves.front();
}

auto split_fields(std::string_view line, std::string_view separator) -> std::vector<std::string> {
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

auto convert_to_san_move(const chesscore::Move &move, const chesscore::Position &position) -> std::string {
    const auto legal_moves = position.all_legal_moves();
    const auto opt_san = chessgame::generate_san_move(move, legal_moves);
    if (opt_san.has_value()) {
        return opt_san.value().san_string;
    }
    throw Error{"Failed to convert move to SAN"};
}

auto convert_to_san_moves(const chesscore::MoveList &moves, chesscore::Position position, chesscore::EpdRecord::move_list &list) -> void {
    for (const auto &move : moves) {
        const auto legal_moves = position.all_legal_moves();
        const auto opt_san = chessgame::generate_san_move(move, legal_moves);
        if (opt_san.has_value()) {
            list.push_back(opt_san.value().san_string);
            position.make_move(move);
        } else {
            throw Error{"Failed to convert move to SAN"};
        }
    }
}
