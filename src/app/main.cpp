/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <iostream>
#include <thread>

#include "uci.h"

auto main() -> int {
    chessengine::maat::UCIEngine engine{std::cin, std::cout};
    engine.run();
    return 0;
}
