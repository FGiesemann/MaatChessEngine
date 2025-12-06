/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/uci_engine.h"

#include <chesscore/position.h>
#include <chessuci/protocol.h>

#include <catch2/catch_all.hpp>

using namespace chesscore;

TEST_CASE("UCIEngine.Construct Position.Start Position", "[uci_engine]") {
    chessuci::position_command command{.fen = chessuci::position_command::startpos, .moves = {}};

    const auto constructed = chessengine::construct_position(command);
    CHECK(constructed.first == Position::start_position());
    CHECK(constructed.second.empty());
}

TEST_CASE("UCIEngine.Construct Position.Alternative start", "[uci_engine]") {
    chessuci::position_command command{
        .fen = "r3k2r/pppq1ppp/2n5/3bp3/3P4/2N5/PPPQPPPP/R3K2R w KQkq - 3 12",
        .moves = {},
    };

    const auto constructed = chessengine::construct_position(command);
    CHECK(constructed.first == Position{FenString{"r3k2r/pppq1ppp/2n5/3bp3/3P4/2N5/PPPQPPPP/R3K2R w KQkq - 3 12"}});
    CHECK(constructed.second.empty());
}

TEST_CASE("UCIEngine.Construct Position.Single move", "[uci_engine]") {
    chessuci::position_command command{
        .fen = chessuci::position_command::startpos,
        .moves = {chessuci::UCIMove{Square::E2, Square::E4}},
    };

    const auto constructed = chessengine::construct_position(command);
    auto reference = Position::start_position();
    reference.make_move(Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn});

    CHECK(constructed.first == reference);
    REQUIRE(constructed.second.size() == 1);
    CHECK(constructed.second[0] == chessuci::UCIMove{Square::E2, Square::E4});
}

TEST_CASE("UCIEngine.Construct Position.Multiple moves", "[uci_engine]") {
    chessuci::position_command command{
        .fen = chessuci::position_command::startpos,
        .moves = {
            chessuci::UCIMove{Square::E2, Square::E4},
            chessuci::UCIMove{Square::E7, Square::E5},
            chessuci::UCIMove{Square::G1, Square::F3},
        }
    };

    const auto constructed = chessengine::construct_position(command);
    auto reference = Position::start_position();
    reference.make_move(Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn});
    reference.make_move(Move{.from = Square::E7, .to = Square::E5, .piece = Piece::BlackPawn});
    reference.make_move(Move{.from = Square::G1, .to = Square::F3, .piece = Piece::WhiteKnight});
    CHECK(constructed.first == reference);
    REQUIRE(constructed.second.size() == 3);
    CHECK(constructed.second[0] == chessuci::UCIMove{Square::E2, Square::E4});
    CHECK(constructed.second[1] == chessuci::UCIMove{Square::E7, Square::E5});
    CHECK(constructed.second[2] == chessuci::UCIMove{Square::G1, Square::F3});
}
