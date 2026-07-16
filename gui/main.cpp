#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "../common/Board.h"
#include "../common/Piece.h"
#include <iostream>


int main() {
    Board::initZorbist();
    sf::RenderWindow window(sf::VideoMode(640, 640), "Chess");

    Renderer renderer(80);
    if (!renderer.loadTextures()) return 1;
    if (!renderer.loadFont()) return 1;

    Board board;
    Square selected{-1, -1};
    bool awaitingPromotion = false;
    Square pendingFrom{-1, -1};
    Square pendingTo  {-1, -1};
    Colour pendingColour = Colour::White;

    // TESTING - debug (arbitrary test position)
    board.setFromFen("rnbqkb1r/pp1ppppp/5n2/2pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 3");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
                board = Board{};                 // reset to starting position
                selected = {-1, -1};
                awaitingPromotion = false;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) {
                // Print current position as FEN to the console
                std::cout << "Fen at Move " << board.fullMoveNumber()
                          << ": " << board.toFen() << "\n";
            }

            else if (event.type == sf::Event::MouseButtonPressed &&
                 event.mouseButton.button == sf::Mouse::Left) {

                if (board.state() != Board::GameState::Ongoing) continue;

                Square clicked{event.mouseButton.y / 80, event.mouseButton.x / 80};
                if (!clicked.onBoard()) continue;

                if (awaitingPromotion) {
                    int rowStep = (pendingColour == Colour::White) ? +1 : -1;
                    int slot = (clicked.row - pendingTo.row) * rowStep;

                    if (clicked.col == pendingTo.col && slot >= 0 && slot <= 4) {
                        const PieceType options[4] = {
                            PieceType::Queen, PieceType::Rook,
                            PieceType::Bishop, PieceType::Knight
                        };
                        board.makeMove(pendingFrom, pendingTo, options[slot]);
                    }

                    // Cancel promotion if clicked outside promotion menu
                    awaitingPromotion = false;
                    selected = {-1, -1};
                    continue;
                }

                if (selected.row == -1) {
                    // First click — select a piece if it's the right color
                    Piece p = board.at(clicked);
                    if (!p.empty() && p.colour == board.sideToMove()) {
                        selected = clicked;
                    }
                } else {
                    // Second click — try to move
                    if (board.isLegalMove(selected, clicked)) {
                        if (board.isPromotionMove(selected, clicked)) {
                            awaitingPromotion = true;
                            pendingFrom = selected;
                            pendingTo = clicked;
                            pendingColour = board.at(selected).colour;
                        } else {
                            board.makeMove(selected, clicked);
                        }
                    }
                    if (!awaitingPromotion) {
                        selected = {-1, -1};
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        renderer.drawBoard(window);
        renderer.drawLastMove(window, board);
        renderer.drawCheckHighlights(window, board);

        // Compute highlight indicators
        std::vector<Square> destinations;
        if (selected.onBoard()) {
            renderer.drawSelection(window, selected);
            destinations = board.legalMoves(selected);
        }

        renderer.drawAllPieces(window, board);

        if (!awaitingPromotion) {
            renderer.drawMoveIndicators(window, destinations, board);
        }

        if (awaitingPromotion) {
            renderer.drawPromotionPicker(window, pendingTo, pendingColour);
        }

        renderer.drawCoordinateLabels(window);
        renderer.drawGameOverBanner(window, board.state(), board.sideToMove());
        
        window.display();
    }
    return 0;
}