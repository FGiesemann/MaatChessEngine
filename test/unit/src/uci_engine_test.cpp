/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessengine/test_engine.h"
#include "chessengine/uci_engine.h"

using namespace chessengine;
using namespace chesscore;
using namespace chessuci;

TEST_CASE("UCIEngine.Position.Continuation", "[uci_engine]") {
    auto uci_engine = UCIEngine<TestEngine>{};
    auto &test_engine = uci_engine.engine();

    Position pos = Position::start_position();
    std::vector<Position> position_returns;
    position_returns.push_back(pos);
    pos.make_move(Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn});
    position_returns.push_back(pos);
    test_engine.set_position_returns(position_returns);

    const auto pos1 = position_command{.fen = position_command::startpos, .moves = {}};
    const auto pos2 = position_command{.fen = position_command::startpos, .moves = {UCIMove{Square::E2, Square::E4}}};
    const auto pos3 = position_command{.fen = position_command::startpos, .moves = {UCIMove{Square::E2, Square::E4}, UCIMove{Square::E7, Square::E5}}};

    uci_engine.position_callback(pos1);
    uci_engine.position_callback(pos2);
    uci_engine.position_callback(pos3);

    const auto &log = test_engine.call_log();
    REQUIRE(log.size() == 5);
    CHECK(std::holds_alternative<TestEngine::set_position_call>(log[0]));
    CHECK(std::get<TestEngine::set_position_call>(log[0]).position == position_returns[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[1]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[2]));
    CHECK(std::get<TestEngine::play_move_call>(log[2]).move == Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn});
    CHECK(std::holds_alternative<TestEngine::position_call>(log[3]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[4]));
    CHECK(std::get<TestEngine::play_move_call>(log[4]).move == Move{.from = Square::E7, .to = Square::E5, .piece = Piece::BlackPawn});
}
