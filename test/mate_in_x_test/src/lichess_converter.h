/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef MATE_IN_X_TEST_LICHESS_CONVERTER_H
#define MATE_IN_X_TEST_LICHESS_CONVERTER_H

#include "multi_solution_finder.h"

#include <chesscore/move.h>
#include <chesscore/position.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace chessengine::mate_in_x {

struct MateInXPuzzle {
    std::string id;
    chesscore::Position position;
    chesscore::MoveList moves;

    [[nodiscard]] auto best_move() const -> const chesscore::Move & { return moves.front(); }
    [[nodiscard]] auto mate_plys() const -> size_t { return moves.size(); }
};

using Puzzles = std::vector<MateInXPuzzle>;

class Error : public std::runtime_error {
public:
    explicit Error(const std::string &message) : std::runtime_error{message} {}
};

namespace fs = std::filesystem;

class LichessConverter {
public:
    static const std::string ExpectedCSVHeader;
    static const size_t ThemeFieldIndex{7};

    auto set_output(const fs::path &output) -> void { m_output = output; }
    auto output() const -> const fs::path & { return m_output; }
    auto setup_multi_solution_finder(const chessuci::ProcessParams &params) -> void;

    auto process(const fs::path &input) -> void;
private:
    std::ifstream m_input_file;
    fs::path m_output;
    chesscore::EpdSuite m_puzzles;
    std::unordered_map<int, int> m_puzzle_distrib;
    std::unique_ptr<MultiSolutionFinder> m_multi_solution_finder{};

    auto open_input(const fs::path &input) -> void;
    auto check_header_fields() -> void;
    auto read_puzzles() -> void;
    auto is_mate_puzzle(std::string_view theme) -> bool;
    auto extract_puzzle(const std::vector<std::string> &fields) -> MateInXPuzzle;
    auto convert_to_epd(const MateInXPuzzle &puzzle) -> chesscore::EpdRecord;
    auto print_puzzle_distrib() -> void;
    auto sort_puzzles() -> void;
    auto write_puzzles() -> void;

    auto write_all_puzzles() -> void;
    auto write_puzzle_files() -> void;
};

} // namespace chessengine::mate_in_x

#endif
