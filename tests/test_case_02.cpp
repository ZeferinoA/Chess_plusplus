#include "../catch2/catch_amalgamated.hpp"
#include "ChessTestWrapper.h"

TEST_CASE("Invalid moves", "[invalid]") {
    ChessGame game;
    
    // Moving through pieces
    CHECK(game.makeMove("e2e4") == MoveResult::SUCCESS);
    CHECK(game.makeMove("d1h5") == MoveResult::INVALID); // Queen can't jump over pawns
    
    // Moving opponent's piece
    CHECK(game.makeMove("e7e5") == MoveResult::SUCCESS);
    CHECK(game.makeMove("a7a6") == MoveResult::INVALID); // Not black's turn
    
    // Moving to square occupied by own piece
    CHECK(game.makeMove("g1f3") == MoveResult::SUCCESS);
    CHECK(game.makeMove("f3d2") == MoveResult::INVALID); // Square occupied
    
    // Invalid knight move
    CHECK(game.makeMove("f3f6") == MoveResult::INVALID); // Not L-shaped
}