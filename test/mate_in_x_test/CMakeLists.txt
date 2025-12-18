add_executable(mate_in_x_test
    src/main.cpp
    src/mate_in_x.cpp
)
target_link_libraries(mate_in_x_test
    PRIVATE
    ChessEngineLib
    ChessGame::ChessGame
)
target_compile_options(mate_in_x_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)

add_executable(lichess_converter
    src/lichess_converter.cpp
    src/multi_solution_finder.cpp
)
target_link_libraries(lichess_converter
    PRIVATE
    chesscore::chesscore
    ChessGame::ChessGame
    ChessUCI::ChessUCI
)
target_compile_features(lichess_converter PRIVATE cxx_std_23)
target_compile_options(lichess_converter PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)
