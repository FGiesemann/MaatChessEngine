/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_TYPES_H
#define CHESSENGINE_TYPES_H

#include <compare>
#include <cstdint>

namespace chessengine {

template<typename T, typename Tag>
struct StrongType {
    using value_type = T;
    value_type value{};

    constexpr explicit StrongType(value_type value = {}) : value{value} {}

    [[nodiscard]] constexpr std::strong_ordering operator<=>(const StrongType &other) const = default;
};

/**
 * \brief The evaluation score for a chess position.
 */
struct [[nodiscard]] Score : public StrongType<std::int16_t, struct ScoreTag> {
    using StrongType::StrongType;

    constexpr auto operator+=(Score other) -> Score & {
        value = static_cast<value_type>(value + other.value);
        return *this;
    }
    constexpr auto operator-=(Score other) -> Score & {
        value = static_cast<value_type>(value - other.value);
        return *this;
    }
    constexpr auto operator*=(int factor) -> Score & {
        value = static_cast<value_type>(value * factor);
        return *this;
    }

    static const Score Infinity;
    static const Score NegInfinity;
    static const Score Mate;
};

struct [[nodiscard]] Depth : public StrongType<std::int16_t, struct DepthTag> {
    using StrongType::StrongType;

    constexpr auto operator+=(Depth other) -> Depth & {
        value = static_cast<value_type>(value + other.value);
        return *this;
    }
    constexpr auto operator-=(Depth other) -> Depth & {
        value = static_cast<value_type>(value - other.value);
        return *this;
    }

    constexpr auto operator++() -> Depth & {
        value = static_cast<value_type>(value + 1);
        return *this;
    }

    [[nodiscard]] constexpr auto operator++(int) -> Depth {
        Depth old = *this;
        operator++();
        return old;
    }

    constexpr auto operator--() -> Depth & {
        value = static_cast<value_type>(value - 1);
        return *this;
    }

    [[nodiscard]] constexpr auto operator--(int) -> Depth {
        Depth old = *this;
        operator--();
        return old;
    }

    static const Depth MaxMateDepth;
};

/**
 * \brief Negation operator.
 *
 * Negates a Score.
 * \param s The score.
 * \return The negated score.
 */
[[nodiscard]] constexpr auto operator-(Score score) -> Score {
    return Score{static_cast<Score::value_type>(-score.value)};
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

[[nodiscard]] constexpr auto operator-(Score lhs, Depth rhs) -> Score {
    auto value = static_cast<Score::value_type>(lhs.value - rhs.value);
    return Score{value};
}

} // namespace chessengine

#endif
