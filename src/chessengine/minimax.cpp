/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/minimax.h"

namespace chessengine {

auto MinimaxSearch::best_move(const chesscore::Position &position) const -> chesscore::Move {
    m_position = position;
    m_color_to_evaluate = m_position.side_to_move();

    chesscore::Move best_move{};
    auto best_value = Evaluator::NegInfinity;
    for (const auto &move : m_position.all_legal_moves()) {
        m_position.make_move(move);
        const auto value = minimax(m_config.max_depth, Evaluator::NegInfinity, Evaluator::Infinity, false);
        m_position.unmake_move(move);
        if (value > best_value) {
            best_move = move;
            best_value = value;
        }
    }

    return best_move;
}

auto MinimaxSearch::minimax(int depth, Score alpha, Score beta, bool maximizing_player) const -> Score {
    if (depth == 0) {
        return m_evaluator.evaluate(m_position, m_color_to_evaluate);
    }
    const auto all_legal_moves = m_position.all_legal_moves();
    if (all_legal_moves.empty()) {
        return m_evaluator.evaluate(m_position, m_color_to_evaluate);
    }

    if (maximizing_player) {
        auto best_value = Evaluator::NegInfinity;
        for (const auto &move : all_legal_moves) {
            m_position.make_move(move);
            best_value = std::max(best_value, minimax(depth - 1, alpha, beta, false));
            m_position.unmake_move(move);
            alpha = std::max(alpha, best_value);
            if (m_config.use_alpha_beta_pruning && (beta <= alpha)) {
                break;
            }
        }
        return best_value;
    } else {
        auto best_value = Evaluator::Infinity;
        for (const auto &move : all_legal_moves) {
            m_position.make_move(move);
            best_value = std::min(best_value, minimax(depth - 1, alpha, beta, true));
            m_position.unmake_move(move);
            beta = std::min(beta, best_value);
            if (m_config.use_alpha_beta_pruning && (beta <= alpha)) {
                break;
            }
        }
        return best_value;
    }
}

} // namespace chessengine
