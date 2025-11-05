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
    auto evaluate(const chesscore::Position &position, chesscore::Color color) const -> Score;

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
private:
    EvaluatorConfig m_config{};
};

} // namespace chessengine

#endif
