//
// Created by jacob on 6/22/2026.
//

#include "Board.h"
#include <cstdlib>

#include "SFML/Graphics/Color.hpp"

Board::Board() {
    const PieceType backRank[8] = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook
    };

    for (int col = 0; col < 8; ++col) {
        squares_[idx({0, col})] = {backRank[col], Colour::Black};
        squares_[idx({1, col})] = {PieceType::Pawn, Colour::Black};
        squares_[idx({6, col})] = {PieceType::Pawn, Colour::White};
        squares_[idx({7, col})] = {backRank[col], Colour::White};
    }
}

Board::GameState Board::state() const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Square from{row, col};
            Piece piece = at(from);
            if (piece.empty() || piece.colour != sideToMove_) continue;

            for (const Square& to : legalDestinations(from)) {
                if (isLegalMove(from, to)) {
                    return GameState::Ongoing;
                }
            }
        }
    }
    return isInCheck(sideToMove_) ? GameState::Checkmate : GameState::Stalemate;
}



void Board::addSlidingMoves(Square from,
                            std::initializer_list<std::pair<int, int>> directions,
                            std::vector<Square>& out) const {
    Piece moving = at(from);
    for (auto& [dRow, dCol]: directions) {
        Square to{from.row + dRow, from.col + dCol};
        while (to.onBoard()) {
            Piece target = at(to);
            if (target.empty()) {
                out.push_back(to);
            } else {
                if (target.colour != moving.colour) {
                    out.push_back(to);
                }
                break;
            }
            to.row += dRow;
            to.col += dCol;
        }
    }
}
void Board::addCastlingMoves(Square from, std::vector<Square> &out) const {
    Piece moving = at(from);
    int row = (moving.colour == Colour::White) ? 7 : 0;
    Colour enemyColour = (moving.colour == Colour::White) ? Colour::Black : Colour::White;

    // Can't castle if in check
    if (isSquareAttacked({row, 4}, enemyColour)) return;

    bool kingside = (moving.colour == Colour::White) ? whiteKingside_ : blackKingside_;
    bool queenside = (moving.colour == Colour::White) ? whiteQueenside_ : blackQueenside_;

    // Check if squares between king and rook are empty and not under attack
    if (kingside) {
        if (at({row, 5}).empty() && at({row, 6}).empty()
          && !isSquareAttacked({row, 5}, enemyColour)
          && !isSquareAttacked({row, 6}, enemyColour)) {
            out.push_back({row, 6});
        }
    }
    if (queenside) {
        if (at({row, 1}).empty() && at({row, 2}).empty() && at({row, 3}).empty()
          && !isSquareAttacked({row, 2}, enemyColour)
          && !isSquareAttacked({row, 3}, enemyColour)) {
            out.push_back({row, 2});
          }
    }
}

