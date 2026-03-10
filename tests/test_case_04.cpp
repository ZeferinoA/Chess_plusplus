#include "../catch2/catch_amalgamated.hpp"
#include "ChessTestWrapper.h"

TEST_CASE("Castling", "[special]") {
    ChessGame game;
    
    // Clear pieces between king and rook
    game.makeMove("g1f3");
    game.makeMove("b8c6");
    game.makeMove("g2g3");
    game.makeMove("b7b6");
    game.makeMove("f1g2");
    game.makeMove("c8b7");
    
    // Kingside castling
    CHECK(game.makeMove("e1g1") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("g1") == Piece::WHITE_KING);
    CHECK(game.getPieceAt("f1") == Piece::WHITE_ROOK);
    
    // Queenside castling for black
    game.makeMove("e8c8");
    CHECK(game.getPieceAt("c8") == Piece::BLACK_KING);
    CHECK(game.getPieceAt("d8") == Piece::BLACK_ROOK);
}

TEST_CASE("En passant", "[special]") {
    ChessGame game;
    
    game.makeMove("e2e4");
    game.makeMove("a7a6");
    game.makeMove("e4e5");
    game.makeMove("d7d5"); // Black pawn moves two squares
    
    // White captures en passant
    CHECK(game.makeMove("e5d6") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("d6") == Piece::WHITE_PAWN);
    CHECK(game.getPieceAt("d5") == Piece::EMPTY);
}

TEST_CASE("Pawn promotion", "[special]") {
    ChessGame game;
    
    // Move pawn to 7th rank
    game.makeMove("a2a4");
    game.makeMove("b7b5");
    game.makeMove("a4b5");
    game.makeMove("a7a6");
    game.makeMove("b5b6");
    game.makeMove("a6a5");
    game.makeMove("b6b7");
    
    // Promote to queen
    CHECK(game.makeMove("b7b8q") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("b8") == Piece::WHITE_QUEEN);
}