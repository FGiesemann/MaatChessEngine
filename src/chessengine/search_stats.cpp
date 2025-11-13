/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/search_stats.h"

namespace chessengine {

SearchStats::SearchStats(const SearchStats &other) : nodes{other.nodes.load()}, cutoffs{other.cutoffs.load()} {}

auto SearchStats::operator=(const SearchStats &rhs) -> SearchStats & {
    nodes = rhs.nodes.load();
    cutoffs = rhs.cutoffs.load();
    return *this;
}

} // namespace chessengine
