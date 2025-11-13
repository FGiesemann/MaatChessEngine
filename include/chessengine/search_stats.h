/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_SEARCH_STATS_H
#define CHESSENGINE_SEARCH_STATS_H

#include <cstdint>

namespace chessengine {

/**
 * \brief Statistics of the last search.
 */
struct SearchStats {
    std::uint64_t nodes{0};   ///< Number of noes evaluated during search.
    std::uint64_t cutoffs{0}; ///< Number of branches cut off during search.
};

} // namespace chessengine

#endif
