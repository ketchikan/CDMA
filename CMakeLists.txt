cmake_minimum_required(VERSION 3.10)
project(CDMA)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(CDMA ${SOURCES})

target_include_directories(CDMA PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# # GTEST STUFF
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/refs/heads/main.zip
# )
# # For Windows/MSVC-style CRT flags — harmless on MinGW too
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

# enable_testing()

# add_executable(my_tests test_main.cpp)
# target_link_libraries(my_tests PRIVATE gtest_main)

# include(GoogleTest)
# gtest_discover_tests(my_tests)