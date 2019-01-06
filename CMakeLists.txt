cmake_minimum_required(VERSION 2.8)
project(hasher)

set(SOURCES
    main.cpp
    file.cpp
    file.hpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lcrypto -O2 -ffast-math")

add_executable(${PROJECT_NAME} ${SOURCES})
