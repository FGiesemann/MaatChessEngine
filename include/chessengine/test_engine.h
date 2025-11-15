/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESS_ENGINE_TEST_ENGINE_H
#define CHESS_ENGINE_TEST_ENGINE_H

#include <exception>
#include <queue>
#include <variant>
#include <vector>

#include "chessengine/types.h"

#include <chesscore/position.h>

namespace chessengine {

/**
 * \brief A test double for the ChessEngine.
 */
class TestEngine {
public:
    struct new_game_call {};
    struct set_debugging_call {
        bool debugging;
    };
    struct set_position_call {
        chesscore::Position position;
    };
    struct position_call {};
    struct play_move_call {
        chesscore::Move move;
    };
    struct start_search_call {};
    struct stop_search_call {};
    struct best_move_call {};
    using CallRecord = std::variant<new_game_call, set_debugging_call, set_position_call, position_call, play_move_call, start_search_call, stop_search_call, best_move_call>;
    using CallLog = std::vector<CallRecord>;

    auto new_game() -> void { m_call_log.emplace_back(new_game_call{}); }
    auto set_debugging(bool debugging) -> void { m_call_log.emplace_back(set_debugging_call{debugging}); }
    auto set_position(const chesscore::Position &position) -> void { m_call_log.emplace_back(set_position_call{position}); }
    auto position() const -> const chesscore::Position & {
        m_call_log.emplace_back(position_call{});
        if (m_position_return_values.empty()) {
            throw std::runtime_error("position() called more times than expected");
        }
        m_position = m_position_return_values.front();
        m_position_return_values.pop();
        return m_position;
    }
    auto play_move(const chesscore::Move &move) -> void { m_call_log.emplace_back(play_move_call{move}); }
    auto start_search(/* some options */) -> void { m_call_log.emplace_back(start_search_call{}); }
    auto stop_search() -> void { m_call_log.emplace_back(stop_search_call{}); }
    auto best_move() const -> chesscore::Move {
        m_call_log.emplace_back(best_move_call{});
        return {};
    }

    auto call_log() const -> const CallLog & { return m_call_log; }
    auto set_position_returns(const std::vector<chesscore::Position> &positions) -> void { set_queue(m_position_return_values, positions); }
    auto on_search_ended(SearchEndedCallback) -> void {}
    auto on_search_progress(SearchProgressCalback) -> void {}
private:
    mutable CallLog m_call_log;
    mutable std::queue<chesscore::Position> m_position_return_values{};
    mutable chesscore::Position m_position;

    template<typename T>
    auto set_queue(std::queue<T> &queue, const std::vector<T> &values) -> void {
        clear_queue(queue);
        fill_queue(queue, values);
    }
    template<typename T>
    auto clear_queue(std::queue<T> &queue) -> void {
        while (!queue.empty()) {
            queue.pop();
        }
    }
    template<typename T>
    auto fill_queue(std::queue<T> &queue, const std::vector<T> &source) -> void {
        for (const auto &element : source) {
            queue.push(element);
        }
    }
};

} // namespace chessengine

#endif
