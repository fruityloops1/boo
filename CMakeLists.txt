cmake_minimum_required(VERSION 3.5)
project(boo)

add_subdirectory(lib/oead)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 20)

add_executable(boo src/boo.cpp)
target_link_libraries(boo PUBLIC oead)