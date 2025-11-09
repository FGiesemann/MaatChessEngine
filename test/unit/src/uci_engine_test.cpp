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

namespace {

struct TestCase {
    std::vector<Position> positions;
    std::vector<UCIMove> uci_moves;
    std::vector<Move> moves;
    std::vector<position_command> position_commands;
};

auto setup_test(const std::string &start_pos, const std::vector<UCIMove> &uci_moves) -> TestCase {
    TestCase test_case;
    const auto start_fen = start_pos == position_command::startpos ? FenString::starting_position() : FenString{start_pos};
    Position position{start_fen};
    std::vector<UCIMove> collected_moves{};
    test_case.position_commands.emplace_back(start_pos, std::vector<UCIMove>{});
    test_case.positions.push_back(position);

    for (auto current_move_it = uci_moves.begin(); current_move_it != uci_moves.end(); ++current_move_it) {
        test_case.uci_moves.push_back(*current_move_it);
        const auto opt_move = convert_move(*current_move_it, position);
        if (!opt_move.has_value()) {
            throw std::runtime_error("Invalid move " + to_string(*current_move_it));
        }
        position.make_move(*opt_move);
        test_case.moves.push_back(*opt_move);
        test_case.positions.push_back(position);
        collected_moves.push_back(*current_move_it);
        test_case.position_commands.emplace_back(start_pos, collected_moves);
    }
    return test_case;
}

} // namespace

TEST_CASE("UCIEngine.Position.Continuation", "[uci_engine]") {
    auto uci_engine = UCIEngine<TestEngine>{};
    auto &test_engine = uci_engine.engine();

    const auto test_case = setup_test(position_command::startpos, {UCIMove{Square::E2, Square::E4}, UCIMove{Square::E7, Square::E5}});
    test_engine.set_position_returns(test_case.positions);

    std::ranges::for_each(test_case.position_commands, [&](const auto &cmd) -> void { uci_engine.position_callback(cmd); });
    const auto &log = test_engine.call_log();
    REQUIRE(log.size() == 5);
    CHECK(std::holds_alternative<TestEngine::set_position_call>(log[0]));
    CHECK(std::get<TestEngine::set_position_call>(log[0]).position == test_case.positions[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[1]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[2]));
    CHECK(std::get<TestEngine::play_move_call>(log[2]).move == test_case.moves[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[3]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[4]));
    CHECK(std::get<TestEngine::play_move_call>(log[4]).move == test_case.moves[1]);
}

TEST_CASE("UCIEngine.Position.Skip Call", "[uci_engine]") {
    auto uci_engine = UCIEngine<TestEngine>{};
    auto &test_engine = uci_engine.engine();

    const auto test_case = setup_test(
        "r4rnk/1pp4p/3p4/3P1b2/1PPbpBPq/8/2QNB1KP/1R3R2 b - - 0 23",
        {UCIMove{Square::G8, Square::F6}, UCIMove{Square::B4, Square::B5}, UCIMove{Square::F6, Square::G4}, UCIMove{Square::E2, Square::G4}, UCIMove{Square::H4, Square::G4}}
    );
    test_engine.set_position_returns(test_case.positions);
    for (std::size_t i = 0; i < test_case.position_commands.size(); ++i) {
        if (i == 3) {
            continue;
        }
        uci_engine.position_callback(test_case.position_commands[i]);
    }
    const auto &log = test_engine.call_log();
    REQUIRE(log.size() == 11);
    CHECK(std::holds_alternative<TestEngine::set_position_call>(log[0]));
    CHECK(std::get<TestEngine::set_position_call>(log[0]).position == test_case.positions[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[1]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[2]));
    CHECK(std::get<TestEngine::play_move_call>(log[2]).move == test_case.moves[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[3]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[4]));
    CHECK(std::get<TestEngine::play_move_call>(log[4]).move == test_case.moves[1]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[5]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[6]));
    CHECK(std::get<TestEngine::play_move_call>(log[6]).move == test_case.moves[2]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[7]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[8]));
    CHECK(std::get<TestEngine::play_move_call>(log[8]).move == test_case.moves[3]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[9]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[10]));
    CHECK(std::get<TestEngine::play_move_call>(log[10]).move == test_case.moves[4]);
}

