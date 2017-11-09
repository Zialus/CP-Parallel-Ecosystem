##### GENERAL CONFIG ZONE #####

cmake_minimum_required(VERSION 3.0)
project(cp-rabits-and-foxes)

set(CMAKE_CXX_STANDARD 14)

##### OpenMP ZONE #####
FIND_PACKAGE(OpenMP REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

##### EXECUTABLE ZONE #####
set(SOURCE_FILES main.cpp main.hpp)
add_executable(ecosystem ${SOURCE_FILES})

##### TESTS ZONE #####
enable_testing()
add_test(input5x5 ecosystem -f ../tests/input5x5 -np 4 -pt)
add_test(input10x10 ecosystem -f ../tests/input10x10 -np 4 -pt)
add_test(input20x20 ecosystem -f ../tests/input20x20 -np 4 -pt)
add_test(input100x100 ecosystem -f ../tests/input100x100 -np 4 -pt)
add_test(input100x100_unbal01 ecosystem -f ../tests/input100x100_unbal01 -np 4 -pt)
add_test(input100x100_unbal02 ecosystem -f ../tests/input100x100_unbal02 -np 4 -pt)
add_test(input200x200 ecosystem -f ../tests/input200x200 -np 4 -pt)
