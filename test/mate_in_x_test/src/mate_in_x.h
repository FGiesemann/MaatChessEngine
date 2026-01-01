/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_MATE_IN_X_H
#define CHESSENGINE_MATE_IN_X_H

#include <chesscore/epd.h>
#include <chessengine/config.h>
#include <chessengine/logger.h>
#include <chessengine/types.h>

#include <fstream>
#include <mutex>
#include <string>

namespace chessengine::mate_in_x {

struct MateInXResult {
    bool found_mate{false};
    chessengine::Depth expected_depth;
    chessengine::Depth found_depth;
    chesscore::MoveList expected_moves;
    chesscore::Move found_move;
    chessengine::SearchStats search_stats;
    std::string test_id;
};

class MateInXTest {
public:
    auto set_log(const std::string &log_file_path) -> void;
    auto set_threads(int thread_count) -> void { m_max_threads = thread_count; }
    auto set_config(const chessengine::Config &config) -> void { m_base_config = config; }
    auto enable_debug() -> void { Logger::instance().enable("engine_debug.log"); }
    auto run_tests(const std::string &file_path, const std::string &first_test_id = "") -> void;

    auto test_count() const -> std::size_t { return m_tests.size(); }
    auto max_threads() const -> int { return m_max_threads; }
private:
    auto reset_stats() -> void;
    auto load_tests(const std::string &test_file_path) -> void;
    auto write_log(const std::string &message) -> void;
    auto calculate_places() -> void;
    auto process_tests(const std::string &first_test_id) -> void;
    auto perform_test(const chesscore::EpdRecord &test) -> MateInXResult;
    auto log_result(const MateInXResult &result) -> void;
    auto print_summary() -> void;

    chesscore::EpdSuite m_tests;
    std::ofstream m_log_file;
    int m_places;
    int m_tests_performed{0};
    int m_tests_passed{0};
    int m_max_threads{1};
    chessengine::Config m_base_config;
    std::mutex m_log_mutex;
};

} // namespace chessengine::mate_in_x

#endif
