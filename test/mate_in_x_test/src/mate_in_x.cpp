/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "mate_in_x.h"

#include <chessengine/chess_engine.h>
#include <chessgame/san.h>

#include <cmath>
#include <future>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

auto convert_from_san(const std::string &san, const chesscore::Position &position) -> chesscore::Move {
    const auto opt_san_move = chessgame::parse_san(san, position.side_to_move());
    if (!opt_san_move.has_value()) {
        throw std::runtime_error{"Failed to parse SAN"};
    }
    const auto legal_moves = position.all_legal_moves();
    const auto matched_moves = chessgame::match_move(opt_san_move.value(), legal_moves);
    if (matched_moves.size() != 1) {
        throw std::runtime_error{"Failed to find move"};
    }
    return matched_moves.front();
}

} // namespace

namespace chessengine::mate_in_x {

auto MateInXTest::run_tests(const std::string &file_path, const std::string &first_test_id) -> void {
    reset_stats();
    load_tests(file_path);
    write_log("Loaded " + std::to_string(m_tests.size()) + " test cases\n");
    calculate_places();
    process_tests(first_test_id);
}

auto MateInXTest::process_tests(const std::string &first_test_id) -> void {
    std::vector<std::future<MateInXResult>> futures;
    bool start_processing = first_test_id.empty();
    for (const auto &test : m_tests) {
        if (!start_processing) {
            if (test.id.has_value() && test.id.value() == first_test_id) {
                start_processing = true;
            } else {
                continue;
            }
        }
        if (test.pv.size() == 1 || m_max_threads == 1) {
            log_result(perform_test(test));
            continue;
        }
        while (futures.size() >= m_max_threads) {
            if (!start_processing) {
                if (test.id.has_value() && test.id.value() == first_test_id) {
                    start_processing = true;
                } else {
                    continue;
                }
            }
            auto it = std::ranges::find_if(futures, [](const std::future<MateInXResult> &fut) { return fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready; });
            if (it != futures.end()) {
                log_result(it->get());
                futures.erase(it);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
        futures.push_back(std::async(&MateInXTest::perform_test, this, std::cref(test)));
    }
    for (auto &future : futures) {
        log_result(future.get());
    }
}

auto MateInXTest::perform_test(const chesscore::EpdRecord &test) -> MateInXResult {
    MateInXResult test_result{};
    test_result.test_id = test.id.value_or("N/A");
    test_result.expected_depth = chessengine::Depth{static_cast<chessengine::Depth::value_type>(test.pv.size())};
    test_result.expected_moves = std::views::transform(test.bm, [&](const auto &move) { return convert_from_san(move, test.position); }) | std::ranges::to<chesscore::MoveList>();

    chessengine::ChessEngine engine{};
    engine.set_config(m_base_config);
    engine.set_position(test.position);
    chessengine::StopParameters stop_params{.max_search_depth = chessengine::Depth{test_result.expected_depth + chessengine::Depth::Step}};
    const auto result = engine.search(stop_params);
    test_result.search_stats = engine.search_stats();
    test_result.found_move = result.move;
    if (is_winning_score(result.score)) {
        test_result.found_mate = true;
        test_result.found_depth = ply_to_mate(result.score);
    }
    return test_result;
}

auto MateInXTest::log_result(const MateInXResult &result) -> void {
    ++m_tests_performed;
    std::stringstream log_message;
    log_message << "Test " << std::setw(m_places) << m_tests_performed << " (" << std::fixed << std::setw(6) << std::setprecision(2)
                << (m_tests_performed / static_cast<double>(m_tests.size()) * 100.0F) << " %) [" << result.test_id << "]: ";
    if (!result.found_mate) {
        log_message << "NO MATE\n";
    } else {
        log_message << std::format("{:>13} @ {:>2} ", to_string(result.found_move), result.found_depth.value);
        if (!chesscore::move_list_contains(result.expected_moves, result.found_move)) {
            log_message << "!! Unexpected move! Expected: "
                        << (result.expected_moves | std::views::transform([&](const auto &move) { return to_string(move); }) | std::views::join_with(std::string{", "}) |
                            std::ranges::to<std::string>());
        } else if (result.found_depth != result.expected_depth) {
            log_message << "!! Unexpected depth! Expected: " << result.expected_depth.value;
        } else {
            log_message << "PASSED";
            ++m_tests_passed;
        }
    }
    log_message << " (" << result.search_stats.nodes << " nodes, " << result.search_stats.cutoffs << " cutoffs, " << result.search_stats.elapsed_time.count() << " ms)\n";
    write_log(log_message.str());
}

auto MateInXTest::load_tests(const std::string &test_file_path) -> void {
    std::ifstream test_file{test_file_path};
    if (!test_file.is_open()) {
        throw std::runtime_error{"Unable to open test file: " + test_file_path};
    }

    m_tests = chesscore::read_epd(test_file);
}

auto MateInXTest::set_log(const std::string &log_file_path) -> void {
    m_log_file.open(log_file_path, std::ios::out);
    if (!m_log_file.is_open()) {
        throw std::runtime_error{"Unable to open log file: " + log_file_path};
    }
}

auto MateInXTest::write_log(const std::string &message) -> void {
    std::scoped_lock lock{m_log_mutex};
    std::cout << message;
    if (m_log_file.is_open()) {
        m_log_file << message;
    }
}

auto MateInXTest::reset_stats() -> void {
    m_tests_performed = 0;
    m_tests_passed = 0;
}

auto MateInXTest::calculate_places() -> void {
    m_places = static_cast<int>(std::floor(std::log(m_tests.size()) / std::log(10))) + 1;
}

} // namespace chessengine::mate_in_x
