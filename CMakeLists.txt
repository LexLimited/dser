cmake_minimum_required(VERSION 3.27)

set(TARGET dser)

project(${TARGET}
    VERSION 0.0.1
    LANGUAGES CXX
)

file(GLOB_RECURSE CPP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(
	"include"
)

# set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -Wpedantic -std=c++20 -fopenmp")

find_package(OpenSSL)
find_package(GnuTLS)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Choose between library and executable build
option(MAKE_LIB "will build a static library" OFF)

if (MAKE_LIB)
    add_library(${TARGET} STATIC ${CPP_SRC})
else()
    add_executable(${TARGET} ${CPP_SRC})
endif()

target_compile_options(${TARGET} PUBLIC
    -std=c++2c
    -Werror -Wall -Wextra -Wpedantic
    -fopenmp
)

target_link_libraries(${TARGET}
    -fopenmp
    OpenSSL::SSL
    OpenSSL::Crypto
    GnuTLS::GnuTLS
)

target_link_libraries(${TARGET}
    /usr/lib/libpq.so
)
