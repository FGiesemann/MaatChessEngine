/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include <chesscore/epd.h>
#include <chesscore/fen.h>
#include <chessgame/san.h>
#include <chessuci/gui_handler.h>

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

class MultiSolutionFinder {
public:
    MultiSolutionFinder(const chessuci::ProcessParams &params, const std::string &input, const std::string &output);
    ~MultiSolutionFinder();

    auto process() -> void;
private:
    enum class Callback { None, IsReady, BestMove };

    chessuci::UCIGuiHandler m_uci_handler;
    std::string m_in_path;
    std::string m_out_path;
    chesscore::EpdSuite m_tests;
    std::mutex m_mutex;
    std::mutex m_info_mutex;
    std::condition_variable m_condvar;
    Callback m_received_callback{Callback::None};

    chesscore::EpdRecord *m_current_record{};

    auto read_test_suite() -> void;
    auto process_tests() -> void;
    auto process_test(chesscore::EpdRecord &record) -> void;
    auto write_results() -> void;

    auto readyok() -> void;
    auto search_info(const chessuci::search_info &info) -> void;
    auto bestmove(const chessuci::bestmove_info &info) -> void;
};

MultiSolutionFinder::MultiSolutionFinder(const chessuci::ProcessParams &params, const std::string &input, const std::string &output) {
    m_uci_handler.on_readyok(std::bind(&MultiSolutionFinder::readyok, this));
    m_uci_handler.on_info(std::bind(&MultiSolutionFinder::search_info, this, std::placeholders::_1));
    m_uci_handler.on_bestmove(std::bind(&MultiSolutionFinder::bestmove, this, std::placeholders::_1));
    m_uci_handler.start(params);
    m_uci_handler.send_uci();
    m_uci_handler.send_isready();
    m_in_path = input;
    m_out_path = output;
}

MultiSolutionFinder::~MultiSolutionFinder() {
    m_uci_handler.stop();
}

auto MultiSolutionFinder::process() -> void {
    read_test_suite();
    process_tests();
    write_results();
}

auto MultiSolutionFinder::read_test_suite() -> void {
    std::ifstream input_file{m_in_path};
    if (!input_file.is_open()) {
        throw std::runtime_error{"Could not open input file"};
    }

    m_tests = chesscore::read_epd(input_file);
}

auto MultiSolutionFinder::process_tests() -> void {
    for (auto &record : m_tests) {
        if (record.pv.size() == 1) {
            process_test(record);
        }
    }
}

auto MultiSolutionFinder::process_test(chesscore::EpdRecord &record) -> void {
    m_current_record = &record;
    m_received_callback = Callback::None;
    int expected_depth = record.pv.size();
    std::string position_fen = chesscore::FenString{record.position.piece_placement(), record.position.state()}.str();

    m_uci_handler.send_ucinewgame();
    m_uci_handler.send_setoption({.name = "MultiPV", .value = "10"});
    m_uci_handler.send_isready();
    std::unique_lock lock{m_mutex};
    m_condvar.wait(lock, [&]() { return m_received_callback == Callback::IsReady; });
    m_uci_handler.send_position({.fen = position_fen});
    m_uci_handler.send_go({.depth = expected_depth});
    m_condvar.wait(lock, [&]() { return m_received_callback == Callback::BestMove; });
    m_current_record = nullptr;
}

auto MultiSolutionFinder::write_results() -> void {
    std::ofstream out_file{m_out_path};
    if (!out_file.is_open()) {
        throw std::runtime_error{"Could no open output file"};
    }

    for (const auto &record : m_tests) {
        chesscore::write_epd_record(out_file, record);
    }
}

auto MultiSolutionFinder::readyok() -> void {
    {
        std::scoped_lock lock{m_mutex};
        m_received_callback = Callback::IsReady;
    }
    m_condvar.notify_one();
}

auto MultiSolutionFinder::search_info(const chessuci::search_info &info) -> void {
    std::scoped_lock lock{m_info_mutex};
    if (m_current_record == nullptr) {
        // shouldn't happen
        return;
    }
    if (info.score.has_value() && info.score.value().mate.has_value()) {
        int mate = info.score->mate.value();
        if (!info.pv.empty()) {
            const auto &uci_move = info.pv.front();
            const auto move = convert_legal_move(uci_move, m_current_record->position);
            if (!move.has_value()) {
                return;
            }
            const auto all_moves = m_current_record->position.all_legal_moves();
            const auto san_move = chessgame::generate_san_move(move.value(), all_moves);
            if (san_move.has_value()) {
                const auto san_str = to_string(san_move.value());
                if (std::ranges::find(m_current_record->bm, san_str) == m_current_record->bm.end()) {
                    m_current_record->bm.push_back(san_str);
                }
            }
        }
    }
}

auto MultiSolutionFinder::bestmove(const chessuci::bestmove_info &info) -> void {
    {
        std::scoped_lock lock{m_mutex};
        m_received_callback = Callback::BestMove;
    }
    m_condvar.notify_one();
}

auto main(int argc, char *argv[]) -> int {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <stockfish> <input> <output>\n";
        return 1;
    }

    chessuci::ProcessParams params{.executable = std::string{argv[1]}};
    MultiSolutionFinder finder{params, std::string{argv[2]}, std::string{argv[3]}};
    finder.process();
}
