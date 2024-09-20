cmake_minimum_required(VERSION 3.27)
project(Database)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY  https://github.com/fmtlib/fmt
        GIT_TAG         10.2.1
)

FetchContent_MakeAvailable(fmt)

add_executable(untitled5 main.cpp
        Database.cpp
        Database.h
)

target_link_libraries(untitled5 fmt)

