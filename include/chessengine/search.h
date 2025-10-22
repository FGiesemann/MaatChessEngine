/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include <chesscore/position.h>

#include "chessengine/config.h"

namespace chessengine {

/**
 * \brief Statistics of the last search.
 */
struct SearchStats {
    std::uint64_t nodes{0};   ///< Number of noes evaluated during search.
    std::uint64_t cutoffs{0}; ///< Number of branches cut off during search.
};

class Search {
public:
    Search(const Config &config);
    auto best_move(const chesscore::Position &position) const -> EvaluatedMove;

    auto search_stats() const -> const SearchStats & { return m_search_stats; }
private:
    Config m_config;
    mutable SearchStats m_search_stats;
};

} // namespace chessengine

#endif
