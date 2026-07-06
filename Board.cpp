//
// Created by jacob on 6/22/2026.
//

#include "Board.h"

#include "SFML/Graphics/Color.hpp"

Board::Board() {
    const PieceType backRank[8] = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook
    };

    for (int col = 0; col < 8; ++col) {
        squares_[idx({0, col})] = {backRank[col], Colour::Black};
        squares_[idx({1, col})] = {PieceType::Pawn, Colour::Black};
        squares_[idx({6, col})] = {PieceType::Pawn, Colour::White};
        squares_[idx({7, col})] = {backRank[col], Colour::White};
    }
}

Piece Board::at(Square square) const {
    return squares_[idx(square)];
}

void Board::addSlidingMoves(Square from,
                            std::initializer_list<std::pair<int, int>> directions,
                            std::vector<Square>& out) const {
    Piece moving = at(from);
    for (auto& [dRow, dCol]: directions) {
        Square to{from.row + dRow, from.col + dCol};
        while (to.onBoard()) {
            Piece target = at(to);
            if (target.empty()) {
                out.push_back(to);
            } else {
                if (target.colour != moving.colour) {
                    out.push_back(to);
                }
                break;
            }
            to.row += dRow;
            to.col += dCol;
        }
    }
}

void Board::addKnightMoves(Square from, std::vector<Square> &out) const {
    // Returns list of possible moves for a selected knight piece

    const int deltas[8][2] = {
        {-2, -1}, {-2, +1}, {-1, -2}, {-1, +2},
        {+1, -2}, {+1, +2}, {+2, -1}, {+2, +1}
    };

    Piece moving = at(from);
    for (auto &d: deltas) {
        // Simulate potential destination square
        Square to{from.row + d[0], from.col + d[1]};
        // Check if on board, continue to next loop iteration if not
        if (!to.onBoard()) continue;

        Piece target = at(to);
        // if target square is empty OR the colour of the piece on that square is not the same colour,
        // we add it to list of possible moves
        if (target.empty() || target.colour != moving.colour) {
            out.push_back(to);
        }
    }
}

void Board::addKingMoves(Square from, std::vector<Square> &out) const {
    const int deltas[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1},
        {0, -1}, {0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    };

    Piece moving = at(from);
    for (auto &d: deltas) {
        Square to{from.row + d[0], from.col + d[1]};
        if (!to.onBoard()) continue;

        Piece target = at(to);

        if (target.empty() || target.colour != moving.colour) {
            out.push_back(to);
        }
    }
}

void Board::addPawnMoves(Square from, std::vector<Square> &out) const {
    Piece moving = at(from);
    int dir = (moving.colour == Colour::White) ? -1 : +1;
    int startRow = (moving.colour == Colour::White) ? 6 : 1;

    Square oneAhead{from.row + dir, from.col};
    if (oneAhead.onBoard() && at(oneAhead).empty()) {
        out.push_back(oneAhead);

        Square twoAhead{from.row + 2 * dir, from.col};
        if (from.row == startRow && twoAhead.onBoard() && at(twoAhead).empty()) {
            out.push_back(twoAhead);
        }
    }

    for (int dCol : {+1, -1}) {
        Square diag{from. row + dir, from.col + dCol};
        if (!diag.onBoard()) continue;

        Piece target = at(diag);
        if (!target.empty() && target.colour != moving.colour) {
            out.push_back(diag);
        }
    }
}

void Board::addBishopMoves(Square from, std::vector<Square> &out) const {
    addSlidingMoves(from, { {-1, -1}, {-1, 1}, {1, -1}, {+1, 1} } , out);
}

void Board::addRookMoves(Square from, std::vector<Square> &out) const {
    addSlidingMoves(from, { {-1, 0}, {0, -1}, {0, 1}, {1, 0} } , out);
}

void Board::addQueenMoves(Square from, std::vector<Square> &out) const {
    addSlidingMoves(from, {
        {-1, -1}, {-1, 0}, {-1, +1},
        {0, -1}, {0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    }, out);
};

std::vector<Square> Board::legalDestinations(Square from) const {
    std::vector<Square> result;
    Piece p = at(from);
    if (p.empty()) return result;   

    switch (p.type) {
        case PieceType::Pawn: addPawnMoves(from, result);
            break;
        case PieceType::Knight: addKnightMoves(from, result);
            break;
        case PieceType::Bishop: addBishopMoves(from, result);
            break;
        case PieceType::Rook: addRookMoves(from, result);
            break;
        case PieceType::Queen: addQueenMoves(from, result);
            break;
        case PieceType::King: addKingMoves(from, result);
            break;
    }
    return result;
}

std::vector<Square> Board::legalMoves(Square from) const {
    std::vector<Square> result;

    for (const Square& to : legalDestinations(from)) {
        if (isLegalMove(from, to)) {
            result.push_back(to);
        }
    }
    return result;
}

bool Board::isLegalMove(Square from, Square to) const {
    if (!from.onBoard() || !to.onBoard()) return false;

    Piece p = at(from);
    if (p.empty()) return false;
    if (p.colour != sideToMove_) return false;

    std::vector<Square> dests = legalDestinations(from);
    bool found = false;
    for (const Square &s: dests) {
        if (s == to) {found = true; break;}
    }
    if (!found) return false;

    Board copy{*this};
    copy.squares_[idx(to)] = copy.squares_[idx(from)];
    copy.squares_[idx(from)] = Piece{};

    Colour enemy = (p.colour == Colour::White) ? Colour::Black : Colour::White;
    return !copy.isSquareAttacked(copy.kingSquare(p.colour), enemy);
}

bool Board::isSquareAttacked(Square target, Colour byColour) const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Square from(row, col);
            Piece p = at(from);
            if (p.empty() || p.colour != byColour) continue;

            std::vector<Square> attacks;
            switch (p.type) {
                case PieceType::Pawn:   addPawnMoves(from, attacks); break;
                    // Change to pawn attacks since pawns can only move diagonally if a piece is there to capture
                case PieceType::Knight: addKnightMoves(from, attacks); break;
                case PieceType::Bishop: addBishopMoves(from, attacks); break;
                case PieceType::Rook:   addRookMoves(from, attacks);   break;
                case PieceType::Queen:  addQueenMoves(from, attacks);  break;
                case PieceType::King:   addKingMoves(from, attacks);   break;
                default: break;
            }

            for (Square &s : attacks) {
                if (s == target) return true;
            }
        }
    }
    return false;
}

Square Board::kingSquare(Colour colour) const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Square square(row, col);
            Piece piece = at(square);
            if (piece.type == PieceType::King && piece.colour == colour) {
                return square;
            }
        }
    }
    return {-1, -1};
}

bool Board::isInCheck(Colour colour) const {
    Square kSquare = kingSquare(colour);
    Colour enemyColour = (colour == Colour::White) ? Colour::Black : Colour::White;
    return isSquareAttacked(kSquare, enemyColour);
}

void Board::makeMove(Square from, Square to) {
    squares_[idx(to)] = squares_[idx(from)];
    squares_[idx(from)] = Piece{};
    lastFrom_ = from;
    lastTo_ = to;
    sideToMove_ = (sideToMove_ == Colour::White) ? Colour::Black : Colour::White;

}
