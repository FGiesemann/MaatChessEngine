/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/search.h"
#include "chessengine/evaluation.h"
#include "chessengine/minimax.h"

namespace chessengine {

auto find_best_move(const chesscore::Position &position) -> EvaluatedMove {
    MinimaxSearch search{MinimaxConfig{}, Evaluator{EvaluatorConfig{}}};
    return search.best_move(position);
}

} // namespace chessengine
