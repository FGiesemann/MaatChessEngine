add_executable(check_in_x_test
    src/main.cpp
)

target_link_libraries(check_in_x_test
    PRIVATE
    ChessEngineLib
)
