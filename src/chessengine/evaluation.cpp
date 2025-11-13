/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/evaluation.h"

namespace chessengine {

auto Evaluator::evaluate(const chesscore::Position &position, chesscore::Color color) const -> Score {
    if (is_mate(position)) {
        return color == position.side_to_move() ? -Score::Mate : Score::Mate;
    }
    return countup_material(position, color) - countup_material(position, chesscore::other_color(color)) + evaluate_pieces_on_squares(position, color);
}

auto Evaluator::evaluate(const chesscore::Move &move) const -> Score {
    return get_capture_score(move) + get_promotion_score(move) + get_piece_movement_score(move);
}

auto Evaluator::is_mate(const chesscore::Position &position) -> bool {
    return position.check_state() == chesscore::CheckState::Checkmate;
}

auto Evaluator::countup_material(const chesscore::Position &position, chesscore::Color color) const -> Score {
    Score material{0};
    for (const auto piece_type : chesscore::all_piece_types) {
        material += m_config.piece_value(piece_type) * position.board().piece_count(chesscore::Piece{.type = piece_type, .color = color});
    }
    return material;
}

auto Evaluator::evaluate_pieces_on_squares(const chesscore::Position &position, chesscore::Color color) const -> Score {
    Score score{0};
    chesscore::Square square{chesscore::Square::A1};
    for (int i = 0; i < chesscore::Square::count; ++i) {
        const auto piece = position.board().get_piece(square);
        if (piece.has_value() && piece->color == color) {
            score += m_config.piece_on_square_value(piece.value(), square);
        }
        square += 1;
    }
    return score;
}

auto Evaluator::get_capture_score(const chesscore::Move &move) const -> Score {
    if (move.is_capture()) {
        return m_config.piece_value(move.captured.value().type) - m_config.piece_value(move.piece.type);
    }
    return Score{0};
}

auto Evaluator::get_promotion_score(const chesscore::Move &move) const -> Score {
    if (move.is_pawn_promotion()) {
        return m_config.pawn_promotion_score + m_config.piece_value(move.promoted.value().type) - m_config.piece_value(chesscore::PieceType::Pawn);
    }
    return Score{0};
}

auto Evaluator::get_piece_movement_score(const chesscore::Move &move) const -> Score {
    return m_config.piece_on_square_value(move.piece, move.to) - m_config.piece_on_square_value(move.piece, move.from);
}

} // namespace chessengine
