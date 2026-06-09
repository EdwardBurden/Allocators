cmake_minimum_required(VERSION 3.10)
project(Allocators VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE ALLOCATORS "src/*.cpp" "src/*.h")
add_executable(${PROJECT_NAME} ${ALLOCATORS})