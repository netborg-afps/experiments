cmake_minimum_required(VERSION 3.25)
project(experiments)

set(CMAKE_CXX_STANDARD 20)

add_executable(experiments main.cpp)
target_compile_options(experiments PRIVATE -O2 -Wall -g) # -fsanitize=address)
# target_link_options(experiments PRIVATE -fsanitize=address)

