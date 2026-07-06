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
