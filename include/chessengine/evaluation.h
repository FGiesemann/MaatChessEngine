/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_EVALUATION_H
#define CHESSENGINE_EVALUATION_H

#include <chesscore/position.h>

#include "chessengine/config.h"
#include "chessengine/types.h"

namespace chessengine {

class Evaluator {
public:
    Evaluator() = default;
    explicit Evaluator(EvaluatorConfig config) : m_config{std::move(config)} {}

    /**
     * \brief Evaluate a position.
     *
     * Evaluates a given position from the perspective of the given player.
     * \param position The position to evaluate.
     * \param color The player whose perspective is used for evaluation.
     * \return
     */
    auto evaluate(const chesscore::Position &position, chesscore::Color color) const -> Score;

    /**
     * \brief Evaluation of a single move.
     *
     * Evaluation of a move provides a score that should be higher for moves
     * that promise to give an advantage. It is used to compare moves during
     * search for the best move, e.g. to provide hints for move ordering.
     * \param move The move to evaluate.
     * \return The score for the move.
     */
    auto evaluate(const chesscore::Move &move) const -> Score;

    /**
     * \brief Checks if the position is mate.
     *
     * Checks, if the player who moves next is in checkmate.
     * \param position The position.
     * \return If the active polayer is checkmate or not.
     */
    static auto is_mate(const chesscore::Position &position) -> bool;

    /**
     * \brief Calculate the material score for a player.
     *
     * The material score is the sum of the scores of pieces of the given color.
     * These scores are described in the EvaluatorConfig.
     * \param position The position to evaluate.
     * \param color The color for which to evaluate.
     * \return The calculated score.
     */
    auto countup_material(const chesscore::Position &position, chesscore::Color color) const -> Score;

    /**
     * \brief Accumulate the scores for pieces on squares.
     *
     * Calculates the sum of the scores for each piece on its square.
     * \param position The position to evaluate.
     * \param color The color for which to evaluate.
     * \return The caculated score.
     */
    auto evaluate_pieces_on_squares(const chesscore::Position &position, chesscore::Color color) const -> Score;

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
    EvaluatorConfig m_config{};
};

} // namespace chessengine

#endif
