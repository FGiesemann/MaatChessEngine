/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_MAAT_UCIENGINE_H
#define CHESS_ENGINE_MAAT_UCIENGINE_H

#include <algorithm>
#include <condition_variable>
#include <iosfwd>
#include <mutex>
#include <sstream>
#include <string>

#include <chesscore/fen.h>
#include <chessuci/engine_handler.h>

#include "chessengine/chess_engine.h"

namespace chessengine {

namespace detail {

static inline auto position_to_string(const chesscore::Position &position) -> std::string {
    std::ostringstream result;
    const auto &bitboard = position.board();
    result << "  a b c d e f g h\n";
    for (int rank = chesscore::Rank::max_rank; rank >= chesscore::Rank::min_rank; --rank) {
        result << rank << ' ';
        for (int file = chesscore::File::min_file; file <= chesscore::File::max_file; ++file) {
            const chesscore::Square square{file, rank};
            const auto piece = bitboard.get_piece(square);
            if (piece) {
                result << piece.value().piece_char();
            } else {
                result << "·";
            }
            result << ' ';
        }
        result << rank << '\n';
    }
    result << "  a b c d e f g h\n";
    return result.str();
}

} // namespace detail

template<typename EngineT = ChessEngine>
class UCIEngine {
public:
    explicit UCIEngine(std::istream &in_stream = std::cin, std::ostream &out_stream = std::cout) : m_handler{in_stream, out_stream} { register_callbacks(); }

    auto run() -> void {
        m_handler.start();
        std::unique_lock<std::mutex> lock{m_quit_mutex};
        m_quit_signal.wait(lock);
    }
    auto is_running() const -> bool { return m_handler.is_running(); }

    auto engine() const -> const EngineT & { return m_engine; }
    auto engine() -> EngineT & { return m_engine; }

    auto uci_callback() -> void {
        m_handler.send_id({.name = ChessEngine::identifier, .author = ChessEngine::author});
        m_handler.send_uciok();
    }
    auto debug_callback(bool debug_on) -> void { m_engine.set_debugging(debug_on); }
    auto is_ready_callback() -> void { m_handler.send_readyok(); }
    auto set_option_callback([[maybe_unused]] const chessuci::setoption_command &command) -> void {
        // currently no options
    }
    auto uci_new_game_callback() -> void { m_engine.new_game(); }
    auto position_callback(const chessuci::position_command &command) -> void {
        if (m_position_setup != command.fen) {
            m_engine.set_position(construct_position(command));
        } else {
            const auto mismatch = std::ranges::mismatch(m_move_list, command.moves);
            if (mismatch.in1 == m_move_list.end()) {
                std::for_each(mismatch.in2, command.moves.end(), [this](const chessuci::UCIMove &move) -> void {
                    const auto matched_move = chessuci::convert_legal_move(move, m_engine.position());
                    if (!matched_move.has_value()) {
                        throw chessuci::UCIError{"Invalid move " + to_string(move)};
                    }
                    m_engine.play_move(matched_move.value());
                    m_move_list.push_back(move);
                });
            } else {
                m_engine.set_position(construct_position(command));
            }
        }
    }
    auto go_callback([[maybe_unused]] const chessuci::go_command &command) -> void {
        // evaluate command parameters and start search
        m_engine.start_search();
    }
    auto stop_callback() -> void {
        m_engine.stop_search();
        chessuci::bestmove_info move_info{.bestmove = chessuci::UCIMove{m_engine.best_move()}, .pondermove = {}};
        m_handler.send_bestmove(move_info);
    }
    auto ponder_hit_callback() -> void {
        // TODO
    }
    auto quit_callback() -> void {
        m_handler.stop();
        m_quit_signal.notify_one();
    }

    auto display_board() -> void { m_handler.send_raw(detail::position_to_string(m_engine.position())); }

    auto unknown_command_handler(const chessuci::TokenList &tokens) -> void { m_handler.send_raw("unknown command " + tokens[0]); }

    auto construct_position(const chessuci::position_command &command) -> chesscore::Position {
        m_position_setup = command.fen;
        m_move_list.clear();
        const auto fen = command.fen == chessuci::position_command::startpos ? chesscore::FenString::starting_position() : chesscore::FenString{command.fen};
        auto position = chesscore::Position{fen};
        std::ranges::for_each(command.moves, [&position, this](const chessuci::UCIMove &move) -> void {
            const auto matched_move = chessuci::convert_legal_move(move, position);
            if (matched_move.has_value()) {
                position.make_move(matched_move.value());
                m_move_list.push_back(move);
            } else {
                throw chessuci::UCIError{"Invalid move " + to_string(move)};
            }
        });
        return position;
    }
private:
    chessuci::UCIEngineHandler m_handler;
    EngineT m_engine;
    std::string m_position_setup{};
    std::vector<chessuci::UCIMove> m_move_list{}; ///< Moves played so far.
    std::condition_variable m_quit_signal;
    std::mutex m_quit_mutex;

    auto register_callbacks() -> void {
        m_handler.on_uci([this]() -> void { uci_callback(); });
        m_handler.on_position([this](const chessuci::position_command &command) -> void { position_callback(command); });
        m_handler.on_quit([this]() -> void { quit_callback(); });

        m_handler.register_command("d", [this](const chessuci::TokenList &) -> void { display_board(); });
        m_handler.on_unknown_command([this](const chessuci::TokenList &tokens) -> void { unknown_command_handler(tokens); });
    }
};

} // namespace chessengine

#endif
