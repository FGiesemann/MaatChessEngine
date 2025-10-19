add_executable(mate_in_x_test
    src/main.cpp
)

target_link_libraries(mate_in_x_test
    PRIVATE
    ChessEngineLib
)

add_executable(lichess_converter
    src/lichess_converter.cpp
)

target_link_libraries(lichess_converter
    PRIVATE
    chesscore::chesscore
    ChessGame::ChessGame
    ChessUCI::ChessUCI
)
