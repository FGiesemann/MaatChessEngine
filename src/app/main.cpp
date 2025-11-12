/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <iostream>
#include <thread>

#include "chessengine/chess_engine.h"
#include "chessengine/uci_engine.h"

auto main() -> int {
    chessengine::UCIEngine<chessengine::ChessEngine> engine{std::cin, std::cout};
    engine.run();
    return 0;
}
