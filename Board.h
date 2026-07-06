//
// Created by jacob on 6/22/2026.
//
#pragma once
#include "Piece.h"
#include "Square.h"
#include <array>
#include <vector>

class Board {
public:
    Board(); // sets up starting position

    // Return empty piece if square is empty
    Piece at(Square square) const;
    Piece at(int row, int col) const { return at(Square(row, col)); };
    Color sideToMove() const { return sideToMove_; }

    bool isLegalMove(Square from, Square to) const;
    std::vector<Square> legalDestinations(Square from) const;
    void makeMove(Square from, Square to);

    Square lastMoveFrom() const { return lastFrom_; }
    Square lastMoveTo() const { return lastTo_; }

private:
    std::array<Piece, 64> squares_{};
    Color sideToMove_ = Color::White;

    void addSlidingMoves(Square from,
                     std::initializer_list<std::pair<int, int>> directions,
                     std::vector<Square>& out) const;
    void addPawnMoves(Square from, std::vector<Square>& out) const;
    void addKnightMoves(Square from, std::vector<Square>& out) const;
    void addBishopMoves(Square from, std::vector<Square>& out) const;
    void addRookMoves(Square from, std::vector<Square>& out) const;
    void addQueenMoves(Square from, std::vector<Square>& out) const;
    void addKingMoves(Square from, std::vector<Square>& out) const;

    bool isSquareAttacked(Square square, Color byColour) const;

    Square kingSquare(Color colour) const;

    static int idx(Square square) { return square.row * 8 + square.col; }

    Square lastFrom_{-1, -1};   // sentinel: no move yet
    Square lastTo_{-1, -1};
};
