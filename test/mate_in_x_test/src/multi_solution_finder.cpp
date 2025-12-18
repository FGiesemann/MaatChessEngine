/* ************************************************************************** *
 * Chess Engine Maat                                                          *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#include "multi_solution_finder.h"

#include <chessgame/san.h>

#include <fstream>
#include <iostream>

namespace chessengine::mate_in_x {

MultiSolutionFinder::MultiSolutionFinder(const chessuci::ProcessParams &params) {
    m_uci_handler.on_readyok(std::bind(&MultiSolutionFinder::readyok, this));
    m_uci_handler.on_info(std::bind(&MultiSolutionFinder::search_info, this, std::placeholders::_1));
    m_uci_handler.on_bestmove(std::bind(&MultiSolutionFinder::bestmove, this, std::placeholders::_1));
    m_uci_handler.start(params);
    m_uci_handler.send_uci();
    m_uci_handler.send_isready();
}

MultiSolutionFinder::~MultiSolutionFinder() {
    m_uci_handler.stop();
}

auto MultiSolutionFinder::process(chesscore::EpdRecord &record) -> void {
    m_current_record = &record;
    int expected_depth = record.pv.size();
    std::string position_fen = chesscore::FenString{record.position.piece_placement(), record.position.state()}.str();
    int max_variants = 5;

    m_received_callback = Callback::None;
    while (true) {
        m_uci_handler.send_ucinewgame();
        m_uci_handler.send_setoption({.name = "MultiPV", .value = std::to_string(max_variants)});
        m_uci_handler.send_isready();
        std::unique_lock lock{m_mutex};
        m_condvar.wait(lock, [&]() { return m_received_callback == Callback::IsReady; });
        m_uci_handler.send_position({.fen = position_fen});
        m_uci_handler.send_go({.depth = expected_depth});
        m_condvar.wait(lock, [&]() { return m_received_callback == Callback::BestMove; });
        if (record.bm.size() < max_variants) {
            break;
        }
        max_variants *= 2;
        m_received_callback = Callback::None;
    }
    m_current_record = nullptr;
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

} // namespace chessengine::mate_in_x
