/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <chesscore/position.h>
#include <chessuci/move.h>

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

auto split_fields(std::string_view line, std::string_view separator = ",") -> std::vector<std::string>;
auto is_mate_puzzle(std::string_view theme) -> bool;
auto extract_puzzle(const std::vector<std::string> &fields, std::vector<MateInXPuzzle> &puzzles) -> void;

auto main(int argc, const char *argv[]) -> int {
    if (argc < 2) {
        std::cerr << "Usage: "s << argv[0] << " <input_file>\n";
        return 1;
    }

    std::string input_filename{argv[1]};
    std::ifstream input_file{input_filename};
    if (!input_file.is_open()) {
        std::cerr << "Unable to open " << input_filename << "\n";
        return 1;
    }

    std::string line{};
    std::getline(input_file, line);
    if (line != ExpectedCSVHeader) {
        std::cout << "WARNING! File does not start with expected header\n";
    }

    std::vector<MateInXPuzzle> puzzles{};
    std::size_t line_count{0};
    std::cout << "Found 0 puzzles";
    while (std::getline(input_file, line)) {
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

    // TODO:
    // - sort by x (as in mate-in-x)
    // - write to EPD output file

    return 0;
}

auto convert_move(const std::string &uci_str, const chesscore::Position &position) -> chesscore::Move {
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

auto extract_puzzle(const std::vector<std::string> &fields, std::vector<MateInXPuzzle> &puzzles) -> void {
    const auto solution = split_fields(fields[2], " ");
    auto position = chesscore::Position{chesscore::FenString{fields[1]}};
    const auto setup_move = convert_move(solution.front(), position);
    position.make_move(setup_move);

    chesscore::MoveList moves{};
    auto test_position{position};
    for (size_t index = 1; index < solution.size(); ++index) {
        const auto move = convert_move(solution[index], test_position);
        moves.push_back(move);
        test_position.make_move(move);
    }

    puzzles.emplace_back(fields[0], position, moves);
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

auto is_mate_puzzle(std::string_view theme) -> bool {
    return theme.contains(" mate ") || theme.contains(",mate") || theme.contains("mate,");
}
