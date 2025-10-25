/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessengine/evaluation.h"
using namespace chessengine;
using namespace chesscore;

namespace {
auto get_default_config() -> EvaluatorConfig;
}

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
    EvaluatorConfig config = get_default_config();

    CHECK(config.piece_value(PieceType::Pawn) == Score{100});
    CHECK(config.piece_value(PieceType::Rook) == Score{500});
    CHECK(config.piece_value(PieceType::Knight) == Score{300});
    CHECK(config.piece_value(PieceType::Bishop) == Score{300});
    CHECK(config.piece_value(PieceType::Queen) == Score{900});
    CHECK(config.piece_value(PieceType::King) == Score{0});
}

TEST_CASE("Evaluation.Material.Position", "[evaluation]") {
    Position position{FenString{"1k2q3/3r1pn1/2b4p/4n3/1P6/2N1B1PB/P7/2Q3KR w - - 0 1"}};
    EvaluatorConfig config = get_default_config();
    Evaluator evaluator{config};

    CHECK(evaluator.countup_material(position, Color::White) == Score{2600});
    CHECK(evaluator.countup_material(position, Color::Black) == Score{2500});
}

TEST_CASE("Evaluation.Pieces on Squares", "[evaluation]") {
    Position position1{FenString{"1k2q3/3r1pn1/2b4p/4n3/1P6/2N1B1PB/P7/2Q3KR w - - 0 1"}};
    Position position2{FenString{"4r3/1P3pp1/2n1N3/R4R2/3Bn3/Q1bk2P1/5rB1/NK2b2q w - - 0 1"}};
    EvaluatorConfig config = get_default_config();
    Evaluator evaluator{config};

    CHECK(evaluator.evaluate_pieces_on_squares(position1, Color::White) == Score{30});
    CHECK(evaluator.evaluate_pieces_on_squares(position1, Color::Black) == Score{50});

    CHECK(evaluator.evaluate_pieces_on_squares(position2, Color::White) == Score{45 - 35 + 15 - 5 - 10 + 30});
    CHECK(evaluator.evaluate_pieces_on_squares(position2, Color::Black) == Score{20 + 30 - 5 + 15 - 20 - 50});
}

