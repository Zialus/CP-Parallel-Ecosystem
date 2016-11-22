cmake_minimum_required(VERSION 2.8)
project(cp_rabits_and_foxes)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(ecosystem ${SOURCE_FILES})

enable_testing()
add_test(input5x5 ecosystem ../test/input5x5.txt)
add_test(input10x10 ecosystem ../test/input10x10.txt)
add_test(input20x20 ecosystem ../test/input20x20.txt)
add_test(input100x100 ecosystem ../test/input100x100.txt)
add_test(input200x200 ecosystem ../test/input200x200.txt)