/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/types.h"

namespace chessengine {

const Score Score::Infinity{32700};
const Score Score::NegInfinity{-Infinity};
const Score Score::Mate{32000};

const Depth Depth::MaxMateDepth{256};

} // namespace chessengine
