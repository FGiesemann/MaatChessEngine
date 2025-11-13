/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_SEARCH_STATS_H
#define CHESSENGINE_SEARCH_STATS_H

#include <atomic>

namespace chessengine {

/**
 * \brief Statistics of the last search.
 */
struct SearchStats {
    std::atomic<std::uint64_t> nodes{0};   ///< Number of noes evaluated during search.
    std::atomic<std::uint64_t> cutoffs{0}; ///< Number of branches cut off during search.

    SearchStats() = default;
    SearchStats(const SearchStats &other);
    auto operator=(const SearchStats &rhs) -> SearchStats &;
    SearchStats(SearchStats &&other) = default;
    auto operator=(SearchStats &&other) -> SearchStats & = default;
    ~SearchStats() = default;
};

} // namespace chessengine

#endif
