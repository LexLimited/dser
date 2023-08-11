cmake_minimum_required(VERSION 3.27)

set(TARGET dser)

project(${TARGET})

file(GLOB_RECURSE CPP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(
	"include"
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -O3 -std=c++20 -fopenmp")

find_package(OpenSSL)
find_package(GnuTLS)

add_executable(${TARGET} ${CPP_SRC})

target_link_libraries(${TARGET}
    OpenSSL::SSL
    OpenSSL::Crypto
    GnuTLS::GnuTLS
)

