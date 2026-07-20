#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include "Renderer.h"
#include "../common/Board.h"
#include "../common/Piece.h"
#include "../common/uci.h"
#include "EngineProcess.h"


int main() {

    try {
        EngineProcess engine("assets/engines/kirushka.exe");;

        engine.sendCommand("uci");
        engine.sendCommand("isready");
        engine.sendCommand("ucinewgame");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));   // let engine process


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

        Colour playerColour = Colour::White;
        bool waitingForEngine = false;

        auto notifyEngineIfItsTurn = [&]() {
            if (board.sideToMove() != playerColour &&
                board.state() == Board::GameState::Ongoing) {
                engine.sendCommand("position fen " + board.toFen());
                engine.sendCommand("go movetime 500");
                waitingForEngine = true;
                }
        };

        if (board.sideToMove() != playerColour) {
            engine.sendCommand("position fen " + board.toFen());
            engine.sendCommand("go movetime 500");
            waitingForEngine = true;
        }

        while (window.isOpen()) {

            // drain engine output that arrived since last frame
            for (const auto& line : engine.takeLines()) {
                std::cout << "<-" << line << "\n";

                // parse bestmove output
                if (line.rfind("bestmove ", 0) == 0) {
                    std::string rest = line.substr(9);
                    size_t space = rest.find(' ');
                    std::string moveStr = (space == std::string::npos) ? rest : rest.substr(0, space);

                    Square from, to;
                    PieceType promo;

                    // make move
                    if (uciToMove(moveStr, from, to, promo)) {
                        board.makeMove(from, to, promo == PieceType::None ? PieceType::Queen : promo); // default to queen when uciToMove reports none
                    }
                    waitingForEngine = false;
                }
            }


            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::N) {
                    board = Board{};                 // reset to starting position
                    selected = {-1, -1};
                    awaitingPromotion = false;
                    waitingForEngine =false;
                    engine.sendCommand("ucinewgme");
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F) {
                    // Print current position as FEN to the console
                    std::cout << "Fen at Move " << board.fullMoveNumber()
                              << ": " << board.toFen() << "\n";
                }

                else if (event.type == sf::Event::MouseButtonPressed &&
                     event.mouseButton.button == sf::Mouse::Left) {

                    if (waitingForEngine) continue;
                    if (board.state() != Board::GameState::Ongoing) continue;
                    if (board.sideToMove() != playerColour) continue;

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
                            notifyEngineIfItsTurn();
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
                                notifyEngineIfItsTurn();
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

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

}