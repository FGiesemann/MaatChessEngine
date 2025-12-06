/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessengine/types.h"
using namespace chessengine;

TEST_CASE("Depth.Constants", "[depth]") {
    CHECK(Depth::Zero.value == 0);
    CHECK(Depth::Step.value == 1);
}

TEST_CASE("Depth.Operators.Basic", "[depth]") {
    CHECK(Depth{5} + Depth{3} == Depth{8});
    CHECK(Depth{5} - Depth{3} == Depth{2});
}

TEST_CASE("Depth.Operators.Assignment operations", "[depth]") {
    Depth d1{7};
    d1 += Depth{3};
    CHECK(d1 == Depth{10});

    d1 -= Depth{4};
    CHECK(d1 == Depth{6});

    d1 += Depth::Step;
    CHECK(d1 == Depth{7});
}
