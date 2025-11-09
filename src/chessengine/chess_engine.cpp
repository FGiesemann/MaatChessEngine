/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/chess_engine.h"

namespace chessengine {

const std::string ChessEngine::identifier = "Maat v0.1";
const std::string ChessEngine::author = "Florian Giesemann";

auto ChessEngine::new_game() -> void {
    m_position = chesscore::Position{chesscore::FenString::starting_position()};
}

auto ChessEngine::start_search() -> void {
    // TODO
}

auto ChessEngine::stop_search() -> void {
    // stop search thread
}

auto ChessEngine::best_move() const -> chesscore::Move {
    // TODO
    return {};
}

auto ChessEngine::set_position(const chesscore::Position &position) -> void {
    m_position = position;
}

auto ChessEngine::play_move(const chesscore::Move &move) -> void {
    m_position.make_move(move);
}

auto ChessEngine::set_debugging(bool debug_on) -> void {
    m_debugging = debug_on;
}

auto ChessEngine::load_config(const std::filesystem::path &filename) -> void {
    m_config = Config::from_file(filename);
}

} // namespace chessengine
