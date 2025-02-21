cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

add_executable(shell ${SOURCE_FILES})

include_directories("src/handler")

#own additions
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(shell src/main.cpp src/handler/command/commandHandler.cpp)

