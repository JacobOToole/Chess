//
// Created by jacob on 6/22/2026.
//

#include "Renderer.h"
#include "Board.h"
#include <iostream>

Renderer::Renderer(int squareSize)
    : squareSize_(squareSize),
      lightColour_(240, 217, 181),
      darkColour_(181, 136, 99) {
}

bool Renderer::loadTextures() {
    struct Entry { Color color; PieceType type; const char* file; };
    const Entry entries[] = {
    { Color::White, PieceType::Pawn, "assets/white_pawn.png" },
    { Color::White, PieceType::Knight, "assets/white_knight.png" },
    { Color::White, PieceType::Bishop, "assets/white_bishop.png" },
    { Color::White, PieceType::Rook, "assets/white_rook.png" },
    { Color::White, PieceType::Queen, "assets/white_queen.png" },
    { Color::White, PieceType::King, "assets/white_king.png" },
    { Color::Black, PieceType::Pawn, "assets/black_pawn.png" },
    { Color::Black, PieceType::Knight, "assets/black_knight.png" },
    { Color::Black, PieceType::Bishop, "assets/black_bishop.png" },
    { Color::Black, PieceType::Rook, "assets/black_rook.png" },
    { Color::Black, PieceType::Queen, "assets/black_queen.png" },
    { Color::Black, PieceType::King, "assets/black_king.png" },
    };

    for (const Entry& entry : entries) {
        sf::Texture& tex = textures_[static_cast<int>(entry.color)]
                                    [static_cast<int>(entry.type)];

        if (!tex.loadFromFile(entry.file)) {
            std::cerr << "Error while loading texture " << entry.file << std::endl;
            return false;
        }
        tex.setSmooth(true);
    }
    return true;
}

void Renderer::drawBoard(sf::RenderWindow& window) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            sf::RectangleShape square(sf::Vector2f(squareSize_, squareSize_));
            square.setPosition(col * squareSize_, row * squareSize_);
            square.setFillColor((row + col) % 2 == 0 ? lightColour_ : darkColour_);
            window.draw(square);
        }
    }
}

void Renderer::drawPiece(sf::RenderWindow& window, Piece piece, int row, int column) const {
    sf::Texture tex = textures_[static_cast<int>(piece.colour)]
                                      [static_cast<int>(piece.type)];
    sf::Sprite sprite(tex);

    // Scale so the sprite is exactly squareSize x squareSize
    float scale = static_cast<float>(squareSize_) / tex.getSize().x;
    sprite.setScale(scale, scale);

    sprite.setPosition(column * squareSize_, row * squareSize_);

    window.draw(sprite);
}

void Renderer::drawAllPieces(sf::RenderWindow &window, const Board &board) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece p = board.at(row, col);
            if (!p.empty()) {
                drawPiece(window, p, row, col);
            }
        }
    }
}

void Renderer::drawSelection(sf::RenderWindow &window, Square selected) const {
    if (!selected.onBoard()) return;

    sf::RectangleShape highlight(sf::Vector2f(squareSize_, squareSize_));
    highlight.setPosition(selected.col * squareSize_, selected.row * squareSize_);
    highlight.setFillColor(sf::Color(255, 150, 175, 152));
    window.draw(highlight);
}

void Renderer::drawMoveIndicators(sf::RenderWindow &window, const std::vector<Square> &destinations, const Board &board) const {
    const float squareF = static_cast<float>(squareSize_);

    for (const Square& square : destinations) {
        Piece target = board.at(square);
        if (target.empty()) {
            // empty square, small circle
            float radius = squareF * 0.15f;
            sf::CircleShape dot(radius);
            dot.setOrigin(radius, radius);
            dot.setPosition(square.col * squareF + squareF / 2.0f,
                            square.row * squareF + squareF / 2.0f);
            dot.setFillColor(sf::Color(0, 0, 0, 90));
            window.draw(dot);
        } else {
            // Capture: hollow ring around the piece.
            float radius = squareF * 0.45f;
            sf::CircleShape ring(radius);
            ring.setOrigin(radius, radius);
            ring.setPosition(square.col * squareF + squareF / 2.0f,
                             square.row * squareF + squareF / 2.0f);
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineThickness(4.0f);
            ring.setOutlineColor(sf::Color(0, 0, 0, 120));
            window.draw(ring);
        }
    }
}
