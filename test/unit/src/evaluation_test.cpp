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

TEST_CASE("Evaluation.Material.Piece Values", "[evaluation]") {
    EvaluatorConfig config{.piece_values = {Score{100}, Score{500}, Score{300}, Score{300}, Score{900}, Score{0}}};

    CHECK(config.piece_value(PieceType::Pawn) == Score{100});
    CHECK(config.piece_value(PieceType::Rook) == Score{500});
    CHECK(config.piece_value(PieceType::Knight) == Score{300});
    CHECK(config.piece_value(PieceType::Bishop) == Score{300});
    CHECK(config.piece_value(PieceType::Queen) == Score{900});
    CHECK(config.piece_value(PieceType::King) == Score{0});
}

TEST_CASE("Evaluation.Material.Position", "[evaluation]") {
    Position position{FenString{"1k2q3/3r1pn1/2b4p/4n3/1P6/2N1B1PB/P7/2Q3KR w - - 0 1"}};
    EvaluatorConfig config{.piece_values = {Score{100}, Score{500}, Score{300}, Score{300}, Score{900}, Score{0}}};
    Evaluator evaluator{config};

    CHECK(evaluator.countup_material(position, Color::White) == Score{2600});
    CHECK(evaluator.countup_material(position, Color::Black) == Score{2500});
}
