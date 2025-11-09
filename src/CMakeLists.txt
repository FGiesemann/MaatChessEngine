add_library(ChessEngineLib
    chessengine/chess_engine.cpp
    chessengine/config.cpp
    chessengine/evaluation.cpp
    chessengine/minimax.cpp
    chessengine/search.cpp
    chessengine/types.cpp
)
add_compiler_warnings(ChessEngineLib)
target_compile_features(ChessEngineLib PUBLIC cxx_std_23)
target_include_directories(ChessEngineLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(ChessEngineLib PUBLIC ChessGame::ChessGame)

add_executable(maat
    app/main.cpp

    app/uci.cpp
)
add_compiler_warnings(maat)
target_link_libraries(maat PRIVATE ChessEngineLib ChessUCI::ChessUCI)
