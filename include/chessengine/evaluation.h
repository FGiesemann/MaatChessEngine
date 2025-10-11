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
private:
    EvaluatorConfig m_config;
};

} // namespace chessengine

#endif
