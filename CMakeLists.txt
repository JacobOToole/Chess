cmake_minimum_required(VERSION 3.20)
project(chess)
set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(SFML)

add_subdirectory(common)
add_subdirectory(gui)
add_subdirectory(engine)