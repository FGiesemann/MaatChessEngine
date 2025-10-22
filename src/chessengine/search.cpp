/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/search.h"
#include "chessengine/minimax.h"

namespace chessengine {

Search::Search(const Config &config) : m_config{config} {}

auto Search::best_move(const chesscore::Position &position) const -> EvaluatedMove {
    Evaluator evaluator{m_config.evaluator_config};
    EvaluatedMove current_best_move{};

    Depth current_depth = m_config.search_config.iterative_deepening ? Depth{1} : m_config.search_config.max_depth;
    MinimaxSearch minimax{m_config.minimax_config, evaluator};
    while (current_depth <= m_config.search_config.max_depth) {
        auto best_move = minimax.best_move(position, current_depth);
        m_search_stats = minimax.search_stats();
        if (is_winning_score(best_move.score)) {
            current_best_move = best_move;
            break;
        }
        if (best_move.score > current_best_move.score) {
            current_best_move = best_move;
        }
        if (!m_config.search_config.iterative_deepening) {
            break;
        }
        current_depth += Depth::Step;
    }

    return current_best_move;
}

} // namespace chessengine