namespace {

auto get_default_config() -> EvaluatorConfig {
    return EvaluatorConfig{
        .piece_values = {Score{100}, Score{500}, Score{300}, Score{300}, Score{900}, Score{0}},
        .piece_square_tables = {
            {Score{0},  Score{0},  Score{0},  Score{0},  Score{0},  Score{0},   Score{0},  Score{0},  Score{5},   Score{10}, Score{10}, Score{-20}, Score{-20},
             Score{10}, Score{10}, Score{5},  Score{5},  Score{-5}, Score{-10}, Score{0},  Score{0},  Score{-10}, Score{-5}, Score{5},  Score{0},   Score{0},
             Score{0},  Score{20}, Score{20}, Score{0},  Score{0},  Score{0},   Score{5},  Score{5},  Score{10},  Score{25}, Score{25}, Score{10},  Score{5},
             Score{5},  Score{10}, Score{10}, Score{20}, Score{30}, Score{30},  Score{20}, Score{10}, Score{10},  Score{50}, Score{50}, Score{50},  Score{50},
             Score{50}, Score{50}, Score{50}, Score{50}, Score{0},  Score{0},   Score{0},  Score{0},  Score{0},   Score{0},  Score{0},  Score{0}},
            {Score{0},  Score{0},  Score{0},  Score{5},  Score{5}, Score{0},  Score{0},  Score{0}, Score{-5}, Score{0}, Score{0},  Score{0},  Score{0},
             Score{0},  Score{0},  Score{-5}, Score{-5}, Score{0}, Score{0},  Score{0},  Score{0}, Score{0},  Score{0}, Score{-5}, Score{-5}, Score{0},
             Score{0},  Score{0},  Score{0},  Score{0},  Score{0}, Score{-5}, Score{-5}, Score{0}, Score{0},  Score{0}, Score{0},  Score{0},  Score{0},
             Score{-5}, Score{-5}, Score{0},  Score{0},  Score{0}, Score{0},  Score{0},  Score{0}, Score{-5}, Score{5}, Score{10}, Score{10}, Score{10},
             Score{10}, Score{10}, Score{10}, Score{5},  Score{0}, Score{0},  Score{0},  Score{0}, Score{0},  Score{0}, Score{0},  Score{0}},
            {Score{-50}, Score{-40}, Score{-30}, Score{-30}, Score{-30}, Score{-30}, Score{-40}, Score{-50}, Score{-40}, Score{-20}, Score{0},   Score{5},   Score{5},
             Score{0},   Score{-20}, Score{-40}, Score{-30}, Score{5},   Score{10},  Score{15},  Score{15},  Score{10},  Score{5},   Score{-30}, Score{-30}, Score{0},
             Score{15},  Score{20},  Score{20},  Score{15},  Score{0},   Score{-30}, Score{-30}, Score{5},   Score{15},  Score{20},  Score{20},  Score{15},  Score{5},
             Score{-30}, Score{-30}, Score{0},   Score{10},  Score{15},  Score{15},  Score{10},  Score{0},   Score{-30}, Score{-40}, Score{-20}, Score{0},   Score{0},
             Score{0},   Score{0},   Score{-20}, Score{-40}, Score{-50}, Score{-40}, Score{-30}, Score{-30}, Score{-30}, Score{-30}, Score{-40}, Score{-50}},
            {Score{-20}, Score{-10}, Score{-10}, Score{-10}, Score{-10}, Score{-10}, Score{-10}, Score{-20}, Score{-10}, Score{5},   Score{0},   Score{0},   Score{0},
             Score{0},   Score{5},   Score{-10}, Score{-10}, Score{10},  Score{10},  Score{10},  Score{10},  Score{10},  Score{10},  Score{-10}, Score{-10}, Score{0},
             Score{10},  Score{10},  Score{10},  Score{10},  Score{0},   Score{-10}, Score{-10}, Score{5},   Score{5},   Score{10},  Score{10},  Score{5},   Score{5},
             Score{-10}, Score{-10}, Score{0},   Score{5},   Score{10},  Score{10},  Score{5},   Score{0},   Score{-10}, Score{-10}, Score{0},   Score{0},   Score{0},
             Score{0},   Score{0},   Score{0},   Score{-10}, Score{-20}, Score{-10}, Score{-10}, Score{-10}, Score{-10}, Score{-10}, Score{-10}, Score{-20}},
            {Score{-20}, Score{-10}, Score{-10}, Score{-5},  Score{-5},  Score{-10}, Score{-10}, Score{-20}, Score{-10}, Score{0},   Score{5},   Score{0},  Score{0},
             Score{0},   Score{0},   Score{-10}, Score{-10}, Score{5},   Score{5},   Score{5},   Score{5},   Score{5},   Score{0},   Score{-10}, Score{0},  Score{0},
             Score{5},   Score{5},   Score{5},   Score{5},   Score{0},   Score{-5},  Score{-5},  Score{0},   Score{5},   Score{5},   Score{5},   Score{5},  Score{0},
             Score{-5},  Score{-10}, Score{0},   Score{5},   Score{5},   Score{5},   Score{5},   Score{0},   Score{-10}, Score{-10}, Score{0},   Score{0},  Score{0},
             Score{0},   Score{0},   Score{0},   Score{-10}, Score{-20}, Score{-10}, Score{-10}, Score{-5},  Score{-5},  Score{-10}, Score{-10}, Score{-20}},
            {Score{20},  Score{30},  Score{10},  Score{0},   Score{0},   Score{10},  Score{30},  Score{20},  Score{20},  Score{20},  Score{0},   Score{0},   Score{0},
             Score{0},   Score{20},  Score{20},  Score{-10}, Score{-20}, Score{-20}, Score{-20}, Score{-20}, Score{-20}, Score{-20}, Score{-10}, Score{-20}, Score{-30},
             Score{-30}, Score{-40}, Score{-40}, Score{-30}, Score{-30}, Score{-20}, Score{-30}, Score{-40}, Score{-40}, Score{-50}, Score{-50}, Score{-40}, Score{-40},
             Score{-30}, Score{-30}, Score{-40}, Score{-40}, Score{-50}, Score{-50}, Score{-40}, Score{-40}, Score{-30}, Score{-30}, Score{-40}, Score{-40}, Score{-50},
             Score{-50}, Score{-40}, Score{-40}, Score{-30}, Score{-30}, Score{-40}, Score{-40}, Score{-50}, Score{-50}, Score{-40}, Score{-40}, Score{-30}},
            {Score{-50}, Score{-30}, Score{-30}, Score{-30}, Score{-30}, Score{-30}, Score{-30}, Score{-50}, Score{-30}, Score{-30}, Score{0},   Score{0},   Score{0},
             Score{0},   Score{-30}, Score{-30}, Score{-30}, Score{-10}, Score{20},  Score{30},  Score{30},  Score{20},  Score{-10}, Score{-30}, Score{-30}, Score{-10},
             Score{30},  Score{40},  Score{40},  Score{30},  Score{-10}, Score{-30}, Score{-30}, Score{-10}, Score{30},  Score{40},  Score{40},  Score{30},  Score{-10},
             Score{-30}, Score{-30}, Score{-10}, Score{20},  Score{30},  Score{30},  Score{20},  Score{-10}, Score{-30}, Score{-30}, Score{-20}, Score{-10}, Score{0},
             Score{0},   Score{-10}, Score{-20}, Score{-30}, Score{-50}, Score{-40}, Score{-30}, Score{-20}, Score{-20}, Score{-30}, Score{-40}, Score{-50}}
        }
    };
}

} // namespace
