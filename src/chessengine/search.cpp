/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/search.h"
#include "chessengine/minimax.h"

namespace chessengine {

Search::Search(const Config &config) : m_config{config} {}

auto Search::best_move(const chesscore::Position &position) const -> EvaluatedMove {
    Evaluator evaluator{m_config.evaluator_config};
    MoveOrdering move_ordering{m_config.evaluator_config};
    MinimaxSearch minimax{m_config.minimax_config, evaluator, move_ordering};

    Depth current_depth = m_config.search_config.iterative_deepening ? Depth{1} : m_config.search_config.max_depth;
    EvaluatedMove current_best_move{};
    while (current_depth <= m_config.search_config.max_depth) {
        // feed in the best move from the previous iteration to search first...
        auto best_move = minimax.best_move(position, current_depth);
        m_search_stats = minimax.search_stats();
        if (is_winning_score(best_move.score)) {
            current_best_move = best_move;
            break;
        }
        if (best_move.score > current_best_move.score) {
            current_best_move = best_move;
        }
        if (!m_config.search_config.iterative_deepening) {
            break;
        }
        current_depth += Depth::Step;
    }

    return current_best_move;
}

auto MoveOrdering::operator()(const chesscore::Move &lhs, const chesscore::Move &rhs) const -> bool {
    Score lhs_score = evaluate_move(lhs);
    Score rhs_score = evaluate_move(rhs);

    return lhs_score > rhs_score;
}

auto MoveOrdering::evaluate_move(const chesscore::Move &move) const -> Score {
    return get_capture_score(move) + get_promotion_score(move) + get_piece_movement_score(move);
}

auto MoveOrdering::get_capture_score(const chesscore::Move &move) const -> Score {
    if (move.is_capture()) {
        return m_config.piece_value(move.captured.value().type) - m_config.piece_value(move.piece.type);
    }
    return Score{0};
}

auto MoveOrdering::get_promotion_score(const chesscore::Move &move) const -> Score {
    if (move.is_pawn_promotion()) {
        return m_config.pawn_promotion_score + m_config.piece_value(move.promoted.value().type) - m_config.piece_value(chesscore::PieceType::Pawn);
    }
    return Score{0};
}

auto MoveOrdering::get_piece_movement_score(const chesscore::Move &move) const -> Score {
    return m_config.piece_on_square_value(move.piece, move.to) - m_config.piece_on_square_value(move.piece, move.from);
}

auto sort_moves(chesscore::MoveList &moves, MoveOrdering ordering) -> void {
    std::ranges::sort(moves, ordering);
}

} // namespace chessengine
