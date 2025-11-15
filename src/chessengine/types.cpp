/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/types.h"

#include <limits>

namespace chessengine {

const Score Score::Infinity{32700};
const Score Score::NegInfinity{-Infinity};
const Score Score::Mate{32000};

const Depth Depth::Zero{0};
const Depth Depth::Step{1};
const Depth Depth::MaxMateDepth{256};
const Depth Depth::Infinite{std::numeric_limits<Depth::value_type>::max()};

} // namespace chessengine
