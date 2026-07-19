//
// Created by jacob on 7/20/2026.
//

#ifndef CHESS_UCI_H
#define CHESS_UCI_H
#include <string>

#include "Piece.h"
#include "Square.h"


std::string moveToUci (Square from, Square to, PieceType promotion = PieceType::None);

bool uciToMove (const std::string& uci, Square& from, Square& to, PieceType& promotion);

#endif //CHESS_UCI_H
