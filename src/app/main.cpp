/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <iostream>
#include <thread>

#include "chessengine/chess_engine.h"
#include "chessengine/uci_adapter.h"

auto main() -> int {
    chessengine::UCIAdapter<chessengine::ChessEngine> uci_adapter{std::cin, std::cout};

    auto config = uci_adapter.engine().config();
    config.search_config.iterative_deepening = true;
    uci_adapter.engine().set_config(config);

    uci_adapter.run();
    return 0;
}
