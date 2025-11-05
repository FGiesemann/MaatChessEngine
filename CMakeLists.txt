cmake_minimum_required(VERSION 3.15)
project(MaatChessEngine VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(BUILD_TESTING "Build unittests" ON)

include(cmake/compiler_warnings.cmake)

find_package(chesscore CONFIG REQUIRED COMPONENTS chesscore)
find_package(chessgame CONFIG REQUIRED)
find_package(chessuci CONFIG REQUIRED)

add_subdirectory(src)

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
