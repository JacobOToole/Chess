//
// Created by jacob on 7/20/2026.
//


#include <string>

#include "Uci.h"
#include "Piece.h"
#include "Square.h"

std::string moveToUci (Square from, Square to, PieceType promotion) {
    std::string fromSquare, toSquare, promotionPiece;

    fromSquare += static_cast<char>('a' + from.col);
    fromSquare += static_cast<char>('0' + (8 - from.row));

    toSquare += static_cast<char>('a' + to.col);
    toSquare += static_cast<char>('0' + (8 - to.row));

    switch (promotion) {
        case PieceType::Knight: promotionPiece = "n"; break;
        case PieceType::Bishop: promotionPiece = "b"; break;
        case PieceType::Rook: promotionPiece = "r"; break;
        case PieceType::Queen: promotionPiece = "q"; break;
        default: break;
    }

    return fromSquare + toSquare + promotionPiece;
}

bool uciToMove (const std::string& uci, Square& from, Square& to, PieceType& promotion) {
    if (uci.size() != 4 && uci.size() != 5) return false;

    int fromCol = uci[0] - 'a';
    int fromRow = 8 - (uci[1] - '0');
    int toCol   = uci[2] - 'a';
    int toRow   = 8 - (uci[3] - '0');

    if (fromCol < 0 || fromCol > 7 || fromRow < 0 || fromRow > 7) return false;
    if (toCol   < 0 || toCol   > 7 || toRow   < 0 || toRow   > 7) return false;

    from = { fromRow, fromCol };
    to   = { toRow,   toCol   };

    if (uci.size() == 5) {
        switch (uci[4]) {
            case 'q': promotion = PieceType::Queen;  break;
            case 'r': promotion = PieceType::Rook;   break;
            case 'b': promotion = PieceType::Bishop; break;
            case 'n': promotion = PieceType::Knight; break;
            default: return false;
        }
    } else {
        promotion = PieceType::None;
    }
    return true;
}