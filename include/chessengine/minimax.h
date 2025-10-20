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
    auto best_move(const chesscore::Position &position, Depth depth) const -> EvaluatedMove;

    /**
     * \brief Statistics of the last search.
     */
    struct SearchStats {
        std::uint64_t nodes{0};   ///< Number of noes evaluated during search.
        std::uint64_t cutoffs{0}; ///< Number of branches cut off during search.
    };

    /**
     * \brief Get the search statistics.
     *
     * Gives the statistics of the last search.
     * \return Statistics of the last search.
     */
    auto search_stats() const -> SearchStats { return m_stats; }
private:
    MinimaxConfig m_config;
    Evaluator m_evaluator;
    mutable SearchStats m_stats;
    mutable chesscore::Position m_position;
    mutable chesscore::Color m_color_to_evaluate;

    auto minimax(Depth depth, Score alpha, Score beta, bool maximizing_player) const -> Score;
};

} // namespace chessengine

#endif
