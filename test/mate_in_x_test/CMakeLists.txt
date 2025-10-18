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

find_package(chesscore CONFIG REQUIRED COMPONENTS chesscore)
find_package(chessuci CONFIG REQUIRED)

target_link_libraries(lichess_converter
    PRIVATE
    chesscore::chesscore
    ChessUCI::ChessUCI
)