TEST_CASE("UCIEngine.Position.Jump Back", "[uci_engine]") {
    auto uci_engine = UCIEngine<TestEngine>{};
    auto &test_engine = uci_engine.engine();

    const auto test_case = setup_test(
        "r4rnk/1pp4p/3p4/3P1b2/1PPbpBPq/8/2QNB1KP/1R3R2 b - - 0 23", {UCIMove{Square::G8, Square::F6}, UCIMove{Square::B4, Square::B5}, UCIMove{Square::F6, Square::G4}}
    );
    test_engine.set_position_returns(test_case.positions);
    std::ranges::for_each(test_case.position_commands, [&](const auto &cmd) -> void { uci_engine.position_callback(cmd); });
    uci_engine.position_callback(test_case.position_commands[1]);
    const auto &log = test_engine.call_log();
    REQUIRE(log.size() == 8);
    CHECK(std::holds_alternative<TestEngine::set_position_call>(log[0]));
    CHECK(std::get<TestEngine::set_position_call>(log[0]).position == test_case.positions[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[1]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[2]));
    CHECK(std::get<TestEngine::play_move_call>(log[2]).move == test_case.moves[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[3]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[4]));
    CHECK(std::get<TestEngine::play_move_call>(log[4]).move == test_case.moves[1]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[5]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[6]));
    CHECK(std::get<TestEngine::play_move_call>(log[6]).move == test_case.moves[2]);
    CHECK(std::holds_alternative<TestEngine::set_position_call>(log[7]));
    CHECK(std::get<TestEngine::set_position_call>(log[7]).position == test_case.positions[1]);
}

TEST_CASE("UCIEngine.Position.Switch Line", "[uci_engine]") {
    auto uci_engine = UCIEngine<TestEngine>{};
    auto &test_engine = uci_engine.engine();

    const auto test_case1 = setup_test(
        "r4rnk/1pp4p/3p4/3P1b2/1PPbpBPq/8/2QNB1KP/1R3R2 b - - 0 23", {UCIMove{Square::G8, Square::F6}, UCIMove{Square::B4, Square::B5}, UCIMove{Square::F6, Square::G4}}
    );
    const auto test_case2 = setup_test(
        "r4rnk/1pp4p/3p4/3P1b2/1PPbpBPq/8/2QNB1KP/1R3R2 b - - 0 23", {UCIMove{Square::G8, Square::F6}, UCIMove{Square::C4, Square::C5}, UCIMove{Square::D6, Square::C5}}
    );
    test_engine.set_position_returns(test_case1.positions);

    std::ranges::for_each(test_case1.position_commands, [&](const auto &cmd) -> void { uci_engine.position_callback(cmd); });
    uci_engine.position_callback(test_case2.position_commands[3]);

    const auto &log = test_engine.call_log();
    REQUIRE(log.size() == 8);
    CHECK(std::holds_alternative<TestEngine::set_position_call>(log[0]));
    CHECK(std::get<TestEngine::set_position_call>(log[0]).position == test_case1.positions[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[1]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[2]));
    CHECK(std::get<TestEngine::play_move_call>(log[2]).move == test_case1.moves[0]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[3]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[4]));
    CHECK(std::get<TestEngine::play_move_call>(log[4]).move == test_case1.moves[1]);
    CHECK(std::holds_alternative<TestEngine::position_call>(log[5]));
    CHECK(std::holds_alternative<TestEngine::play_move_call>(log[6]));
    CHECK(std::get<TestEngine::play_move_call>(log[6]).move == test_case1.moves[2]);
    CHECK(std::holds_alternative<TestEngine::set_position_call>(log[7]));
    CHECK(std::get<TestEngine::set_position_call>(log[7]).position == test_case2.positions[3]);
}
