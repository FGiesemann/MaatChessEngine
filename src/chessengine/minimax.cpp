/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/minimax.h"

namespace chessengine {

auto MinimaxSearch::best_move(const chesscore::Position &position) const -> chesscore::Move {
    m_position = position;

    chesscore::Move best_move{};
    int best_value = std::numeric_limits<int>::min();
    for (const auto &move : m_position.all_legal_moves()) {
        m_position.make_move(move);
        const auto value = minimax(m_config.max_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
        m_position.unmake_move(move);
        if (value > best_value) {
            best_move = move;
            best_value = value;
        }
    }

    return best_move;
}

auto MinimaxSearch::minimax(int depth, int alpha, int beta, bool maximizing_player) const -> int {
    if (depth == 0) {
        return m_evaluator.evaluate(m_position);
    }
    const auto all_legal_moves = m_position.all_legal_moves();
    if (all_legal_moves.empty()) {
        return m_evaluator.evaluate(m_position);
    }

    if (maximizing_player) {
        int best_value = std::numeric_limits<int>::min();
        for (const auto &move : all_legal_moves) {
            m_position.make_move(move);
            best_value = std::max(best_value, minimax(depth - 1, alpha, beta, false));
            m_position.unmake_move(move);
            alpha = std::max(alpha, best_value);
            if (beta <= alpha) {
                break;
            }
        }
        return best_value;
    } else {
        int best_value = std::numeric_limits<int>::max();
        for (const auto &move : all_legal_moves) {
            m_position.make_move(move);
            best_value = std::min(best_value, minimax(depth - 1, alpha, beta, true));
            m_position.unmake_move(move);
            beta = std::min(beta, best_value);
            if (beta <= alpha) {
                break;
            }
        }
        return best_value;
    }
}

} // namespace chessengine
