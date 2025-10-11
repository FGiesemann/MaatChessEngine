add_library(ChessEngineLib
    chessengine/config.cpp
    chessengine/evaluation.cpp
    chessengine/minimax.cpp
    chessengine/search.cpp
)
add_compiler_warnings(ChessEngineLib)
target_compile_features(ChessEngineLib PUBLIC cxx_std_23)
target_include_directories(ChessEngineLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
find_package(chessgame REQUIRED)
target_link_libraries(ChessEngineLib PUBLIC ChessGame::ChessGame)

add_executable(ChessEngine
    app/main.cpp
)
add_compiler_warnings(ChessEngine)
target_link_libraries(ChessEngine PRIVATE ChessEngineLib)
