 cmake_minimum_required(VERSION 3.16)

 project(intrusive_list_cpp)

 set(CMAKE_CXX_STANDARD 20)

 project(dionysus-data-structure)

 add_library(dbl INTERFACE)

 target_include_directories(dbl INTERFACE include/)

 add_subdirectory(test)
