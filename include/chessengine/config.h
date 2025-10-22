/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_CONFIG_H
#define CHESSENGINE_CONFIG_H

#include <filesystem>

#include "chessengine/types.h"

namespace chessengine {

/**
 * \brief Configuration parameters for the search algorithm.
 */
struct MinimaxConfig {
    bool use_alpha_beta_pruning{true}; ///< If alpha-beta-pruning should be applied.
};

/**
 * \brief Configuration parameters for the search strategy.
 */
struct SearchConfig {
    Depth max_depth{5};              ///< Maximum search depth.
    bool iterative_deepening{false}; ///< If iterative deepening should be used.
};

/**
 * \brief Configuration for the evaluator.
 *
 * The configuration defines several parameters that influence the evaluation
 * of a chess position.
 */
class EvaluatorConfig {
public:
    /**
     * \brief Get the value for a piece of a given type.
     *
     * \param piece_type Type of the piece.
     * \return Value for a piece of the given type.
     */
    auto piece_value(chesscore::PieceType piece_type) const -> Score { return m_piece_values[get_index(piece_type)]; }
    auto empty_board_value() const -> Score { return Score{0}; }
private:
    Score m_piece_values[6]{Score{100}, Score{300}, Score{300}, Score{500}, Score{900}, Score{0}}; // same order as chesscore piece types

    static_assert(get_index(chesscore::PieceType::Pawn) == 0);
    static_assert(get_index(chesscore::PieceType::Rook) == 1);
    static_assert(get_index(chesscore::PieceType::Knight) == 2);
    static_assert(get_index(chesscore::PieceType::Bishop) == 3);
    static_assert(get_index(chesscore::PieceType::Queen) == 4);
    static_assert(get_index(chesscore::PieceType::King) == 5);
};

/**
 * \brief Configuration for the chess engine.
 *
 * Holds values for the different parameters of the chess engine.
 */
struct Config {
    MinimaxConfig minimax_config;     ///< Configuration of the search algorithm.
    SearchConfig search_config;       ///< Configuration of the search strategy.
    EvaluatorConfig evaluator_config; ///< Configuration of the evaluation function.
};

/**
 * \brief Read the configuration from a file.
 *
 * Reads the configuration parameters for the chess engine from the given file.
 * \param filename Path to the config file.
 * \return The config parameters.
 */
auto read_config_file(const std::filesystem::path &filename) -> Config;

} // namespace chessengine

#endif
