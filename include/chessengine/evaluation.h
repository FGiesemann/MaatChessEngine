/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_EVALUATION_H
#define CHESSENGINE_EVALUATION_H

#include <chesscore/position.h>

#include <compare>
#include <cstdint>

namespace chessengine {

/**
 * \brief The evaluation score for a chess position.
 */
struct [[nodiscard]] Score {
    using value_type = std::int16_t; ///< The underlying data type.

    value_type value{}; ///< The value.

    /**
     * \brief Construct a new Score object.
     *
     * Create a new Score with the given value.
     * \param value The value.
     */
    constexpr explicit Score(value_type value = {}) : value{value} {}

    /**
     * \brief Comparison operator.
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Score &other) const = default;

    /**
     * \brief Addition assignment operator.
     *
     * Adds a value to the score.
     * \param other The value to add.
     * \return The incremented score.
     */
    constexpr auto operator+=(Score other) -> Score & {
        value = static_cast<value_type>(value + other.value);
        return *this;
    }

    /**
     * \brief Subtraction assignment operator.
     *
     * Subtracts a value from the score.
     * \param other The value to subtract.
     * \return The decremented score.
     */
    constexpr auto operator-=(Score other) -> Score & {
        value = static_cast<value_type>(value - other.value);
        return *this;
    }

    /**
     * \brief Multiplication assignment operator.
     *
     * Multiplies the score by a factor.
     * \param factor The factor.
     * \return The multiplied score.
     */
    constexpr auto operator*=(int factor) -> Score & {
        value = static_cast<value_type>(value * factor);
        return *this;
    }
};

/**
 * \brief Negation operator.
 *
 * Negates a Score.
 * \param s The score.
 * \return The negated score.
 */
[[nodiscard]] constexpr auto operator-(Score s) -> Score {
    return Score{static_cast<Score::value_type>(-s.value)};
}

/**
 * \brief Addition operator.
 *
 * Adds two scores.
 * \param lhs The left operand.
 * \param rhs The right operand.
 * \return The sum of the scores.
 */
[[nodiscard]] constexpr auto operator+(Score lhs, Score rhs) -> Score {
    Score result{lhs};
    return result += rhs;
}

/**
 * \brief Subtraction operator.
 *
 * Subtracts two scores.
 * \param lhs The left operand.
 * \param rhs The right operand.
 * \return The difference of the scores.
 */
[[nodiscard]] constexpr auto operator-(Score lhs, Score rhs) -> Score {
    Score result{lhs};
    return result -= rhs;
}

[[nodiscard]] constexpr auto operator*(Score lhs, int rhs) -> Score {
    Score result{lhs};
    return result *= rhs;
}

[[nodiscard]] constexpr auto operator*(int lhs, Score rhs) -> Score {
    return rhs * lhs;
}

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
    static constexpr Score Infinity{32700};
    static constexpr Score NegInfinity{-Infinity};
    static constexpr Score Mate{32000};

    explicit Evaluator(EvaluatorConfig config) : m_config{std::move(config)} {}
    auto evaluate(const chesscore::Position &position, chesscore::Color color) const -> Score;

    auto countup_material(const chesscore::Position &position, chesscore::Color color) const -> Score;
private:
    EvaluatorConfig m_config;
};

} // namespace chessengine

#endif
