/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_MAAT_UCIADAPTER_H
#define CHESS_ENGINE_MAAT_UCIADAPTER_H

#include "chessengine/chess_engine.h"
#include "chessengine/logger.h"

#include <chesscore/fen.h>
#include <chessuci/engine_handler.h>

#include <algorithm>
#include <condition_variable>
#include <iosfwd>
#include <mutex>
#include <sstream>
#include <string>

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

using UCIMoveList = std::vector<chessuci::UCIMove>;

auto construct_position(const chessuci::position_command &command) -> std::pair<chesscore::Position, UCIMoveList>;

template<typename EngineT = ChessEngine>
class UCIAdapter {
public:
    explicit UCIAdapter(std::istream &in_stream = std::cin, std::ostream &out_stream = std::cout) : m_handler{in_stream, out_stream} { register_callbacks(); }

    auto run() -> void {
        m_handler.start();
        std::unique_lock<std::mutex> lock{m_quit_mutex};
        m_quit_signal.wait(lock);
    }

    auto is_running() const -> bool { return m_handler.is_running(); }

    auto engine() const -> const EngineT & { return m_engine; }

    auto engine() -> EngineT & { return m_engine; }

    auto uci_callback() -> void {
        log_uci_out("sending UCI identification");
        m_handler.send_id({.name = ChessEngine::identifier, .author = ChessEngine::author});
        log_uci_out("sending uciok");
        m_handler.send_uciok();
    }

    auto debug_callback(bool debug_on) -> void {
        log_info("setting debug " + std::string(debug_on ? "on" : "off"));
        m_engine.set_debugging(debug_on);
    }

    auto is_ready_callback() -> void {
        log_uci_out("sending readyok");
        m_handler.send_readyok();
    }

    auto set_option_callback([[maybe_unused]] const chessuci::setoption_command &command) -> void {
        log_info_stream() << "request to set option '" << command.name << "' ignored";
        // currently no options
    }

    auto uci_new_game_callback() -> void {
        log_info("setting up new game");
        m_engine.new_game();
    }

    auto position_callback(const chessuci::position_command &command) -> void {
        if (m_position_setup != command.fen) {
            log_info("setting up position from new FEN");
            setup_position(command);
        } else {
            const auto mismatch = std::ranges::mismatch(m_move_list, command.moves);
            if (mismatch.in1 == m_move_list.end()) {
                log_info("applying new moves to current position");
                std::for_each(mismatch.in2, command.moves.end(), [this](const chessuci::UCIMove &move) -> void {
                    const auto matched_move = chessuci::convert_legal_move(move, m_engine.position());
                    if (!matched_move.has_value()) {
                        throw chessuci::UCIError{"Invalid move " + to_string(move)};
                    }
                    log_info("playing move: " + to_string(move));
                    m_engine.play_move(matched_move.value());
                    m_move_list.push_back(move);
                });
            } else {
                log_info("setting up position due to changes in move list");
                setup_position(command);
            }
        }
    }

    auto go_callback(const chessuci::go_command &command) -> void {
        log_uci_in(to_string(command));
        StopParameters stop_params;
        stop_params.max_search_depth = Depth{static_cast<Depth::value_type>(command.depth.value_or(0))};
        stop_params.max_search_nodes = command.nodes.value_or(0);
        stop_params.max_search_time = compute_target_movetime(command);
        log_info_stream() << "starting search with stopping criteria: " << to_string(stop_params);
        m_engine.start_search(stop_params);
    }

    auto stop_callback() -> void {
        log_info("stop requested");
        m_engine.stop_search();
        const auto &evaluated_move = m_engine.best_move();
        const chessuci::UCIMove move{
            evaluated_move.move.from, evaluated_move.move.to,
            evaluated_move.move.promoted.has_value() ? std::optional<chesscore::PieceType>{evaluated_move.move.promoted.value().type} : std::nullopt
        };
        chessuci::bestmove_info move_info{.bestmove = move, .pondermove = {}};
        log_uci_out_stream() << "best move " << to_string(move) << "; value " << evaluated_move.score.value;
        m_handler.send_bestmove(move_info);
    }

    auto ponder_hit_callback() -> void {
        log_info("ponderhit ignored");
        // TODO
    }

    auto quit_callback() -> void {
        log_info("requested to quit");
        m_handler.stop();
        m_quit_signal.notify_one();
    }

    auto display_board() -> void { m_handler.send_raw(detail::position_to_string(m_engine.position())); }

