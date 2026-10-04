cmake_minimum_required(VERSION 3.14)
project(CDMA)

set(CMAKE_CXX_STANDARD 17)

# Everything in src/ except main.cpp is built as a library, so the simulation executable and the tests both use the same code
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX "/src/main\\.cpp$")

add_library(cdma_core ${LIB_SOURCES})
target_include_directories(cdma_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# GTEST
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows/MSVC-style CRT flags — harmless on MinGW too
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Every .cpp under tests/ is compiled once, into an object library that both executables below include
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
add_library(cdma_test_cases OBJECT ${TEST_SOURCES})
target_link_libraries(cdma_test_cases PUBLIC cdma_core GTest::gtest)

# The simulation. It can also run all the tests itself (see main.cpp).
add_executable(CDMA src/main.cpp $<TARGET_OBJECTS:cdma_test_cases>)
target_link_libraries(CDMA PRIVATE cdma_core GTest::gtest)

# The same tests as a plain gtest executable, for ctest and for --gtest_filter runs
add_executable(cdma_tests $<TARGET_OBJECTS:cdma_test_cases>)
target_link_libraries(cdma_tests PRIVATE cdma_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(cdma_tests)
