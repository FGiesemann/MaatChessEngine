/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_TYPES_H
#define CHESSENGINE_TYPES_H

#include <chesscore/move.h>

#include <chrono>
#include <compare>
#include <cstdint>
#include <functional>
#include <iostream>

namespace chessengine {

/**
 * \brief Base implementation for a strong (integral) type.
 *
 * \tparam T The underlying base type.
 * \tparam Tag Some name to distinguish different realisations of strong types.
 */
template<typename T, typename Tag>
struct StrongType {
    using value_type = T; ///< Underlying type.
    value_type value{};   ///< The value.

    /**
     * \brief Construct an instance of the strong type with a value.
     *
     * \param value The value.
     */
    constexpr explicit StrongType(value_type val = {}) : value{val} {}

    /**
     * \brief Comparison operator for values of the same strong type.
     *
     * \param other The other value to compare against.
     * \return Comparison result.
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const StrongType &other) const = default;
};

template<typename T, typename Tag>
std::ostream &operator<<(std::ostream &os, const StrongType<T, Tag> &type) {
    return os << type.value;
}

/**
 * \brief The evaluation score for a chess position.
 */
struct [[nodiscard]] Score : public StrongType<std::int16_t, struct ScoreTag> {
    using StrongType::StrongType;

    constexpr auto operator-() const -> Score { return Score{static_cast<value_type>(-value)}; }
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

    constexpr auto negative() const -> bool { return value < 0; }

    static const Score Infinity;    ///< A Score regarded as infinity.
    static const Score NegInfinity; ///< A Score regarded as negative inifity.
    static const Score Mate;        ///< The score for a mate in the current position.
};

/**
 * \brief Bounds for evaluation during alpha-beta-search.
 */
struct Bounds {
    Score alpha{Score::NegInfinity}; ///< α bound
    Score beta{Score::Infinity};     ///< β bound

    auto swap() -> Bounds { return {-beta, -alpha}; }
};

/**
 * \brief A search depth or a number of steps.
 *
 */
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

    static const Depth Zero;         ///< Pre-defined zero depth.
    static const Depth Step;         ///< Pre-defines depth of one.
    static const Depth Infinite;     ///< Very large depth, nearly infinite.
    static const Depth MaxMateDepth; ///< The maximum depth in the search for a mate.
};

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

[[nodiscard]] constexpr auto operator+(Score lhs, Depth rhs) -> Score {
    auto value = static_cast<Score::value_type>(lhs.value + rhs.value);
    return Score{value};
}

[[nodiscard]] constexpr auto operator-(Depth lhs, Depth rhs) -> Depth {
    Depth result{lhs};
    return result -= rhs;
}

[[nodiscard]] constexpr auto operator+(Depth lhs, Depth rhs) -> Depth {
    Depth result{lhs};
    return result += rhs;
}

/**
 * \brief Checks, if the given score describes a winning position.
 *
 * A winning position is a position with a (forced) mate.
 * \param score The score.
 * \return If the score describes a winning position.
 */
constexpr auto is_winning_score(Score score) -> bool {
    return score >= (Score::Mate - Depth::MaxMateDepth);
}

/**
 * \brief Checks, if the given score describes a losing position.
 *
 * A losing position is a position with a (forced) mate for the opponent.
 * \param score The score.
 * \return If the score describes a losing position.
 */
constexpr auto is_losing_score(Score score) -> bool {
    return score <= -(Score::Mate - Depth::MaxMateDepth);
}

/**
 * \brief Checks, if the given score is for a winning or losing position.
 *
 * Decisive means either winning or losing.
 * \param score The score.
 * \return If the score describes a decisive position.
 */
constexpr auto is_decisive_score(Score score) -> bool {
    return is_winning_score(score) || is_losing_score(score);
}

/**
 * \brief Extract the number of plys from a mate score.
 *
 * A mate score is higher for mates in less half-moves. This function extracts
 * the number of half-moves (plys) needed to reach the mate from the score.
 * \param score The score.
 * \return The number of half-moves (plys) to reach the mate.
 */
constexpr auto ply_to_mate(Score score) -> Depth {
    if (score.negative()) {
        return Depth{(Score::Mate + score).value};
    }
    return Depth{(Score::Mate - score).value};
}

/**
 * \brief A move combined with a score.
 */
struct EvaluatedMove {
    chesscore::Move move;            ///< The move.
    Score score{Score::NegInfinity}; ///< Score for the move.
};

/**
 * \brief Statistics of the last search.
 */
struct SearchStats {
    std::int64_t nodes{0};                  ///< Number of noes evaluated during search.
    std::int64_t cutoffs{0};                ///< Number of branches cut off during search.
    EvaluatedMove best_move;                ///< Best move so far.
    Depth depth;                            ///< Depth reached so far.
    std::chrono::milliseconds elapsed_time; ///< Time spent so far.

    auto calculate_nps() const -> std::optional<std::uint64_t> {
        const auto ms_count = elapsed_time.count();
        if (ms_count != 0) {
            return nodes * 1000 / ms_count;
        } else {
            return {};
        }
    }
};

using SearchEndedCallback = std::function<void(const EvaluatedMove &)>;
using SearchProgressCalback = std::function<void(SearchStats)>;

/**
 * \brief Parameters for stopping criteria of the search.
 *
 * Describes parameters that influence the stopping criteria evaluated during
 * the search for a best move in a position.
 */
struct StopParameters {
    /// Maximum allowed search time.
    std::chrono::milliseconds max_search_time{std::chrono::milliseconds::max()};
    /// Maximum allowed search depth. 0 means "no restriction"
    Depth max_search_depth = Depth::Zero;
    /// Maximum number of nodes to evaluate. 0 means "no restriction"
    std::int64_t max_search_nodes{0};
};

auto to_string(const StopParameters &params) -> std::string;

} // namespace chessengine

#endif
