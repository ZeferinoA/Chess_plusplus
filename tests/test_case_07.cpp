#include "../catch2/catch_amalgamated.hpp"

TEST_CASE("Move disambiguation", "[notation]") {
    ChessGame game;
    
    // Move both knights to same square
    game.makeMove("Nc3");
    game.makeMove("d5");
    game.makeMove("Nd2"); // Second knight
    
    // Need to specify which knight moves to e4
    CHECK(game.makeMove("Nce4") == MoveResult::SUCCESS);
    
    // File disambiguation
    game.makeMove("Nf6");
    CHECK(game.makeMove("Rfe1") == MoveResult::SUCCESS);
}

TEST_CASE("Move disambiguation", "[notation]") {
    ChessGame game;
    
    // Move both knights to same square
    game.makeMove("Nc3");
    game.makeMove("d5");
    game.makeMove("Nd2"); // Second knight
    
    // Need to specify which knight moves to e4
    CHECK(game.makeMove("Nce4") == MoveResult::SUCCESS);
    
    // File disambiguation
    game.makeMove("Nf6");
    CHECK(game.makeMove("Rfe1") == MoveResult::SUCCESS);
}

TEST_CASE("Edge cases", "[edge]") {
    ChessGame game;
    
    // Invalid notation
    CHECK(game.makeMove("invalid") == MoveResult::INVALID_NOTATION);
    CHECK(game.makeMove("") == MoveResult::INVALID_NOTATION);
    
    // Moving from empty square (using Kx notation)
    CHECK(game.makeMove("Kxe5") == MoveResult::INVALID);
    
    // Moving king into check
    game.makeMove("e4");
    game.makeMove("e5");
    game.makeMove("Ke2"); // King moving through attacked squares
    CHECK(game.makeMove("Ke2") == MoveResult::INVALID); // Would be in check
    
    // Attempting castling through check
    game.makeMove("Nf3");
    game.makeMove("Bb4"); // Bishop attacks f1
    CHECK(game.makeMove("O-O") == MoveResult::INVALID);
}