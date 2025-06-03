cmake_minimum_required(VERSION 3.10)
project(chess-engine LANGUAGES CXX)

# Use C++17 or later
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES
    "${SRC_DIR}/bitboard.cpp"
)

# Define the executable
add_executable(chess-engine ${SOURCES})

# Set compiler warnings per target
target_compile_options(chess-engine PRIVATE
    -Wall
    -Wextra
)

# Set Debug/Release flags
target_compile_options(chess-engine PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
)