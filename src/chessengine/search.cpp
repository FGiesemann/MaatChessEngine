/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/search.h"
#include "chessengine/minimax.h"

namespace chessengine {

SearchStats::SearchStats(const SearchStats &other) : nodes{other.nodes.load()}, cutoffs{other.cutoffs.load()} {}

auto SearchStats::operator=(const SearchStats &rhs) -> SearchStats & {
    nodes = rhs.nodes.load();
    cutoffs = rhs.cutoffs.load();
    return *this;
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
