cmake_minimum_required(VERSION 4.0)
project(micro_maker)

set(CMAKE_CXX_STANDARD 14)

include_directories(engine/include)

add_executable(micro_maker
        engine/include/ring_buffer.h
        engine/include/types.h
        engine/src/main.cpp)
