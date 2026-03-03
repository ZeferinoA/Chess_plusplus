#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

TEST_CASE("Basic pawn moves", "[movement]") {
    ChessGame game;
    
    // White pawn moves two squares forward from starting position
    CHECK(game.makeMove("e4") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("e4") == Piece::WHITE_PAWN);
    CHECK(game.getPieceAt("e2") == Piece::EMPTY);
    
    // Black pawn moves two squares forward
    CHECK(game.makeMove("e5") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("e5") == Piece::BLACK_PAWN);
    
    // White pawn moves one square forward
    CHECK(game.makeMove("d4") == MoveResult::SUCCESS);
}

TEST_CASE("Knight moves", "[movement]") {
    ChessGame game;
    
    // Knight L-shaped movement
    CHECK(game.makeMove("Nf3") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("f3") == Piece::WHITE_KNIGHT);
    
    // Another knight move
    CHECK(game.makeMove("Nc6") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("c6") == Piece::BLACK_KNIGHT);
}