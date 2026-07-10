//
// Created by jacob on 6/22/2026.
//
#pragma once

#include <SFML/Graphics.hpp>
#include "Piece.h"
#include "Square.h"
#include <vector>

class Board;

class Renderer {
public:
    explicit Renderer(int squareSize);

    // Load all 12 piece PNGs from disk. Call once at startup.
    // Returns false if any file failed to load.
    bool loadTextures();

    // Draw checker pattern
    void drawBoard(sf::RenderWindow& window) const;

    // Draw piece at given coordinates
    void drawPiece(sf::RenderWindow& window, Piece piece, int row, int column) const;

    void drawAllPieces(sf::RenderWindow& window, const Board& board) const;

    void drawSelection(sf::RenderWindow& window, Square selected) const;

    void drawMoveIndicators(sf::RenderWindow& window,
                            const std::vector<Square>& destinations,
                            const Board& board) const;

    void drawLastMove(sf::RenderWindow& window, const Board& board) const;

    void drawCheckHighlights(sf::RenderWindow& window, const Board& board) const;

    void drawPromotionPicker(sf::RenderWindow& window, Square promotionSquare, Colour promotingColour) const;

private:
    // Declare private variables and texture array

    int squareSize_;
    sf::Color lightColour_;
    sf::Color darkColour_;

    // textures_[color][type] — e.g. textures_[0][5] is the white king.
    sf::Texture textures_[static_cast<int>(Colour::Count)][static_cast<int>(PieceType::Count)];

};