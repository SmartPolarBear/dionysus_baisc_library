# cmake_minimum_required(VERSION <specify CMake version here>)
project(intrusive_list_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(intrusive_list_cpp main.cpp avl_tree.h hash_table.h skip_list.h priority_queue.h)