cmake_minimum_required(VERSION 3.20)

set(TARGET dser)

project(${TARGET})

file(GLOB_RECURSE CPP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(
	"include"
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -O0 -g -std=c++20 -fopenmp")

add_executable(${TARGET} ${CPP_SRC})

