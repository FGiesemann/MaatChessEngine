/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_MAAT_ENGINE_H
#define CHESS_ENGINE_MAAT_ENGINE_H

#include <atomic>
#include <filesystem>
#include <thread>

#include <chesscore/position.h>

#include "chessengine/config.h"

namespace chessengine::maat {

class Engine {
public:
    static const std::string identifier; ///< Name an version of the engine.
    static const std::string author;     ///< Author of the engine.

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
    auto best_move() const -> chesscore::Move;

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
    Config m_config{};                         ///< The engine configuration (search, evaluation, ...)
    chesscore::Position m_position;            ///< The current position.
    bool m_debugging{false};                   ///< Debugging mode.
    std::thread m_search_thread{};             ///< Thread for the search.
    std::atomic<bool> m_search_running{false}; ///< If a search is running.
};

} // namespace chessengine::maat

#endif