void Board::addKnightMoves(Square from, std::vector<Square> &out) const {
    // Returns list of possible moves for a selected knight piece

    const int deltas[8][2] = {
        {-2, -1}, {-2, +1}, {-1, -2}, {-1, +2},
        {+1, -2}, {+1, +2}, {+2, -1}, {+2, +1}
    };

    Piece moving = at(from);
    for (auto &d: deltas) {
        // Simulate potential destination square
        Square to{from.row + d[0], from.col + d[1]};
        // Check if on board, continue to next loop iteration if not
        if (!to.onBoard()) continue;

        Piece target = at(to);
        // if target square is empty OR the colour of the piece on that square is not the same colour,
        // we add it to list of possible moves
        if (target.empty() || target.colour != moving.colour) {
            out.push_back(to);
        }
    }
}
void Board::addKingMoves(Square from, std::vector<Square> &out) const {
    const int deltas[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1},
        {0, -1}, {0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    };

    Piece moving = at(from);
    for (auto &d: deltas) {
        Square to{from.row + d[0], from.col + d[1]};
        if (!to.onBoard()) continue;

        Piece target = at(to);

        if (target.empty() || target.colour != moving.colour) {
            out.push_back(to);
        }
    }
}
void Board::addPawnMoves(Square from, std::vector<Square> &out) const {
    Piece moving = at(from);
    int dir = (moving.colour == Colour::White) ? -1 : +1;
    int startRow = (moving.colour == Colour::White) ? 6 : 1;

    Square oneAhead{from.row + dir, from.col};
    if (oneAhead.onBoard() && at(oneAhead).empty()) {
        out.push_back(oneAhead);

        Square twoAhead{from.row + 2 * dir, from.col};
        if (from.row == startRow && twoAhead.onBoard() && at(twoAhead).empty()) {
            out.push_back(twoAhead);
        }
    }

    for (int dCol : {+1, -1}) {
        Square diag{from. row + dir, from.col + dCol};
        if (!diag.onBoard()) continue;

        Piece target = at(diag);
        if ((!target.empty() && target.colour != moving.colour) || diag == enPassantTarget_){
            out.push_back(diag);
        }
    }
}
void Board::addPawnAttacks(Square from, std::vector<Square> &out) const {
    Piece moving = at(from);
    int dir = (moving.colour == Colour::White) ? -1 : +1;

    for (int dCol : {+1, -1}) {
        Square diag{from.row + dir, from.col + dCol};
        if (diag.onBoard()) {
            out.push_back(diag);
        }
    }
}
void Board::addBishopMoves(Square from, std::vector<Square> &out) const {
    addSlidingMoves(from, { {-1, -1}, {-1, 1}, {1, -1}, {+1, 1} } , out);
}
void Board::addRookMoves(Square from, std::vector<Square> &out) const {
    addSlidingMoves(from, { {-1, 0}, {0, -1}, {0, 1}, {1, 0} } , out);
}
void Board::addQueenMoves(Square from, std::vector<Square> &out) const {
    addSlidingMoves(from, {
        {-1, -1}, {-1, 0}, {-1, +1},
        {0, -1}, {0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    }, out);
}

std::vector<Square> Board::legalDestinations(Square from) const {
    std::vector<Square> result;
    Piece p = at(from);
    if (p.empty()) return result;   

    switch (p.type) {
        case PieceType::Pawn: addPawnMoves(from, result);
            break;
        case PieceType::Knight: addKnightMoves(from, result);
            break;
        case PieceType::Bishop: addBishopMoves(from, result);
            break;
        case PieceType::Rook: addRookMoves(from, result);
            break;
        case PieceType::Queen: addQueenMoves(from, result);
            break;
        case PieceType::King:
            addKingMoves(from, result);
            addCastlingMoves(from, result);
            break;
    }
    return result;
}
std::vector<Square> Board::legalMoves(Square from) const {
    std::vector<Square> result;

    for (const Square& to : legalDestinations(from)) {
        if (isLegalMove(from, to)) {
            result.push_back(to);
        }
    }
    return result;
}
bool Board::isLegalMove(Square from, Square to) const {
    if (!from.onBoard() || !to.onBoard()) return false;

    Piece p = at(from);
    if (p.empty()) return false;
    if (p.colour != sideToMove_) return false;

    std::vector<Square> dests = legalDestinations(from);
    bool found = false;
    for (const Square &s: dests) {
        if (s == to) {found = true; break;}
    }
    if (!found) return false;

    Board copy{*this};
    copy.squares_[idx(to)] = copy.squares_[idx(from)];
    copy.squares_[idx(from)] = Piece{};

    Colour enemy = (p.colour == Colour::White) ? Colour::Black : Colour::White;
    return !copy.isSquareAttacked(copy.kingSquare(p.colour), enemy);
}

bool Board::isSquareAttacked(Square target, Colour byColour) const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Square from(row, col);
            Piece p = at(from);
            if (p.empty() || p.colour != byColour) continue;

            std::vector<Square> attacks;
            switch (p.type) {
                case PieceType::Pawn:   addPawnAttacks(from, attacks); break;
                case PieceType::Knight: addKnightMoves(from, attacks); break;
                case PieceType::Bishop: addBishopMoves(from, attacks); break;
                case PieceType::Rook:   addRookMoves(from, attacks);   break;
                case PieceType::Queen:  addQueenMoves(from, attacks);  break;
                case PieceType::King:   addKingMoves(from, attacks);   break;
                default: break;
            }

            for (Square &s : attacks) {
                if (s == target) return true;
            }
        }
    }
    return false;
}
Square Board::kingSquare(Colour colour) const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Square square(row, col);
            Piece piece = at(square);
            if (piece.type == PieceType::King && piece.colour == colour) {
                return square;
            }
        }
    }
    return {-1, -1};
}
bool Board::isInCheck(Colour colour) const {
    Square kSquare = kingSquare(colour);
    Colour enemyColour = (colour == Colour::White) ? Colour::Black : Colour::White;
    return isSquareAttacked(kSquare, enemyColour);
}
void Board::determineCastlingRights() {
    // White king moved
    if (at({7, 4}).type != PieceType::King || at({7, 4}).colour != Colour::White) {
        whiteKingside_ = whiteQueenside_ = false;
    }
    // Black king moved
    if (at({0, 4}).type != PieceType::King || at({0, 4}).colour != Colour::Black) {
        blackKingside_ = blackQueenside_ = false;
    }
    // White A rook moved
    if (at({7, 0}).type != PieceType::Rook || at({7, 0}).colour != Colour::White) {
        whiteQueenside_ = false;
    }
    // White H rook moved
    if (at({7, 7}).type != PieceType::Rook || at({7, 7}).colour != Colour::White) {
        whiteKingside_ = false;
    }
    // Black A rook moved
    if (at({0, 0}).type != PieceType::Rook || at({0, 0}).colour != Colour::Black) {
        blackQueenside_ = false;
    }
    // Black H rook moved
    if (at({0, 7}).type != PieceType::Rook || at({0, 7}).colour != Colour::Black) {
        blackKingside_ = false;
    }
}

