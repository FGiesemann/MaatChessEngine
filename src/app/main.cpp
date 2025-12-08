/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/chess_engine.h"
#include "chessengine/logger.h"
#include "chessengine/uci_adapter.h"

#include <iostream>
#include <thread>

auto main(int argc, char *argv[]) -> int {
    chessengine::UCIAdapter<chessengine::ChessEngine> uci_adapter{std::cin, std::cout};

    auto config = uci_adapter.engine().config();
    config.search_config.iterative_deepening = true;
    uci_adapter.engine().set_config(config);

    if (argc > 1 && std::string(argv[1]) == "--debug") {
        chessengine::Logger::instance().enable("engine_debug.log");
    }

    uci_adapter.run();
    return 0;
}
