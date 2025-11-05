/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/types.h"

namespace chessengine {

const Score Score::Infinity{32700};
const Score Score::NegInfinity{-Infinity};
const Score Score::Mate{32000};

const Depth Depth::Zero{0};
const Depth Depth::Step{1};
const Depth Depth::MaxMateDepth{256};

} // namespace chessengine
