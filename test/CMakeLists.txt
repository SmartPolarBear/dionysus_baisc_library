project(test)

add_subdirectory(google_test)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(google_test_run list_test.cpp)
target_include_directories(google_test_run PRIVATE ../include)
target_link_libraries(google_test_run gtest gtest_main)

add_test(Test google_test_run)
enable_testing()