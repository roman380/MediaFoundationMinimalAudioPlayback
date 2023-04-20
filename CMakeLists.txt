# https://cmake.org/examples/
cmake_minimum_required (VERSION 3.0.11)
set(CMAKE_CXX_STANDARD 17)
project(Sandbox)
add_executable(Sandbox Sandbox.cpp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)
