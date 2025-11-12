#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>

#include <chesscore/epd.h>
#include <chessgame/san.h>

#include "chessengine/chess_engine.h"
#include "chessengine/search.h"

struct TestResult {
    bool found_mate{false};
    chessengine::Depth expected_depth;
    chessengine::Depth found_depth;
    chesscore::MoveList expected_moves;
    chesscore::Move found_move;
    chessengine::SearchStats search_stats;
};

auto perform_test(const chesscore::EpdRecord &test, const chessengine::Config &config) -> TestResult;
auto convert_from_san(const std::string &san, const chesscore::Position &position) -> chesscore::Move;

auto main(int argc, const char *argv[]) -> int {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    // const auto input_filename = std::string{R"(D:\Programmierung\Projekte\Chess\ChessEngine\test\mate_in_x_test\data\test.epd)"};
    const auto input_filename = std::string{argv[1]};
    std::cout << "Test suite: " << input_filename << '\n';
    std::ifstream input_file{input_filename};
    if (!input_file.is_open()) {
        std::cerr << "Unable to open " << input_filename << "\n";
        return 1;
    }

    const auto test_suite = chesscore::read_epd(input_file);
    std::cout << "Loaded " << test_suite.size() << " test cases\n";

    const auto places = static_cast<int>(std::floor(std::log(test_suite.size()) / std::log(10))) + 1;

    int tests_performed{0};
    int tests_passed{0};

    chessengine::Config config{
        .minimax_config{
            .use_alpha_beta_pruning = true,
        },
        .search_config{.max_depth = chessengine::Depth{4}, .iterative_deepening = true},
        .evaluator_config{}
    };

    for (const auto &test_case : test_suite) {
        std::cout << "Test " << std::setw(places) << (tests_performed + 1) << ": ";
        try {
            const auto result = perform_test(test_case, config);
            ++tests_performed;
            if (!result.found_mate) {
                std::cout << "Did not find mate!";
            } else {
                if (result.found_depth != result.expected_depth) {
                    std::cout << "Found mate in " << result.found_depth.value << ", but expected " << result.expected_depth.value;
                } else {
                    if (!chesscore::move_list_contains(result.expected_moves, result.found_move)) {
                        std::cout << "Found move " << to_string(result.found_move) << ", but expected "
                                  << (result.expected_moves | std::views::transform([&](const auto &move) { return to_string(move); }) | std::views::join_with(std::string{", "}) |
                                      std::ranges::to<std::string>());
                    } else {
                        std::cout << "Passed!";
                        ++tests_passed;
                    }
                }
            }
            std::cout << "  [ nodes: " << result.search_stats.nodes << ", cutoffs: " << result.search_stats.cutoffs << " ]\n";
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
            continue;
        }
    }

    return 0;
}

auto perform_test(const chesscore::EpdRecord &test, const chessengine::Config &config) -> TestResult {
    TestResult test_result{};
    test_result.expected_depth = chessengine::Depth{static_cast<chessengine::Depth::value_type>(test.pv.size())};
    test_result.expected_moves = std::views::transform(test.bm, [&](const auto &move) { return convert_from_san(move, test.position); }) | std::ranges::to<chesscore::MoveList>();

    std::cout << '[' << test.id.value() << "] (" << std::setw(2) << test_result.expected_depth.value << ") ";

    chessengine::Config test_config{config};
    test_config.search_config.max_depth = chessengine::Depth{test_result.expected_depth + chessengine::Depth::Step};
    chessengine::ChessEngine engine{};
    engine.set_config(test_config);
    engine.set_position(test.position);
    const auto result = engine.search();
    test_result.search_stats = engine.search_stats();
    test_result.found_move = result.move;
    if (is_winning_score(result.score)) {
        test_result.found_mate = true;
        test_result.found_depth = ply_to_mate(result.score);
    }
    return test_result;
}

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
