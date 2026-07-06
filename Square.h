//
// Created by jacob on 6/22/2026.
//
#pragma once

struct Square {
    int row;
    int col;

    bool onBoard() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }

    bool operator==(const Square& square) const {
        return row == square.row && col == square.col;
    }
};

struct Move {
    Square from;
    Square to;
};