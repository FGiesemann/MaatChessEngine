/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_MAAT_ENGINE_H
#define CHESS_ENGINE_MAAT_ENGINE_H

#include <condition_variable>
#include <iosfwd>
#include <mutex>

#include <chessuci/engine_handler.h>

namespace chessengine::maat {

class Engine {};

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
    auto quit_callback() -> void;

    auto unknown_command_handler(const chessuci::TokenList &tokens) -> void;
};

} // namespace chessengine::maat

#endif
