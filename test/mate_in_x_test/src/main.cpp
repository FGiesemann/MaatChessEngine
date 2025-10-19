#include <fstream>
#include <iomanip>
#include <iostream>

#include <chesscore/epd.h>
#include <chessgame/san.h>

#include "chessengine/search.h"

struct TestResult {
    bool found_mate{false};
    chessengine::Depth expected_depth;
    chessengine::Depth found_depth;
    chesscore::Move expected_move;
    chesscore::Move found_move;
};

auto perform_test(const chesscore::EpdRecord &test) -> TestResult;
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
    for (const auto &test_case : test_suite) {
        std::cout << "Test " << std::setw(places) << (tests_performed + 1) << ": ";
        try {
            const auto result = perform_test(test_case);
            ++tests_performed;
            if (!result.found_mate) {
                std::cout << "Did not find mate!\n";
            } else {
                if (result.found_depth != result.expected_depth) {
                    std::cout << "Found mate in " << result.found_depth.value << ", but expected " << result.expected_depth.value << '\n';
                } else {
                    if (result.found_move != result.expected_move) {
                        std::cout << "Found move " << to_string(result.found_move) << ", but expected " << to_string(result.expected_move) << '\n';
                    } else {
                        std::cout << "Passed!\n";
                        ++tests_passed;
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cerr << e.what() << '\n';
            continue;
        }
    }

    return 0;
}

auto perform_test(const chesscore::EpdRecord &test) -> TestResult {
    TestResult test_result{};
    test_result.expected_depth = chessengine::Depth{static_cast<chessengine::Depth::value_type>(test.pv.size())};
    test_result.expected_move = convert_from_san(test.bm.front(), test.position);

    std::cout << '[' << test.id.value() << "] (" << std::setw(2) << test_result.expected_depth.value << ") ";

    const auto result = chessengine::find_best_move(test.position);
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
