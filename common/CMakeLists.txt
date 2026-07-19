add_library(chess_common Board.cpp
        uci.h
        uci.cpp)
target_include_directories(chess_common PUBLIC .)