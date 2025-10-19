/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessengine/evaluation.h"
using namespace chessengine;
using namespace chesscore;

TEST_CASE("Evaluation.White mated", "[evaluation]") {
    const Evaluator evaluator{};
    const Position position{FenString{"8/8/8/8/6n1/8/6PP/1r4K1 w - - 0 1"}};
    CHECK(evaluator.evaluate(position, Color::White) == -Score::Mate);
    CHECK(evaluator.evaluate(position, Color::Black) == Score::Mate);
}

TEST_CASE("Evaluation.Black mated", "[evaluation]") {
    const Evaluator evaluator{};
    const Position position{FenString{"8/8/3K4/3B4/8/8/6Q1/3r2k1 b - - 0 1"}};
    CHECK(evaluator.evaluate(position, Color::White) == Score::Mate);
    CHECK(evaluator.evaluate(position, Color::Black) == -Score::Mate);
}

TEST_CASE("Evaluation.Material", "[evaluation]") {
    FAIL();
}