    auto unknown_command_handler(const chessuci::TokenList &tokens) -> void { log_error_stream() << "unknown command '" << tokens[0] << '\''; }

    auto setup_position(const chessuci::position_command &command) -> void {
        m_position_setup = command.fen;
        const auto constructed = construct_position(command);
        m_engine.set_position(constructed.first);
        m_move_list = constructed.second;
    }

    auto engine_finished_search(const EvaluatedMove &move) -> void {
        chessuci::bestmove_info move_info{.bestmove = chessuci::UCIMove{move.move}, .pondermove = {}};
        log_info_stream() << "engine finished search: best move " << to_string(move.move) << "; value " << move.score.value
                          << "; pondermove = " << (move_info.pondermove.has_value() ? to_string(move_info.pondermove.value()) : "none");
        m_handler.send_bestmove(move_info);
    }

    auto engine_search_progress(SearchStats search_stats) -> void {
        chessuci::search_info info{};
        info.currmove = chessuci::UCIMove{search_stats.best_move.move};
        info.depth = search_stats.depth.value;
        info.nodes = search_stats.nodes;
        log_info_stream() << "search progress " << to_string(info.currmove.value()) << ", depth " << info.depth.value() << ", nodes " << info.nodes.value();
        m_handler.send_info(info);
    }
private:
    chessuci::UCIEngineHandler m_handler;
    EngineT m_engine;
    std::string m_position_setup{};
    UCIMoveList m_move_list{}; ///< Moves played so far.
    std::condition_variable m_quit_signal;
    std::mutex m_quit_mutex;

    static constexpr int sudden_death_moves{40};
    static constexpr std::int64_t search_stop_buffer{50};

    auto register_callbacks() -> void {
        m_handler.on_uci([this]() -> void { uci_callback(); });
        m_handler.on_isready([this]() -> void { is_ready_callback(); });
        m_handler.on_position([this](const chessuci::position_command &command) -> void { position_callback(command); });
        m_handler.on_go([this](const chessuci::go_command &command) -> void { go_callback(command); });
        m_handler.on_stop([this]() -> void { stop_callback(); });
        m_handler.on_ponderhit([this]() -> void { ponder_hit_callback(); });
        m_handler.on_quit([this]() -> void { quit_callback(); });

        m_handler.register_command("d", [this](const chessuci::TokenList &) -> void { display_board(); });
        m_handler.on_unknown_command([this](const chessuci::TokenList &tokens) -> void { unknown_command_handler(tokens); });

        m_engine.on_search_ended([this](const EvaluatedMove &move) -> void { engine_finished_search(move); });
        m_engine.on_search_progress([this](SearchStats search_stats) -> void { engine_search_progress(search_stats); });
    }

    auto is_white_to_move() -> bool { return m_engine.position().side_to_move() == chesscore::Color::White; }

    /**
     * \brief Compute the time for the next move.
     *
     * Handles time control for the next move from the parameters of the
     * go-command. It handles movetime, wtime, btime, winc, binc, movestogo and
     * also sudden death mode.
     * If the go_command does not specify any time control, the maximum possible
     * search time, which can be considered as "inifinite", is returned.
     * \param go_command The go command from the GUI.
     * \return The allocated search time for the next move.
     */
    auto compute_target_movetime(const chessuci::go_command &go_command) -> std::chrono::milliseconds {
        if (go_command.movetime.has_value()) {
            return std::chrono::milliseconds{go_command.movetime.value()};
        }
        if (!go_command.has_timing_control() || go_command.infinite) {
            // "Infinite" search, engine should be stopped explicitly
            return std::chrono::milliseconds::max();
        }

        const long long time_left = is_white_to_move() ? go_command.wtime.value_or(0) : go_command.btime.value_or(0);
        const long long increment = is_white_to_move() ? go_command.winc.value_or(0) : go_command.binc.value_or(0);

        const int moves_to_go = go_command.movestogo.value_or(sudden_death_moves);

        long long target_time_ms;

        if (time_left > 0 && moves_to_go > 0) {
            target_time_ms = time_left / moves_to_go;
        } else {
            target_time_ms = 0;
        }

        target_time_ms += (increment * 9) / 10;

        if (time_left > 0) {
            target_time_ms = std::min(target_time_ms, time_left / 2);
        }

        target_time_ms -= search_stop_buffer;

        if (target_time_ms <= 0 && time_left > 0) {
            target_time_ms = 1;
        } else if (time_left <= 0) {
            target_time_ms = 0;
        }

        return std::chrono::milliseconds(target_time_ms);
    }
};

} // namespace chessengine

#endif
