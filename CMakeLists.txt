cmake_minimum_required(VERSION 2.8)
project(cp_rabits_and_foxes)

set(CMAKE_VERBOSE_MAKEFILE true)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_CXX_COMPILER=clang++)
#set(CMAKE_C_COMPILER=clang)
#FIND_PACKAGE(OpenMP REQUIRED)

#if(OPENMP_FOUND)
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

set(SOURCE_FILES main.cpp main.hpp)
add_executable(ecosystem ${SOURCE_FILES})

enable_testing()
add_test(input5x5 ecosystem ../tests/input5x5)
add_test(input10x10 ecosystem ../tests/input10x10)
add_test(input20x20 ecosystem ../tests/input20x20)
add_test(input100x100 ecosystem ../tests/input100x100)
add_test(input100x100_unbal01 ecosystem ../tests/input100x100_unbal01)
add_test(input100x100_unbal02 ecosystem ../tests/input100x100_unbal02)
#add_test(input200x200 ecosystem ../tests/input200x200)
