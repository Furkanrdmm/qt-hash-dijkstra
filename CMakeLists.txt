cmake_minimum_required(VERSION 3.16)
project(qt-hash-dijkstra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# spdlog derleme sırasında GitHub'dan indirilir, repoya kopyalanmaz
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

add_executable(qt-hash-dijkstra
    main.cpp
)

target_link_libraries(qt-hash-dijkstra PRIVATE Qt6::Widgets spdlog::spdlog)
