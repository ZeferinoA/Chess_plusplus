#include "../catch2/catch_amalgamated.hpp"
#include "ChessTestWrapper.h"

TEST_CASE("Piece captures", "[capture]") {
    ChessGame game;
    
    // Setup pawn capture scenario
    game.makeMove("e2e4");
    game.makeMove("d7d5");
    
    // Pawn capture
    CHECK(game.makeMove("e4d5") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("d5") == Piece::WHITE_PAWN);
    CHECK(game.getPieceAt("e4") == Piece::EMPTY);
    
    // Setup knight capture scenario 
    game.makeMove("g8f6"); // Black plays Nf6
    game.makeMove("b1c3"); // White plays Nc3
    game.makeMove("f6d5"); // Black knight captures the white pawn on d5
    
    // Knight capture (White knight from c3 captures black knight on d5)
    CHECK(game.makeMove("c3d5") == MoveResult::SUCCESS); 
    CHECK(game.getPieceAt("d5") == Piece::WHITE_KNIGHT);
}