/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "chessengine/chess_engine.h"
#include "chessengine/logger.h"

namespace chessengine {

const char ChessEngine::identifier[] = "Maat v0.1";
const char ChessEngine::author[] = "Florian Giesemann";

ChessEngine::ChessEngine(const Config &config) : m_config{config}, m_evaluator{config.evaluator_config} {}

ChessEngine::~ChessEngine() {
    if (m_search_thread.joinable()) {
        m_search_thread.join();
    }
}

auto ChessEngine::search(const StopParameters &stop_params) -> EvaluatedMove {
    log_search_stream() << "Searching position:";
    if (Logger::instance().is_enabled()) {
        const auto fen = chesscore::FenString{m_position.piece_placement(), m_position.state()}.str();
        log_search_stream() << "  fen = " << fen;
        log_search_stream() << "  stopping criteria: " << to_string(stop_params);
    }
    m_search_start = std::chrono::steady_clock::now();
    m_stopping_params = stop_params;
    // If iterative_deepening is not used, the max_search_depth should be set!
    auto search_depth = m_config.search_config.iterative_deepening ? Depth{1} : stop_params.max_search_depth;
    m_best_move = {};
    try {
        while (true) {
            check_stop();
            log_search_stream() << "Searching for depth: " << search_depth;
            log_indent();
            m_best_move = search_position(search_depth);
            log_unindent();
            log_search_stream() << "Search for depth " << search_depth << " finishd with best move: " << to_string(m_best_move.move) << " (" << m_best_move.score << ')';
            m_search_stats.depth = search_depth;
            if (m_search_progress_callback) {
                m_search_stats.best_move = m_best_move;
                m_search_stats.elapsed_time = search_time();
                m_search_progress_callback(m_search_stats);
            }
            if (is_winning_score(m_best_move.score)) {
                log_search_stream() << "Stopping search at winning score " << m_best_move.score;
                break;
            }
            search_depth += Depth::Step;
        }
    } catch (const SearchAborted &e) {
        log_search_stream() << "Search stopped: " << e.what();
    }

    m_search_running = false;
    m_search_stats.elapsed_time = search_time();
    log_search_stream() << "Search took " << m_search_stats.elapsed_time.count() << " ms";
    if (m_search_ended_callback) {
        m_search_ended_callback(m_best_move);
    }
    return m_best_move;
}

auto ChessEngine::search_position(Depth depth) -> EvaluatedMove {
    EvaluatedMove best_move{.move = {}, .score = Score::NegInfinity};
    Bounds bounds{};
    const auto moves = moves_to_search(m_config.search_config.search_pv_first && depth > Depth::Step);
    log_search_stream() << "Searching " << moves.size() << " moves for " << to_string(m_position.side_to_move()) << ": " << to_string(moves);
    for (const auto &move : moves) {
        {
            log_search_stream() << "Checking move " << to_string(move) << " for " << to_string(m_position.side_to_move()) << " at depth " << depth;
            log_indent();
            MoveScope scope{m_position, move};
            auto value = -search_position(depth - Depth::Step, bounds.swap());
            log_unindent();
            log_search_stream() << "Move " << to_string(move) << " for " << to_string(m_position.side_to_move()) << " evaluated to " << value;
            if (is_winning_score(value)) {
                value = value - Depth::Step;
            } else if (is_losing_score(value)) {
                value = value + Depth::Step;
            }
            if (value > best_move.score) {
                log_search_stream() << "Found new best move for " << to_string(m_position.side_to_move()) << ": " << to_string(move) << " (" << value << ") replacing "
                                    << to_string(best_move.move) << " (" << best_move.score << ")";
                best_move = {.move = move, .score = value};
            }
        }
        bounds.alpha = std::max(bounds.alpha, best_move.score);
        if (m_config.minimax_config.use_alpha_beta_pruning && (bounds.beta <= bounds.alpha)) {
            log_search("Cancelling search");
            m_search_stats.cutoffs += 1;
            break;
        }
        check_stop();
    }
    m_search_stats.nodes += 1;

    return best_move;
}

auto ChessEngine::search_position(Depth depth, Bounds bounds) -> Score {
    if ((depth == Depth::Zero)) {
        const auto eval = m_evaluator.evaluate(m_position, m_position.side_to_move());
        log_search_stream() << "Search stopped by depth. Position evaluation: " << eval;
        return eval;
    }

    const auto moves = moves_to_search();
    if (moves.empty()) {
        const auto eval = m_evaluator.evaluate(m_position, m_position.side_to_move());
        log_search_stream() << "No moves to search. Position evaluation: " << eval;
        m_search_stats.nodes += 1;
        return eval;
    }

    log_search_stream() << "Searching " << moves.size() << " moves for " << to_string(m_position.side_to_move()) << ": " << to_string(moves);
    log_search_stream() << "Alpha = " << bounds.alpha << " Beta = " << bounds.beta;

    auto best_value = Score::NegInfinity;
    for (const auto &move : moves) {
        check_stop();
        log_search_stream() << "Checking move " << to_string(move) << " for " << to_string(m_position.side_to_move()) << " at depth " << depth;
        {
            log_indent();
            MoveScope scope{m_position, move};
            auto value = -search_position(depth - Depth::Step, bounds.swap());
            log_unindent();
            log_search_stream() << "Move " << to_string(move) << " for " << to_string(m_position.side_to_move()) << " evaluated to " << value;
            if (is_winning_score(value)) {
                value = value - Depth::Step;
            } else if (is_losing_score(value)) {
                value = value + Depth::Step;
            }
            best_value = std::max(best_value, value);
            if (bounds.alpha < best_value) {
                log_search_stream() << "Updated alpha from " << bounds.alpha << " to " << best_value << "; beta = " << bounds.beta;
            }
        }
        bounds.alpha = std::max(bounds.alpha, best_value);
        if (m_config.minimax_config.use_alpha_beta_pruning && (bounds.beta <= bounds.alpha)) {
            log_search("Cancelling search");
            m_search_stats.cutoffs += 1;
            break;
        }
    }
    m_search_stats.nodes += 1;
    return best_value;
}

auto ChessEngine::moves_to_search(bool search_principal_variation_first) const -> chesscore::MoveList {
    auto moves = m_position.all_legal_moves();
    if (m_config.minimax_config.use_move_ordering) {
        sort_moves(moves);
        if (search_principal_variation_first) {
            auto it = std::find(moves.begin(), moves.end(), m_best_move.move);
            if (it != moves.end()) {
                log_search("Move ordering: moving best move of previous iteration to front");
                std::rotate(moves.begin(), it, it + 1);
            }
        }
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

auto ChessEngine::start_search(const StopParameters &stop_params) -> void {
    if (m_search_running.exchange(true)) {
        return;
    }
    // Cleanup previos search
    if (m_search_thread.joinable()) {
        m_search_thread.join();
    }
    m_stop_requested = false;
    m_search_thread = std::thread(&ChessEngine::search, this, stop_params);
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

auto ChessEngine::search_time() const -> std::chrono::milliseconds {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_search_start);
}

auto ChessEngine::check_stop() const -> void {
    static int check_counter{0};

    if (m_stop_requested) {
        log_search("STOPPING. Stop requested");
        throw SearchAborted("user request");
    }
    if (m_stopping_params.max_search_depth > Depth::Zero && m_search_stats.depth > m_stopping_params.max_search_depth) {
        log_search("STOPPING. Max search depth reached");
        throw SearchAborted("max search depth reached");
    }
    if (m_stopping_params.max_search_nodes > 0 && m_search_stats.nodes > m_stopping_params.max_search_nodes) {
        log_search("STOPPING. Max search nodes reached");
        throw SearchAborted("max search nodes reached");
    }
    if ((m_stopping_params.max_search_time.count() > 0) && (++check_counter > stop_check_interval)) {
        check_counter = 0;
        const auto search_duration = search_time();
        const auto time_exceeded = search_duration > m_stopping_params.max_search_time;
        if (time_exceeded) {
            log_search("STOPPING. Max search time exceeded");
            throw SearchAborted("max search time exceeded");
        }
    }
}

} // namespace chessengine
