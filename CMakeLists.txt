 cmake_minimum_required(VERSION 3.16)

 include(ExternalProject)

 project(intrusive_list_cpp)

 set(CMAKE_CXX_STANDARD 20)

 project(dionysus-data-structure)

 add_library(dbl INTERFACE)

 target_include_directories(dbl INTERFACE include/)

 add_subdirectory(test)

# list(APPEND TEST_CMAKE_ARGS
#         -DCMAKE_AR="/usr/bin/ar"
#         -DCMAKE_C_COMPILER=clang-11
#         -DCMAKE_CXX_COMPILER=clang++-11
#         )
#
# ExternalProject_Add(dbl_test
#         SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test
#         CMAKE_ARGS ${TEST_CMAKE_ARGS})