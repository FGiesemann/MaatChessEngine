/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_MINIMAX_H
#define CHESSENGINE_MINIMAX_H

#include <chesscore/move.h>
#include <chesscore/position.h>

#include "chessengine/config.h"
#include "chessengine/evaluation.h"
#include "chessengine/search.h"

namespace chessengine {

/**
 * \brief Implement minimax search with alpha-beta-pruning.
 *
 * Searches a chess position for the best move using the minimax algorithm with
 * alpha-beta pruning.
 */
class MinimaxSearch {
public:
    MinimaxSearch(MinimaxConfig config, const Evaluator &evaluator, const MoveOrdering &move_ordering)
        : m_config{std::move(config)}, m_evaluator{evaluator}, m_move_ordering{move_ordering} {}
    auto best_move(const chesscore::Position &position, Depth depth) const -> EvaluatedMove;

    /**
     * \brief Get the search statistics.
     *
     * Gives the statistics of the last search.
     * \return Statistics of the last search.
     */
    auto search_stats() const -> SearchStats { return m_stats; }
private:
    MinimaxConfig m_config{};
    Evaluator m_evaluator{};
    MoveOrdering m_move_ordering{};
    mutable SearchStats m_stats{};
    mutable chesscore::Position m_position;
    mutable chesscore::Color m_color_to_evaluate;

    auto minimax(Depth depth, Score alpha, Score beta, bool maximizing_player) const -> Score;

    auto moves_to_search() const -> chesscore::MoveList;
};

} // namespace chessengine

#endif
