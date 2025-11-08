/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/engine.h"

namespace chessengine::maat {

auto Engine::new_game() -> void {
    m_position = chesscore::Position{chesscore::FenString::starting_position()};
}

auto Engine::start_search() -> void {
    // TODO
}

auto Engine::stop_search() -> void {
    // stop search thread
}

auto Engine::best_move() const -> chesscore::Move {
    // TODO
    return {};
}

auto Engine::set_position(const chesscore::Position &position) -> void {
    m_position = position;
}

auto Engine::play_move(const chesscore::Move &move) -> void {
    m_position.make_move(move);
}

auto Engine::set_debugging(bool debug_on) -> void {
    m_debugging = debug_on;
}

auto Engine::load_config(const std::filesystem::path &filename) -> void {
    m_config = Config::from_file(filename);
}

} // namespace chessengine::maat
