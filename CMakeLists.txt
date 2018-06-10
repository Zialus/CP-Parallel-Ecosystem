##### GENERAL CONFIG ZONE #####

cmake_minimum_required(VERSION 3.9)
project(cp-rabits-and-foxes)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_VERBOSE_MAKEFILE ON)

FIND_PACKAGE(OpenMP REQUIRED)

##### EXECUTABLE ZONE #####
set(SOURCE_FILES src/main.cpp src/main.hpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/exec")

add_executable(ecosystem ${SOURCE_FILES})
target_link_libraries(ecosystem OpenMP::OpenMP_CXX)

##### TESTS ZONE #####
enable_testing()
add_test(NAME input5x5 COMMAND ecosystem -if ../tests/input5x5 -of ../tests/output5x5 -np 4 -pt)
add_test(NAME input10x10 COMMAND ecosystem -if ../tests/input10x10 -of ../tests/output10x10 -np 4 -pt)
add_test(NAME input20x20 COMMAND ecosystem -if ../tests/input20x20 -of ../tests/output20x20 -np 4 -pt)
add_test(NAME input100x100 COMMAND ecosystem -if ../tests/input100x100 -of ../tests/output100x100 -np 4 -pt)
add_test(NAME input100x100_unbal01 COMMAND ecosystem -if ../tests/input100x100_unbal01 -of ../tests/output100x100_unbal01 -np 4 -pt)
add_test(NAME input100x100_unbal02 COMMAND ecosystem -if ../tests/input100x100_unbal02 -of ../tests/output100x100_unbal02 -np 4 -pt)
add_test(NAME input200x200 COMMAND ecosystem -if ../tests/input200x200 -of ../tests/output200x200 -np 4 -pt)
