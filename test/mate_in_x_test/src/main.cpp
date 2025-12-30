/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "mate_in_x.h"

#include <iostream>
#include <string>

struct Parameters {
    std::string input_file;
    std::string log_file;
    int thread_count{1};
    std::string first_test_id{""};
    bool debug{false};
};

auto read_arguments(int argc, const char *argv[]) -> Parameters {
    Parameters params;
    for (int i = 1; i < argc; ++i) {
        std::string arg{argv[i]};
        if (arg.starts_with("--log=")) {
            params.log_file = arg.substr(6);
        } else if (arg.starts_with("--threads=")) {
            params.thread_count = std::stoi(arg.substr(10));
        } else if (arg.starts_with("--first-test=")) {
            params.first_test_id = arg.substr(13);
        } else if (arg.starts_with("--debug")) {
            params.debug = true;
        } else {
            params.input_file = arg;
        }
    }
    return params;
}

auto main(int argc, const char *argv[]) -> int {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "[--threads=<number>] [--log=<file>] [--first-test=<ID>] <input_file>\n";
        return 1;
    }

    Parameters params = read_arguments(argc, argv);

    chessengine::Config config{
        .minimax_config =
            {
                .use_alpha_beta_pruning = true,
                .use_move_ordering = true,
            },
        .search_config =
            {
                .iterative_deepening = true,
                .search_pv_first = true,
            },
        .evaluator_config = {},
    };
    chessengine::mate_in_x::MateInXTest test_runner;
    test_runner.set_config(config);
    if (!params.log_file.empty()) {
        test_runner.set_log(params.log_file);
    }
    if (params.thread_count > 1) {
        test_runner.set_threads(params.thread_count);
    }
    if (params.debug) {
        test_runner.enable_debug();
    }

    test_runner.run_tests(params.input_file, params.first_test_id);

    return 0;
}
