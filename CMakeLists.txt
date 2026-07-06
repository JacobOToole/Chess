cmake_minimum_required(VERSION 3.20)
project(Chess)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON)
FetchContent_MakeAvailable(SFML)

add_executable(Chess main.cpp
        Board.cpp
        Board.h
        Renderer.cpp
        Renderer.h
        Piece.cpp
        Piece.h
        Square.h)

target_link_libraries(Chess PRIVATE sfml-graphics sfml-window sfml-system)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})