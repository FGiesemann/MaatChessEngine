/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/evaluation.h"

namespace chessengine {

auto Evaluator::evaluate(const chesscore::Position &position, chesscore::Color color) const -> Score {
    return countup_material(position, color) - countup_material(position, chesscore::other_color(color));
}

auto Evaluator::countup_material(const chesscore::Position &position, chesscore::Color color) const -> Score {
    Score material{0};
    for (const auto piece_type : chesscore::all_piece_types) {
        material += m_config.piece_value(piece_type) * position.board().piece_count(chesscore::Piece{piece_type, color});
    }
    return material;
}

} // namespace chessengine
