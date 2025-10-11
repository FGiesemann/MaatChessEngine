/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include <chesscore/move.h>
#include <chesscore/position.h>

namespace chessengine {

auto find_best_move(const chesscore::Position &position) -> chesscore::Move;

} // namespace chessengine

#endif
