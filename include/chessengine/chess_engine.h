/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_CHESS_ENGINE_H
#define CHESS_ENGINE_CHESS_ENGINE_H

#include <atomic>
#include <filesystem>
#include <mutex>
#include <thread>

#include <chesscore/position.h>

#include "chessengine/config.h"
#include "chessengine/evaluation.h"

namespace chessengine {

/**
 * \brief Parameters for stopping criteria of the search.
 *
 * Describes parameters that influence the stopping criteria evaluated during
 * the search for a best move in a position.
 */
struct StopParameters {
    std::chrono::milliseconds max_search_time{0}; ///< Maximum allowed search time.
    Depth max_search_depth = Depth::Zero;         ///< Maximum allowed search depth.
    std::uint64_t max_search_nodes{0};            ///< Maximum number of nodes to evaluate.
};

class ChessEngine {
public:
    static const char identifier[]; ///< Name an version of the engine.
    static const char author[];     ///< Author of the engine.

    ChessEngine() = default;
    explicit ChessEngine(const Config &config);

    /**
     * \brief Search the stored position for the best move.
     *
     * Searches the currently set position for the best move, using the
     * parameters from the stored configuration. This is a blocking call! If you
     * want to start a search in the background, use start_search().
     * The arguments define the stopping criteria. Set any of those values to 0
     * to disable it. If all are set to 0, the search can only be cancelled by
     * setting the stop flag (see stop_search()).
     * \param stop_params The parameters for the stopping criteria.
     * \return The move found by the search.
     */
    auto search(const StopParameters &stop_params) -> EvaluatedMove;

    auto on_search_ended(SearchEndedCallback callback) -> void { m_search_ended_callback = std::move(callback); }
    auto on_search_progress(SearchProgressCalback callback) -> void { m_search_progress_callback = std::move(callback); }

    /**
     * \brief Retrieve statistics from the last searrch.
     *
     * This function should only be called after a search. It is not
     * thread-safe!
     * \return Statistics of the last search.
     */
    auto search_stats() const -> const SearchStats &;

    /**
     * \brief Reset internal state in preparation for a new game.
     */
    auto new_game() -> void;

    /**
     * \brief Set the position of the game.
     *
     * This does not reset the internal state. If a position of a different game
     * should be searched, call new_game() before setting the new position.
     * \param position The position.
     */
    auto set_position(const chesscore::Position &position) -> void;

    /**
     * \brief The current position of the game.
     *
     * \return The current position.
     */
    auto position() const -> const chesscore::Position & { return m_position; }

    /**
     * \brief Play a move in the game.
     *
     * Update the internal position with the given move.
     * \param move The move.
     */
    auto play_move(const chesscore::Move &move) -> void;

    /**
     * \brief Swith debugging on or off.
     *
     * \param debug_on State of debugging.
     */
    auto set_debugging(bool debug_on) -> void;

    /**
     * \brief Begin a search on the current position.
     *
     * The search starts on the position previously set by set_position or
     * reached by the last play_move call.
     */
    auto start_search(/* some options */) -> void;

    /**
     * \brief Stops a running search.
     *
     * Has no effekt, when there's no search running.
     */
    auto stop_search() -> void;

    /**
     * \brief If the engine is currently searching.
     *
     * Returns if the engine is currently performing a search.
     * \return If a search is ongoing.
     */
    auto is_searching() const -> bool { return m_search_running.load(); }

    /**
     * \brief Returns the best move found.
     *
     * The best move found so far. Should be called after a search has finished
     * or stopped.
     * Should not be called during a search, as it is not thread safe.
     * \return Best move found.
     */
    auto best_move() const -> EvaluatedMove { return m_best_move; }

    /**
     * \brief Set the configuration.
     *
     * Allows to set the configuration of the engine. This includes search and
     * evaluation parameters. Setting a configuration takes effect immediately.
     * This may disturb a running search and lead to unreliable results!
     * \param config The config.
     */
    auto set_config(const Config &config) -> void { m_config = config; }

    /**
     * \brief Load a configuration from a file.
     *
     * Instructs the engine to load the configuration from the given file.
     * This may disturb a running search and lead to unreliable results!
     * \param filename The config file.
     */
    auto load_config(const std::filesystem::path &filename) -> void;
private:
    Config m_config{};                                    ///< The engine configuration (search, evaluation, ...)
    Evaluator m_evaluator{m_config.evaluator_config};     ///< Evaluation of positions.
    chesscore::Position m_position;                       ///< The current position.
    bool m_debugging{false};                              ///< Debugging mode.
    std::atomic<bool> m_search_running{false};            ///< If a search is running.
    std::atomic<bool> m_stop_requested{false};            ///< If the search should be stopped.
    std::thread m_search_thread{};                        ///< Thread for the search.
    SearchStats m_search_stats{};                         ///< Statistics of the last search.
    std::mutex m_stats_mutex;                             ///< Mutex protecting access to the search statistics.
    chesscore::Color m_color_to_evaluate{};               ///< The player from who's perspective to evaluate the position.
    EvaluatedMove m_best_move{};                          ///< The best move found so far.
    SearchEndedCallback m_search_ended_callback{};        ///< Callback for search end.
    SearchProgressCalback m_search_progress_callback{};   ///< Callback for search progress.
    StopParameters m_stopping_params{};                   ///< Parameters for the stopping criteria.
    std::chrono::steady_clock::time_point m_search_start; ///< Start of the search.

    static constexpr std::uint64_t stop_check_interval{1000};

    /**
     * \brief Checks, if a running search should be stopped.
     *
     * There might be different reasons why a search should stop as soon as
     * possible. These include a request by the user or timing constraints.
     * \return If the search should be terminated as soon as possible.
     */
    auto should_stop() const -> bool;

    auto search_position(Depth depth) -> EvaluatedMove;
    auto search_position(Depth depth, Bounds bounds, bool maximizing_player) -> Score;
    auto moves_to_search() const -> chesscore::MoveList;

    auto sort_moves(chesscore::MoveList &moves) const -> void;
};

} // namespace chessengine

#endif
