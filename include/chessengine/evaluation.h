/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_EVALUATION_H
#define CHESSENGINE_EVALUATION_H

#include <chesscore/position.h>

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
    auto piece_value(chesscore::PieceType piece_type) const -> int { return m_piece_values[get_index(piece_type)]; }
    auto empty_board_value() const -> int { return 0; }
private:
    int m_piece_values[6]{100, 300, 300, 500, 900, 0}; // same order as chesscore piece types

    static_assert(get_index(chesscore::PieceType::Pawn) == 0);
    static_assert(get_index(chesscore::PieceType::Rook) == 1);
    static_assert(get_index(chesscore::PieceType::Knight) == 2);
    static_assert(get_index(chesscore::PieceType::Bishop) == 3);
    static_assert(get_index(chesscore::PieceType::Queen) == 4);
    static_assert(get_index(chesscore::PieceType::King) == 5);
};

class Evaluator {
public:
    explicit Evaluator(EvaluatorConfig config) : m_config{std::move(config)} {}
    auto evaluate(const chesscore::Position &position, chesscore::Color color) const -> int;

    static constexpr int Infinity = std::numeric_limits<int>::max();
    static constexpr int NegInfinity = std::numeric_limits<int>::min();

    auto countup_material(const chesscore::Position &position, chesscore::Color color) const -> int;
private:
    EvaluatorConfig m_config;
};

} // namespace chessengine

#endif
