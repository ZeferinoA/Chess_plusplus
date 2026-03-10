#include "../catch2/catch_amalgamated.hpp"
#include "ChessTestWrapper.h"

TEST_CASE("Move disambiguation", "[notation]") {
    ChessGame game;
    
    // Move knights
    game.makeMove("b1c3");
    game.makeMove("d7d5");
    
    // Need to specify which knight moves to e4 (UCI is explicit by default)
    CHECK(game.makeMove("c3e4") == MoveResult::SUCCESS);
    
    // File disambiguation equivalent
    game.makeMove("g8f6");
    CHECK(game.makeMove("f1e1") == MoveResult::SUCCESS); 
}

TEST_CASE("Edge cases", "[edge]") {
    ChessGame game;
    
    // Invalid notation
    CHECK(game.makeMove("invalid") == MoveResult::INVALID_NOTATION);
    CHECK(game.makeMove("") == MoveResult::INVALID_NOTATION);
    
    // Moving from empty square 
    CHECK(game.makeMove("e1e5") == MoveResult::INVALID);
    
    // Moving king into check
    game.makeMove("e2e4");
    game.makeMove("e7e5");
    game.makeMove("e1e2"); 
    CHECK(game.makeMove("e2e3") == MoveResult::INVALID); // Would be in check
    
    // Attempting castling through check
    game.makeMove("g1f3");
    game.makeMove("f8b4"); // Bishop attacks f1
    CHECK(game.makeMove("e1g1") == MoveResult::INVALID);
}