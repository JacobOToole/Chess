//
// Created by jacob on 6/22/2026.
//

#include "Renderer.h"
#include "../common/Board.h"
#include <iostream>

Renderer::Renderer(int squareSize)
    : squareSize_(squareSize),
      lightColour_(240, 217, 181),
      darkColour_(181, 136, 99) {
}

bool Renderer::loadTextures() {
    struct Entry { Colour color; PieceType type; const char* file; };
    const Entry entries[] = {
    { Colour::White, PieceType::Pawn, "assets/white_pawn.png" },
    { Colour::White, PieceType::Knight, "assets/white_knight.png" },
    { Colour::White, PieceType::Bishop, "assets/white_bishop.png" },
    { Colour::White, PieceType::Rook, "assets/white_rook.png" },
    { Colour::White, PieceType::Queen, "assets/white_queen.png" },
    { Colour::White, PieceType::King, "assets/white_king.png" },
    { Colour::Black, PieceType::Pawn, "assets/black_pawn.png" },
    { Colour::Black, PieceType::Knight, "assets/black_knight.png" },
    { Colour::Black, PieceType::Bishop, "assets/black_bishop.png" },
    { Colour::Black, PieceType::Rook, "assets/black_rook.png" },
    { Colour::Black, PieceType::Queen, "assets/black_queen.png" },
    { Colour::Black, PieceType::King, "assets/black_king.png" },
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

bool Renderer::loadFont() {
    if (!font_.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return false;
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

void Renderer::drawCoordinateLabels(sf::RenderWindow &window) const {
    const sf::Color labelColour(80, 80, 80);
    const unsigned int charSize = 14;
    const float padding = 4.0f;

    // Column letters
    for (int col = 0; col < 8; ++col) {
        sf::Text text;
        text.setFont(font_);
        text.setString(std::string(1, static_cast<char>('a' + col)));
        text.setCharacterSize(charSize);
        text.setFillColor(labelColour);

        sf::FloatRect bounds = text.getLocalBounds();
        float x = (col + 1) * squareSize_ - bounds.width - padding;
        float y = 8 * squareSize_ - bounds.height - padding - 2;
        text.setPosition(x, y);
        window.draw(text);
    }

    // Row numbers
    for (int row = 0; row < 8; ++row) {
        sf::Text text;
        text.setFont(font_);
        text.setString(std::to_string(8 - row));
        text.setCharacterSize(charSize);
        text.setFillColor(labelColour);

        sf::FloatRect bounds = text.getLocalBounds();
        float x = 8 * squareSize_ - bounds.width - padding;
        float y = row * squareSize_ - padding + 3;
        text.setPosition(x, y);
        window.draw(text);
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

void Renderer::drawLastMove(sf::RenderWindow &window, const Board &board) const {
    Square from = board.lastMoveFrom();
    Square to = board.lastMoveTo();

    sf::Color tint{200, 230, 0, 50};

    sf::RectangleShape square(sf::Vector2f(squareSize_, squareSize_));
    square.setFillColor(tint);

    square.setPosition(from.col * squareSize_, from.row * squareSize_);
    window.draw(square);

    square.setPosition(to.col * squareSize_, to.row * squareSize_);
    window.draw(square);
}

void Renderer::drawCheckHighlights(sf::RenderWindow &window, const Board &board) const {
    Colour side = board.sideToMove();
    if (!board.isInCheck(side)) return;

    Square kSquare = board.kingSquare(side);

    sf::RectangleShape checkHighlight(sf::Vector2f(squareSize_, squareSize_));
    checkHighlight.setPosition(kSquare.col * squareSize_, kSquare.row * squareSize_);
    checkHighlight.setFillColor(sf::Color(255, 0, 0, 130));
    window.draw(checkHighlight);
}

void Renderer::drawPromotionPicker(sf::RenderWindow &window, Square promotionSquare, Colour promotingColour) const {
    const PieceType options[4] = {
        PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight
    };

    int rowStep = (promotingColour == Colour::White) ? +1 : -1;

    for (int i = 0; i < 4; ++i) {
        int row = promotionSquare.row + i * rowStep;

        sf::RectangleShape bg(sf::Vector2f(squareSize_, squareSize_));
        bg.setPosition(promotionSquare.col * squareSize_, row * squareSize_);
        bg.setFillColor(sf::Color(240, 240, 240, 245));
        bg.setOutlineThickness(1.0f);
        bg.setOutlineColor(sf::Color::Black);
        window.draw(bg);

        drawPiece(window, {options[i], promotingColour}, row, promotionSquare.col);
    }
}

void Renderer::drawGameOverBanner(sf::RenderWindow &window, Board::GameState state, Colour sideToMove) const {
    if (state == Board::GameState::Ongoing) return;

    sf::RectangleShape dim(sf::Vector2f(8 * squareSize_, 8 * squareSize_));
    dim.setFillColor(sf::Color(0, 0, 0, 128));
    window.draw(dim);

    std::string message;

    if (state == Board::GameState::Checkmate) {
        message = (sideToMove == Colour::White) ? "Black wins by checkmate" : "White wins by checkmate";
    } else if (state == Board::GameState::Stalemate) {
        message = "Stalemate";
    } else if (state == Board::GameState::DrawByInsufficientMaterial) {
        message = "Draw due to insufficient material";
    }

    // End game message
    sf::Text text;
    text.setFont(font_);
    text.setString(message);
    text.setCharacterSize(28);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.0f);
    text.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = text.getLocalBounds();
    float x = (8 * squareSize_) / 2.0f - bounds.width / 2.0f;
    float y = (8 * squareSize_) / 2.0f - bounds.height / 2.0f - 8.0f;
    text.setPosition(x, y);

    window.draw(text);

    // New game subtitle
    sf::Text subtitle;
    subtitle.setFont(font_);
    subtitle.setString("Press N for a new game");
    subtitle.setCharacterSize(14);
    subtitle.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect subBounds = subtitle.getLocalBounds();
    subtitle.setPosition((8 * squareSize_) / 2.0f - subBounds.width / 2.0f,
                         y + bounds.height + 12);
    window.draw(subtitle);
}
