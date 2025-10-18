/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessengine/types.h"
using namespace chessengine;

TEST_CASE("Score.Mate in X", "[score]") {
    CHECK(ply_to_mate(Score::Mate - Depth{3}) == Depth{3});
    CHECK(ply_to_mate(-(Score::Mate - Depth{5})) == Depth{5});
}
