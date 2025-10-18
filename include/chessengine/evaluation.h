/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_EVALUATION_H
#define CHESSENGINE_EVALUATION_H

#include <chesscore/position.h>

#include "chessengine/types.h"

namespace chessengine {

/**
 * \brief Configuration for the evaluator.
 *
 * The configuration defines several parameters that influence the evaluation
 * of a chess position.
 */
class EvaluatorConfig {
public:
    /**
     * \brief Get the value for a piece of a given type.
     *
     * \param piece_type Type of the piece.
     * \return Value for a piece of the given type.
     */
    auto piece_value(chesscore::PieceType piece_type) const -> Score { return m_piece_values[get_index(piece_type)]; }
    auto empty_board_value() const -> Score { return Score{0}; }
private:
    Score m_piece_values[6]{Score{100}, Score{300}, Score{300}, Score{500}, Score{900}, Score{0}}; // same order as chesscore piece types

    static_assert(get_index(chesscore::PieceType::Pawn) == 0);
    static_assert(get_index(chesscore::PieceType::Rook) == 1);
    static_assert(get_index(chesscore::PieceType::Knight) == 2);
    static_assert(get_index(chesscore::PieceType::Bishop) == 3);
    static_assert(get_index(chesscore::PieceType::Queen) == 4);
    static_assert(get_index(chesscore::PieceType::King) == 5);
};

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
    auto countup_material(const chesscore::Position &position, chesscore::Color color) const -> Score;
private:
    EvaluatorConfig m_config{};
};

} // namespace chessengine

#endif
