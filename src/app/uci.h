/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_MAAT_UCIENGINE_H
#define CHESS_ENGINE_MAAT_UCIENGINE_H

#include <condition_variable>
#include <iosfwd>
#include <mutex>

#include <chessuci/engine_handler.h>

#include "chessengine/engine.h"

namespace chessengine::maat {

class UCIEngine {
public:
    explicit UCIEngine(std::istream &in_stream = std::cin, std::ostream &out_stream = std::cout);

    auto run() -> void;
    auto is_running() const -> bool { return m_handler.is_running(); };
private:
    chessuci::UCIEngineHandler m_handler;
    Engine m_engine;
    std::condition_variable m_quit_signal;
    std::mutex m_quit_mutex;

    auto register_callbacks() -> void;

    auto uci_callback() -> void;
    auto debug_callback(bool debug_on) -> void;
    auto is_ready_callback() -> void;
    auto set_option_callback(const chessuci::setoption_command &command) -> void;
    auto uci_new_game_callback() -> void;
    auto position_callback(const chessuci::position_command &command) -> void;
    auto go_callback(const chessuci::go_command &command) -> void;
    auto stop_callback() -> void;
    auto ponder_hit_callback() -> void;
    auto quit_callback() -> void;

    auto display_board() -> void;

    auto unknown_command_handler(const chessuci::TokenList &tokens) -> void;
};

} // namespace chessengine::maat

#endif
