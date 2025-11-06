/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "engine.h"

namespace chessengine::maat {

auto Engine::set_position(const chesscore::Position &position) -> void {
    m_position = position;
}

} // namespace chessengine::maat
