//
// Created by jacob on 7/13/2026.
//

#include <iostream>
#include <sstream>
#include <string>
#include "Board.h"
#include "Uci.h"

int main() {
    Board::initZorbist();
    Board board; // engine's internal position

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "uci") {
            std::cout << "id name Kirushka\n";
            std::cout << "id author Jacob O'Toole\n";
            std::cout << "uciok\n" << std::flush;
        }
        else if (line == "isready") {
            std::cout << "readyok\n" << std::flush;
        }
        else if (command == "ucinewgame") {
            board = Board{};
        }
        // handle "position startpos moves ..." OR "position fen <FEN> moves..."
        else if (command == "position") {
            std::string token;
            iss >> token;
            if (token == "startpos") {
                board = Board{};
                iss >> token;
            }
            else if (token == "fen") {
                // read 6 whitespace-separated fields as the FEN
                std::string fen, part;
                for (int i = 0; i < 6 && iss >> part; ++i) {
                    if (i > 0) fen += " ";
                    fen += part;
                }
                board.setFromFen(fen);
                iss >> token;
            }
            if (token == "moves") {
                std::string moveString;
                while (iss >> moveString) {
                    Square from, to;
                    PieceType promotion;
                    if (uciToMove(moveString, from, to, promotion)) {
                        board.makeMove(from, to, promotion == PieceType::None ? PieceType::Queen : promotion);
                    }
                }
            }
        }
        else if (command == "go") {
            // Pick random legal move
            Square chosenFrom, chosenTo;
            bool found = false;
            for (int row = 0; row < 8 && !found; row++) {
                for (int col = 0; col < 8 && !found; col++) {
                    Square from{row, col};
                    Piece p = board.at(from);
                    if (p.empty() || p.colour != board.sideToMove()) continue;
                    for (const Square& to : board.legalMoves(from)) {
                        chosenFrom = from;
                        chosenTo = to;
                        found = true;
                        break;
                    }
                }
            }
            if (found) {
                std::cout << "bestmove " << moveToUci(chosenFrom, chosenTo) << "\n" << std::flush;
            } else {
                std::cout << "bestmove 0000\n" << std::flush;   // no legal moves
            }
        }

        else if (line == "quit") {
            break;
        }
    }
    return 0;
}
