/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_EVALUATION_H
#define CHESSENGINE_EVALUATION_H

#include <chesscore/position.h>

namespace chessengine {

class EvaluatorConfig {
public:
    auto empty_board_value() const -> int { return 0; }
};

class Evaluator {
public:
    explicit Evaluator(EvaluatorConfig config) : m_config{std::move(config)} {}
    auto evaluate(const chesscore::Position &position) const -> int;

    static constexpr int Infinity = std::numeric_limits<int>::max();
    static constexpr int NegInfinity = std::numeric_limits<int>::min();
private:
    EvaluatorConfig m_config;
};

} // namespace chessengine

#endif
