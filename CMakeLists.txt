cmake_minimum_required(VERSION 3.10)
project(chess-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/bitboard.cpp"
    "${SRC_DIR}/position.cpp"
)

# Define the executable
add_executable(chess-engine ${SOURCES})

# Warnings (applied to all configs)
target_compile_options(chess-engine PRIVATE
    -Wall
    -Wextra
)
