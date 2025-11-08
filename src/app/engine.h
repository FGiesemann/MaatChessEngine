/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_MAAT_ENGINE_H
#define CHESS_ENGINE_MAAT_ENGINE_H

#include <chesscore/position.h>

namespace chessengine::maat {

class Engine {
public:
    auto new_game() -> void;
    auto set_position(const chesscore::Position &position) -> void;
    auto position() const -> const chesscore::Position & { return m_position; }
    auto set_debugging(bool on) -> void;

    auto start_search(/* some options */) -> void;
    auto stop_search() -> void;
    auto best_move() const -> chesscore::Move;
private:
    chesscore::Position m_position;
    bool m_debugging{false};
};

} // namespace chessengine::maat

#endif
