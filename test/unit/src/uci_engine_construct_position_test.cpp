/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/uci_engine.h"

#include <chesscore/position.h>
#include <chessuci/protocol.h>

#include <catch2/catch_all.hpp>

TEST_CASE("UCIEngine.Construct Position.Start Position", "[uci_engine]") {
    chessuci::position_command command{chessuci::position_command::startpos, {}};

    const auto constructed = chessengine::construct_position(command);
    CHECK(constructed.first == chesscore::Position::start_position());
    CHECK(constructed.second.empty());
}
