add_executable(mate_in_x_test
    src/main.cpp
)
target_link_libraries(mate_in_x_test
    PRIVATE
    ChessEngineLib
    ChessGame::ChessGame
)
target_compile_options(mate_in_x_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)

add_executable(lichess_converter
    src/lichess_converter.cpp
)
target_link_libraries(lichess_converter
    PRIVATE
    chesscore::chesscore
    ChessGame::ChessGame
    ChessUCI::ChessUCI
)
target_compile_features(lichess_converter PUBLIC cxx_std_23)
target_compile_options(lichess_converter PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)

add_executable(multiple_solution_finder
    src/multiple_solution_finder.cpp
)
target_link_libraries(multiple_solution_finder
    PRIVATE
    chesscore::chesscore
    ChessGame::ChessGame
    ChessUCI::ChessUCI
)
target_compile_features(multiple_solution_finder PRIVATE cxx_std_23)
target_compile_options(multiple_solution_finder PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)
