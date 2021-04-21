project(test)

set(CMAKE_AR "/usr/bin/ar")
set(CMAKE_CXX_STANDARD 20)

add_executable(google_test_run list_test.cpp avl_tree_test.cpp utility_test.cpp)

if (BUILD_GTEST)
    target_include_directories(google_test_run
            PRIVATE ${gtest_SOURCE_DIR}/include
            PRIVATE ${gtest_SOURCE_DIR})
endif ()

target_include_directories(google_test_run PRIVATE ../include)

target_compile_options(google_test_run PRIVATE -Wthread-safety)

target_link_libraries(google_test_run gtest gtest_main)

target_link_options(google_test_run PRIVATE -lpthread)

add_test(Test google_test_run)

enable_testing()