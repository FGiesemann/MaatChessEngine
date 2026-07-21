cmake_minimum_required(VERSION 3.15)
project(MaatChessEngine VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(BUILD_TESTING "Build unittests" ON)

include(FetchContent)
FetchContent_Declare(
    ChessBuild
    GIT_REPOSITORY https://github.com/FGiesemann/ChessBuild.git
    GIT_TAG        v0.8.6
)
FetchContent_MakeAvailable(ChessBuild)
message(STATUS "CHESS BUILD MODULE DIR: ${CHESS_BUILD_MODULE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${CHESS_BUILD_MODULE_DIR})
include(CompilerWarnings)
include(CompilerSettings)

find_package(chesscore CONFIG REQUIRED COMPONENTS chesscore)
find_package(chessgame CONFIG REQUIRED)
find_package(chessuci CONFIG REQUIRED)

add_library(ChessEngineLib
    src/chessengine/chess_engine.cpp
    src/chessengine/config.cpp
    src/chessengine/evaluation.cpp
    src/chessengine/logger.cpp
    src/chessengine/test_engine.cpp
    src/chessengine/types.cpp
    src/chessengine/uci_adapter.cpp
)
add_compiler_warnings(ChessEngineLib)
add_optimization_settings(ChessEngineLib)
target_compile_features(ChessEngineLib PUBLIC cxx_std_23)
target_compile_options(ChessEngineLib PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)
target_include_directories(ChessEngineLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(ChessEngineLib PUBLIC
    chesscore::chesscore
    ChessUCI::ChessUCI
)

add_executable(maat
    src/app/main.cpp
)
add_compiler_warnings(maat)
add_optimization_settings(maat)
target_link_libraries(maat PRIVATE ChessEngineLib)
target_compile_options(maat PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)

install(TARGETS ChessEngineLib
    EXPORT ChessEngineTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/chessengine
)
install(DIRECTORY include/chessengine/ DESTINATION include/chessengine)
install(EXPORT ChessEngineTargets
    FILE ChessEngineTargets.cmake
    NAMESPACE ChessEngine::
    DESTINATION lib/cmake/ChessEngine
)
set(ChessEngine_VERSION ${PROJECT_VERSION})

include(CMakePackageConfigHelpers)
set(chessengine_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/chessengine")
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ChessEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ChessEngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChessEngine
    PATH_VARS
        chessengine_INCLUDE_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ChessEngineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ChessEngineConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ChessEngineConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChessEngine
)

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/api")
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile 
            @ONLY
        )

    add_custom_command(
        OUTPUT ${DOXYGEN_OUTPUT_DIR}/html/index.html
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMAND_EXPAND_LISTS
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )

    add_custom_target(doc ALL DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html)

    install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html DESTINATION share/doc/${PROJECT_NAME}
        PATTERN "*.map" EXCLUDE
        PATTERN "*.md5" EXCLUDE
    )
    endif()
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION share/doc/${PROJECT_NAME})

if (NOT BUILD_TESTING STREQUAL OFF)
    enable_testing()
    find_package(Catch2 3 REQUIRED)
    add_subdirectory(test)
endif()
