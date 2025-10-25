/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/minimax.h"

namespace chessengine {

auto MinimaxSearch::best_move(const chesscore::Position &position, Depth depth) const -> EvaluatedMove {
    m_stats = SearchStats{};
    m_position = position;
    m_color_to_evaluate = m_position.side_to_move();

    chesscore::Move best_move{};
    auto best_value = Score::NegInfinity;
    m_stats.nodes += 1;
    const auto moves = moves_to_search();
    for (const auto &move : moves) {
        m_position.make_move(move);
        auto value = minimax(depth - Depth::Step, Score::NegInfinity, Score::Infinity, false);
        if (is_winning_score(value)) {
            value = value - Depth::Step;
        } else if (is_losing_score(value)) {
            value = value + Depth::Step;
        }
        m_position.unmake_move(move);
        if (value > best_value) {
            best_move = move;
            best_value = value;
        }
    }

    return {.move = best_move, .score = best_value};
}

auto MinimaxSearch::minimax(Depth depth, Score alpha, Score beta, bool maximizing_player) const -> Score {
    m_stats.nodes += 1;
    if (depth == Depth::Zero) {
        return m_evaluator.evaluate(m_position, m_color_to_evaluate);
    }
    const auto moves = moves_to_search();
    if (moves.empty()) {
        return m_evaluator.evaluate(m_position, m_color_to_evaluate);
    }

    if (maximizing_player) {
        auto best_value = Score::NegInfinity;
        for (const auto &move : moves) {
            m_position.make_move(move);
            auto value = minimax(depth - Depth::Step, alpha, beta, false);
            if (is_winning_score(value)) {
                value = value - Depth::Step;
            } else if (is_losing_score(value)) {
                value = value + Depth::Step;
            }
            best_value = std::max(best_value, value);
            m_position.unmake_move(move);
            alpha = std::max(alpha, best_value);
            if (m_config.use_alpha_beta_pruning && (beta <= alpha)) {
                m_stats.cutoffs += 1;
                break;
            }
        }
        return best_value;
    } else {
        auto best_value = Score::Infinity;
        for (const auto &move : moves) {
            m_position.make_move(move);
            auto value = minimax(depth - Depth::Step, alpha, beta, true);
            if (is_winning_score(value)) {
                value = value - Depth::Step;
            } else if (is_losing_score(value)) {
                value = value + Depth::Step;
            }
            best_value = std::min(best_value, value);
            m_position.unmake_move(move);
            beta = std::min(beta, best_value);
            if (m_config.use_alpha_beta_pruning && (beta <= alpha)) {
                m_stats.cutoffs += 1;
                break;
            }
        }
        return best_value;
    }
}

auto MinimaxSearch::moves_to_search() const -> chesscore::MoveList {
    auto moves = m_position.all_legal_moves();
    if (m_config.use_move_ordering) {
        sort_moves(moves, m_move_ordering);
    }
    return moves;
}

} // namespace chessengine
