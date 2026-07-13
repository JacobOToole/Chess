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

    enum class GameState {
        Ongoing,
        Checkmate,
        Stalemate,
        DrawByStalemate,
        DrawByFiftyMoveRule,
        DrawByRepetition
    };

    GameState state() const;

    // Return empty piece if square is empty
    Piece at(Square square) const {return squares_[idx(square)];}
    Piece at(int row, int col) const { return at(Square(row, col)); };
    Colour sideToMove() const { return sideToMove_; }

    bool isLegalMove(Square from, Square to) const;
    std::vector<Square> legalDestinations(Square from) const;
    std::vector<Square> legalMoves(Square from) const;

    void makeMove(Square from, Square to, PieceType promoteTo = PieceType::Queen);
    bool isPromotionMove(Square from, Square to) const;

    Square lastMoveFrom() const { return lastFrom_; }
    Square lastMoveTo() const { return lastTo_; }

    Square kingSquare(Colour colour) const;
    bool isSquareAttacked(Square square, Colour byColour) const;
    bool isInCheck(Colour colour) const;
    bool isInsufficientMaterial() const;

    bool canCastleKingside(Colour colour) const {
        return (colour == Colour::White) ? whiteKingside_ : blackKingside_;
    }
    bool canCastleQueenside(Colour colour) const {
        return (colour == Colour::White) ? whiteQueenside_ : blackQueenside_;
    }

    bool canEnPassant(Colour color) const {
    }


private:
    std::array<Piece, 64> squares_{};
    Colour sideToMove_ = Colour::White;
    Square lastFrom_{-1, -1};   // sentinel: no move yet
    Square lastTo_{-1, -1};
    Square enPassantTarget_{-1, -1};

    // Castling rights
    bool whiteKingside_ = true;
    bool whiteQueenside_ = true;
    bool blackKingside_ = true;
    bool blackQueenside_ = true;

    void addSlidingMoves(Square from,
                     std::initializer_list<std::pair<int, int>> directions,
                     std::vector<Square>& out) const;
    void addPawnMoves(Square from, std::vector<Square>& out) const;
    void addPawnAttacks(Square from, std::vector<Square>& out) const;
    void addKnightMoves(Square from, std::vector<Square>& out) const;
    void addBishopMoves(Square from, std::vector<Square>& out) const;
    void addRookMoves(Square from, std::vector<Square>& out) const;
    void addQueenMoves(Square from, std::vector<Square>& out) const;
    void addKingMoves(Square from, std::vector<Square>& out) const;
    void addCastlingMoves(Square from, std::vector<Square>& out) const;
    void determineCastlingRights();

    static int idx(Square square) { return square.row * 8 + square.col; }

};
