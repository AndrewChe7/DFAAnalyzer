cmake_minimum_required(VERSION 3.10)
project(DFAAnalyzer)
set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(imnodes imnodes.cpp imnodes.h imnodes_internal.h)

add_executable(DFAAnalizer main.cpp)
target_link_libraries(DFAAnalizer ${CONAN_LIBS} imnodes)
