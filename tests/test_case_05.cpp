#include "../catch2/catch_amalgamated.hpp"
#include "ChessTestWrapper.h"

TEST_CASE("Check detection", "[check]") {
    ChessGame game;
    
    // Scholar's mate attempt
    game.makeMove("e2e4");
    game.makeMove("e7e5");
    game.makeMove("d1h5");
    game.makeMove("b8c6");
    game.makeMove("f1c4");
    game.makeMove("g8f6");
    
    // Queen delivers check
    CHECK(game.makeMove("h5f7") == MoveResult::CHECK);
    CHECK(game.getPieceAt("f7") == Piece::WHITE_QUEEN);
    CHECK(game.isInCheck(Color::BLACK));
}

TEST_CASE("Checkmate detection", "[checkmate]") {
    ChessGame game;
    
    // Fool's mate
    game.makeMove("f2f3");
    game.makeMove("e7e5");
    game.makeMove("g2g4");
    
    // This should be checkmate
    auto result = game.makeMove("d8h4");
    CHECK(result == MoveResult::CHECKMATE);
    CHECK(game.isGameOver());
    CHECK(game.getWinner() == Color::BLACK);
}