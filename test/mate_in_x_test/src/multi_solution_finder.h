/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef MATE_IN_X_TEST_MULTI_SOLUTION_FINDER_H
#define MATE_IN_X_TEST_MULTI_SOLUTION_FINDER_H

#include <chesscore/epd.h>
#include <chessuci/gui_handler.h>

#include <condition_variable>
#include <mutex>
#include <string>

namespace chessengine::mate_in_x {

class MultiSolutionFinder {
public:
    MultiSolutionFinder(const chessuci::ProcessParams &params);
    ~MultiSolutionFinder();
    auto process(chesscore::EpdRecord &record) -> void;
private:
    enum class Callback { None, IsReady, BestMove };

    chessuci::UCIGuiHandler m_uci_handler;
    chesscore::EpdSuite m_tests;
    std::mutex m_mutex;
    std::mutex m_info_mutex;
    std::condition_variable m_condvar;
    Callback m_received_callback{Callback::None};

    chesscore::EpdRecord *m_current_record{};

    auto readyok() -> void;
    auto search_info(const chessuci::search_info &info) -> void;
    auto bestmove(const chessuci::bestmove_info &info) -> void;
};

} // namespace chessengine::mate_in_x

#endif
