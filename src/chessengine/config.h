/* ************************************************************************** *
 * Chess Engine                                                               *
 * Chess playing engine                                                       *
 * ************************************************************************** */

#ifndef CHESSENGINE_CONFIG_H
#define CHESSENGINE_CONFIG_H

#include <filesystem>

namespace chessengine {

/**
 * \brief Configuration for the chess engine.
 *
 * Holds values for the different parameters of the chess engine.
 */
struct Config {};

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
