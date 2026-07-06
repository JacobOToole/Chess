#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "Board.h"
#include "Piece.h"


int main() {
    sf::RenderWindow window(sf::VideoMode(640, 640), "Chess");

    Renderer renderer(80);
    if (!renderer.loadTextures()) {
        return 1;   // asset load failed; abort
    }

    Board board;

    Square selected{-1, -1};

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::MouseButtonPressed &&
                 event.mouseButton.button == sf::Mouse::Left) {

                Square clicked{event.mouseButton.y / 80, event.mouseButton.x / 80};

                if (!clicked.onBoard()) continue;

                if (selected.row == -1) {
                    // First click — select a piece if it's the right color
                    Piece p = board.at(clicked);
                    if (!p.empty() && p.colour == board.sideToMove()) {
                        selected = clicked;
                    }
                } else {
                    // Second click — try to move
                    if (board.isLegalMove(selected, clicked)) {
                        board.makeMove(selected, clicked);
                    }
                    selected = {-1, -1};
                }
            }
        }

        window.clear(sf::Color::Black);
        renderer.drawBoard(window);

        renderer.drawLastMove(window, board);

        // Compute highlight indicators
        std::vector<Square> destinations;
        if (selected.onBoard()) {
            renderer.drawSelection(window, selected);
            destinations = board.legalDestinations(selected);
        }

        renderer.drawAllPieces(window, board);
        renderer.drawMoveIndicators(window, destinations, board);
        window.display();
    }
    return 0;
}