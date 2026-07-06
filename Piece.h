#pragma once

enum class PieceType {
    None = 0,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    Count   // sentinel — equals the number of types above
};

enum class Color {
    White = 0,
    Black,
    Count
};

struct Piece {
    PieceType type = PieceType::None;
    Color colour = Color::White;

    bool empty() const { return type == PieceType::None; }
};