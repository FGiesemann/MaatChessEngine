/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/chess_engine.h"

#include "chessengine/evaluation.h"
#include "chessengine/minimax.h"

namespace chessengine {

const std::string ChessEngine::identifier = "Maat v0.1";
const std::string ChessEngine::author = "Florian Giesemann";

auto ChessEngine::search() const -> EvaluatedMove {
    Evaluator evaluator{m_config.evaluator_config};
    MoveOrdering move_ordering{m_config.evaluator_config};
    MinimaxSearch minimax{m_config.minimax_config, evaluator, move_ordering};

    Depth current_depth = m_config.search_config.iterative_deepening ? Depth{1} : m_config.search_config.max_depth;
    EvaluatedMove current_best_move{};
    while (current_depth <= m_config.search_config.max_depth) {
        // TODO: feed in the best move from the previous iteration to search first...
        auto best_move = minimax.best_move(m_position, current_depth);
        {
            // TODO: Maybe not a good idea to have a mutex lock inside the search loop...
            std::lock_guard<std::mutex> lock{m_stats_mutex};
            m_search_stats = minimax.search_stats();
        }
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

auto ChessEngine::search_stats() const -> SearchStats {
    std::lock_guard<std::mutex> lock{m_stats_mutex};
    return m_search_stats;
}

auto ChessEngine::new_game() -> void {
    m_position = chesscore::Position{chesscore::FenString::starting_position()};
}

auto ChessEngine::start_search() -> void {
    // TODO
}

auto ChessEngine::stop_search() -> void {
    // stop search thread
}

auto ChessEngine::best_move() const -> chesscore::Move {
    // TODO
    return {};
}

auto ChessEngine::set_position(const chesscore::Position &position) -> void {
    m_position = position;
}

auto ChessEngine::play_move(const chesscore::Move &move) -> void {
    m_position.make_move(move);
}

auto ChessEngine::set_debugging(bool debug_on) -> void {
    m_debugging = debug_on;
}

auto ChessEngine::load_config(const std::filesystem::path &filename) -> void {
    m_config = Config::from_file(filename);
}

} // namespace chessengine
