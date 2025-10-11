add_library(ChessEngineLib
    chessengine/config.cpp
)
add_compiler_warnings(ChessEngineLib)
target_compile_features(ChessEngineLib PUBLIC cxx_std_23)
find_package(chessgame REQUIRED)
target_link_libraries(ChessEngineLib PUBLIC ChessGame::ChessGame)

add_executable(ChessEngine
    app/main.cpp
)
add_compiler_warnings(ChessEngine)
target_link_libraries(ChessEngine PRIVATE ChessEngineLib)
