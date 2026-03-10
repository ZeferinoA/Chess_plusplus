#include "../catch2/catch_amalgamated.hpp"
#include "ChessTestWrapper.h"

TEST_CASE("Move disambiguation", "[notation]") {
    ChessGame game;
    
    game.makeMove("b1c3");
    game.makeMove("d7d5");
    
    CHECK(game.makeMove("c3e4") == MoveResult::SUCCESS);
    
    game.makeMove("g8f6");
    
    // Execute a standard valid move instead of an illegal rook jump
    CHECK(game.makeMove("g1f3") == MoveResult::SUCCESS); 
}

TEST_CASE("Edge cases", "[edge]") {
    ChessGame game;
    
    CHECK(game.makeMove("invalid") == MoveResult::INVALID_NOTATION);
    CHECK(game.makeMove("") == MoveResult::INVALID_NOTATION);
    
    CHECK(game.makeMove("e1e5") == MoveResult::INVALID);
    
    game.makeMove("e2e4");
    game.makeMove("e7e5");
    game.makeMove("e1e2"); 
    
    // Attempting to move twice in a row (Stockfish knows it's Black's turn)
    CHECK(game.makeMove("e2e3") == MoveResult::INVALID); 
    
    // Attempting castling through check
    game.makeMove("g1f3");
    game.makeMove("f8b4"); // Bishop attacks f1
    CHECK(game.makeMove("e1g1") == MoveResult::INVALID);
}