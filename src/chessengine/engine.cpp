/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/engine.h"

namespace chessengine::maat {

auto Engine::new_game() -> void {
    m_position = chesscore::Position{};
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

auto Engine::set_debugging(bool on) -> void {
    m_debugging = on;
}

auto Engine::load_config(const std::filesystem::path &filename) -> void {
    m_config = Config::from_file(filename);
}

} // namespace chessengine::maat
