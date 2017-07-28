cmake_minimum_required(VERSION 3.0)
project(HXTIO VERSION 1.0 LANGUAGES CXX)

# Require C++11
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED on)

# HXTIO Library
add_library(hxtio HXTIO.cpp)
target_include_directories(hxtio
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Example App
add_executable(HXTIOTest HXTIOTest.cpp)
target_link_libraries(HXTIOTest hxtio)
