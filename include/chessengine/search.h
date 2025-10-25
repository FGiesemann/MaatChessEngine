/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include <chesscore/position.h>

#include "chessengine/config.h"

namespace chessengine {

/**
 * \brief Statistics of the last search.
 */
struct SearchStats {
    std::uint64_t nodes{0};   ///< Number of noes evaluated during search.
    std::uint64_t cutoffs{0}; ///< Number of branches cut off during search.
};

class Search {
public:
    Search(const Config &config);
    auto best_move(const chesscore::Position &position) const -> EvaluatedMove;

    auto search_stats() const -> const SearchStats & { return m_search_stats; }
private:
    Config m_config;
    mutable SearchStats m_search_stats;
};

/**
 * \brief Comparison of moves for move ordering.
 *
 * Function object for giving moves an order.
 */
class MoveOrdering {
public:
    MoveOrdering() = default;
    explicit MoveOrdering(const EvaluatorConfig &config) : m_config{config} {}

    /**
     * \brief Compare two moves.
     *
     * Moves that are more promising than other moves should be considered
     * before them during the search.
     * \param lhs Left-hand side of the comparison.
     * \param rhs Right-hand side of the comparison.
     * \return If lhs is more promising that rhs.
     */
    auto operator()(const chesscore::Move &lhs, const chesscore::Move &rhs) const -> bool;

    auto evaluate_move(const chesscore::Move &move) const -> Score;

    /**
     * \brief Get the score for a capturing move.
     *
     * A capturing move is awarded by the value difference of the captured
     * piece and the capturing piece. A non-capture move returns a score of 0.
     * \param move The move to evaluate.
     * \return The capturing score.
     */
    auto get_capture_score(const chesscore::Move &move) const -> Score;

    /**
     * \brief Get the score for a promoting pawn.
     *
     * A pawn promotion is awarded a fixed bonus score plus the difference in
     * value of the promoted-to-piece and the pawn value. A non-promoting move
     * returns a score of 0.
     * \param move The move to evaluate.
     * \return The promotion score.
     */
    auto get_promotion_score(const chesscore::Move &move) const -> Score;

    auto get_piece_movement_score(const chesscore::Move &move) const -> Score;
private:
    EvaluatorConfig m_config{}; ///< Describes the values of pieces.
};

/**
 * \brief Sorts the moves in a move list.
 *
 * \param moves The moves to sort.
 * \param ordering The move oerdering to use.
 */
auto sort_moves(chesscore::MoveList &moves, MoveOrdering ordering = MoveOrdering{}) -> void;

} // namespace chessengine

#endif
