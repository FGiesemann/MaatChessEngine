/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/evaluation.h"

namespace chessengine {

auto Evaluator::evaluate([[maybe_unused]] const chesscore::Position &position) const -> int {
    return m_config.empty_board_value();
}

} // namespace chessengine
