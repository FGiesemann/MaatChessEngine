/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <chesscore/position.h>

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

auto split_fields(std::string_view line, std::string_view separator = ",") -> std::vector<std::string>;
auto is_mate_puzzle(std::string_view theme) -> bool;
auto extract_puzzle(const std::vector<std::string> &fields, std::vector<MateInXPuzzle> &puzzles) -> void;

auto main(int argc, const char *argv[]) -> int {
    if (argc < 2) {
        std::cerr << "Usage: "s << argv[0] << " <input_file>\n";
        return 1;
    }

    std::string input_filename{argv[1]};
    std::cout << "Opening " << input_filename << "\n";
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
    while (std::getline(input_file, line)) {
        ++line_count;
        const auto fields = split_fields(line);
        if (fields.size() >= 8) {
            if (is_mate_puzzle(fields[ThemeFieldIndex])) {
                extract_puzzle(fields, puzzles);
            }
        }
    }
    std::cout << "Processed " << line_count << " lines\n";
    return 0;
}

auto extract_puzzle(const std::vector<std::string> &fields, std::vector<MateInXPuzzle> &puzzles) -> void {
    auto position = chesscore::Position{chesscore::FenString{fields[1]}};
    const auto solution = split_fields(fields[2], " ");

    // first move in solution has to be applied to position in order to get the starting position of the puzzle
    // the remaining moves in solution are collected in the MoveList
    chesscore::MoveList moves{};

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
