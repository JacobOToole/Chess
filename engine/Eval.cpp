//
// Created by jacob on 7/21/2026.
//

#include "Eval.h"

int evaluate(const Board& board) {
    const int pieceValues[7] {
        0, // None
        1, // Pawn
        3, // Knight
        3, // Bishop
        5, // Rook
        9, // Queen
        200 // King
    };

    int score = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece p = board.at({row, col});
            if (p.empty()) continue;

            int value = pieceValues[static_cast<int>(p.type)];
            if (p.colour == Colour::White) score += value;
            else score -= value;
        }
    }

    return score;
}