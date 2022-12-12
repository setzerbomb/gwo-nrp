cmake_minimum_required(VERSION 3.24)
project(uuuu)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP REQUIRED)

add_executable(uuuu main.cpp)

target_link_libraries(uuuu PRIVATE OpenMP::OpenMP_CXX)
