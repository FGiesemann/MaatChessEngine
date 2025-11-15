/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/chess_engine.h"

namespace chessengine {

const char ChessEngine::identifier[] = "Maat v0.1";
const char ChessEngine::author[] = "Florian Giesemann";

ChessEngine::ChessEngine(const Config &config) : m_config{config}, m_evaluator{config.evaluator_config} {}

auto ChessEngine::search(Depth max_depth) -> EvaluatedMove {
    if (m_search_running.exchange(true)) {
        // a search is already running
        return {};
    }
    m_search_stats.depth = m_config.search_config.iterative_deepening ? Depth{1} : max_depth;
    m_best_move = {};
    while (!should_stop() && m_search_stats.depth <= max_depth) {
        auto best_move = search_position(m_search_stats.depth);
        if (is_winning_score(best_move.score)) {
            m_best_move = best_move;
            break;
        }
        if (best_move.score > m_best_move.score) {
            m_best_move = best_move;
        }
        m_search_stats.depth += Depth::Step;
        if (m_search_progress_callback) {
            m_search_stats.best_move = best_move;
            m_search_progress_callback(m_search_stats);
        }
    }

    m_search_running = false;
    if (m_search_ended_callback) {
        m_search_ended_callback(m_best_move);
    }
    return m_best_move;
}

auto ChessEngine::search_position(Depth depth) -> EvaluatedMove {
    m_search_stats = SearchStats{};
    m_color_to_evaluate = m_position.side_to_move();

    chesscore::Move best_move{};
    auto best_value = Score::NegInfinity;
    m_search_stats.nodes += 1;
    const auto moves = moves_to_search();
    for (const auto &move : moves) {
        m_position.make_move(move);
        auto value = search_position(depth - Depth::Step, Bounds{}, false);
        if (is_winning_score(value)) {
            value = value - Depth::Step;
        } else if (is_losing_score(value)) {
            value = value + Depth::Step;
        }
        m_position.unmake_move(move);
        if (value > best_value) {
            best_move = move;
            best_value = value;
        }
    }

    return {.move = best_move, .score = best_value};
}

auto ChessEngine::search_position(Depth depth, Bounds bounds, bool maximizing_player) -> Score {
    m_search_stats.nodes += 1;
    if ((depth == Depth::Zero) || ((m_search_stats.nodes % stop_check_interval == 0) && should_stop())) {
        return m_evaluator.evaluate(m_position, m_color_to_evaluate);
    }
    const auto moves = moves_to_search();
    if (moves.empty()) {
        return m_evaluator.evaluate(m_position, m_color_to_evaluate);
    }

    if (maximizing_player) {
        auto best_value = Score::NegInfinity;
        for (const auto &move : moves) {
            m_position.make_move(move);
            auto value = search_position(depth - Depth::Step, bounds, false);
            if (is_winning_score(value)) {
                value = value - Depth::Step;
            } else if (is_losing_score(value)) {
                value = value + Depth::Step;
            }
            best_value = std::max(best_value, value);
            m_position.unmake_move(move);
            bounds.alpha = std::max(bounds.alpha, best_value);
            if (m_config.minimax_config.use_alpha_beta_pruning && (bounds.beta <= bounds.alpha)) {
                m_search_stats.cutoffs += 1;
                break;
            }
        }
        return best_value;
    } else {
        auto best_value = Score::Infinity;
        for (const auto &move : moves) {
            m_position.make_move(move);
            auto value = search_position(depth - Depth::Step, bounds, true);
            if (is_winning_score(value)) {
                value = value - Depth::Step;
            } else if (is_losing_score(value)) {
                value = value + Depth::Step;
            }
            best_value = std::min(best_value, value);
            m_position.unmake_move(move);
            bounds.beta = std::min(bounds.beta, best_value);
            if (m_config.minimax_config.use_alpha_beta_pruning && (bounds.beta <= bounds.alpha)) {
                m_search_stats.cutoffs += 1;
                break;
            }
        }
        return best_value;
    }
}

auto ChessEngine::moves_to_search() const -> chesscore::MoveList {
    auto moves = m_position.all_legal_moves();
    // TODO: feed in the best move from the previous iteration to search first...
    if (m_config.minimax_config.use_move_ordering) {
        sort_moves(moves);
    }
    return moves;
}

auto ChessEngine::sort_moves(chesscore::MoveList &moves) const -> void {
    std::ranges::sort(moves, [this](const chesscore::Move &lhs, const chesscore::Move &rhs) -> bool { return m_evaluator.evaluate(lhs) > m_evaluator.evaluate(rhs); });
}

auto ChessEngine::search_stats() const -> const SearchStats & {
    return m_search_stats;
}

auto ChessEngine::new_game() -> void {
    m_position = chesscore::Position{chesscore::FenString::starting_position()};
}

auto ChessEngine::start_search() -> void {
    // TODO
    m_stop_requested = false;
}

auto ChessEngine::stop_search() -> void {
    m_stop_requested = true;
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

auto ChessEngine::should_stop() const -> bool {
    return m_stop_requested;
}

} // namespace chessengine
