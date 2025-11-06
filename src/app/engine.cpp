/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "engine.h"

namespace chessengine::maat {

UCIEngine::UCIEngine(std::istream &in_stream, std::ostream &out_stream) : m_handler(in_stream, out_stream) {
    register_callbacks();
}

auto UCIEngine::run() -> void {
    m_handler.start();
    std::unique_lock lock{m_quit_mutex};
    m_quit_signal.wait(lock);
}

auto UCIEngine::register_callbacks() -> void {
    m_handler.on_uci([this]() -> void { uci_callback(); });
    m_handler.on_quit([this]() -> void { quit_callback(); });

    m_handler.on_unknown_command([this](const chessuci::TokenList &tokens) -> void { unknown_command_handler(tokens); });
}

auto UCIEngine::uci_callback() -> void {
    m_handler.send_id({.name = "Maat v0.1", .author = "Florian Giesemann"});
    m_handler.send_uciok();
}

auto UCIEngine::quit_callback() -> void {
    m_handler.stop();
    m_quit_signal.notify_one();
}

auto UCIEngine::unknown_command_handler(const chessuci::TokenList &tokens) -> void {
    m_handler.send_raw("unknown command " + tokens[0]);
}

} // namespace chessengine::maat
