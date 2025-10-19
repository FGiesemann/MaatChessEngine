/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_MINIMAX_H
#define CHESSENGINE_MINIMAX_H

#include <chesscore/move.h>
#include <chesscore/position.h>

#include "chessengine/evaluation.h"

namespace chessengine {

struct MinimaxConfig {
    Depth max_depth{5};
    bool use_alpha_beta_pruning{true};
};

/**
 * \brief Implement minimax search with alpha-beta-pruning.
 *
 * Searches a chess position for the best move using the minimax algorithm with
 * alpha-beta pruning.
 */
class MinimaxSearch {
public:
    MinimaxSearch(MinimaxConfig config, const Evaluator &evaluator) : m_config{std::move(config)}, m_evaluator{evaluator} {}
    auto best_move(const chesscore::Position &position) const -> EvaluatedMove;
private:
    MinimaxConfig m_config;
    Evaluator m_evaluator;
    mutable chesscore::Position m_position;
    mutable chesscore::Color m_color_to_evaluate;

    auto minimax(Depth depth, Score alpha, Score beta, bool maximizing_player) const -> Score;
};

} // namespace chessengine

#endif
