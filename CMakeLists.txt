cmake_minimum_required(VERSION 3.10)
project(CDMA)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(CDMA ${SOURCES})

target_include_directories(CDMA PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)