void Board::makeMove(Square from, Square to, PieceType promoteTo) {
    Piece moving = at(from);
    int dir = (moving.colour == Colour::White) ? -1 : 1;

    // castling. if king moves two columns, relocate rook
    if (moving.type == PieceType::King && std::abs(to.col - from.col) == 2) {
        if (to.col == 6) { // kingside
            squares_[idx({to.row, 5})] = squares_[idx({to.row, 7})];
            squares_[idx({to.row, 7})] = Piece{};
        } else if (to.col == 2) { // queenside
            squares_[idx({to.row, 3})] = squares_[idx({to.row, 0})];
            squares_[idx({to.row, 0})] = Piece{};
        }
    }

    // Check for en passant privileges
    if (moving.type == PieceType::Pawn && std::abs(from.row - to.row) == 2) {
        enPassantTarget_ = Square (from.row + dir, from.col);
    } else {
        enPassantTarget_ = {-1, -1};
    }

    // Check if move being made is en passant
    if (moving.type == PieceType::Pawn && std::abs(from.col - to.col) == 1 && at(to).empty()) {
        squares_[idx(Square{from.row, to.col})] = Piece{};
    }

    squares_[idx(to)] = squares_[idx(from)];
    squares_[idx(from)] = Piece{};

    // promotion
    Piece& landed = squares_[idx(to)];
    if (landed.type == PieceType::Pawn &&
        ((landed.colour == Colour::White && to.row == 0) ||
        (landed.colour == Colour::Black && to.row == 7))) {
            landed.type = promoteTo;
        }

    lastFrom_ = from;
    lastTo_ = to;
    sideToMove_ = (sideToMove_ == Colour::White) ? Colour::Black : Colour::White;

    determineCastlingRights();
}

bool Board::isPromotionMove(Square from, Square to) const {
    Piece p = at(from);
    if (p.type != PieceType::Pawn) return false;
    return (p.colour == Colour::White && to.row == 0) ||
           (p.colour == Colour::Black && to.row == 7);
}

bool Board::isInsufficientMaterial() const {
    int whiteMinors = 0, blackMinors = 0;

    for (int i = 0; i < 64; ++i) {
        Piece p = squares_[i];
        if (p.empty()) continue;

        // any of these pieces means they can still deliver mate
        if (p.type == PieceType::Queen || p.type == PieceType::Rook || p.type == PieceType::Pawn) {
            return false;
        }

        if (p.type == PieceType::Knight || p.type == PieceType::Bishop) {
            (p.colour == Colour::White ? whiteMinors : blackMinors)++;
        }
    }
    return whiteMinors <= 1 || blackMinors <= 1;
}