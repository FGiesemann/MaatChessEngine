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
    auto set_position(const chesscore::Position &position) -> void;
    auto position() const -> const chesscore::Position & { return m_position; }
private:
    chesscore::Position m_position;
};

} // namespace chessengine::maat

#endif
