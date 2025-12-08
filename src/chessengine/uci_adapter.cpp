/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/uci_adapter.h"

namespace chessengine {

auto construct_position(const chessuci::position_command &command) -> std::pair<chesscore::Position, UCIMoveList> {
    UCIMoveList move_list{};
    const auto fen = (command.fen == chessuci::position_command::startpos) ? chesscore::FenString::starting_position() : chesscore::FenString{command.fen};
    auto position = chesscore::Position{fen};
    std::ranges::for_each(command.moves, [&position, &move_list](const chessuci::UCIMove &move) -> void {
        const auto matched_move = chessuci::convert_legal_move(move, position);
        if (matched_move.has_value()) {
            position.make_move(matched_move.value());
            move_list.push_back(move);
        } else {
            throw chessuci::UCIError{"Invalid move " + to_string(move)};
        }
    });
    return std::make_pair(position, move_list);
}

} // namespace chessengine
