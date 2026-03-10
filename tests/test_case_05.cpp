#include "../catch2/catch_amalgamated.hpp"
#include "ChessTestWrapper.h"

TEST_CASE("Check detection", "[check]") {
    ChessGame game;
    
    game.makeMove("e2e4");
    game.makeMove("e7e5");
    game.makeMove("d1h5");
    game.makeMove("g8f6"); // Black defends with Knight
    
    // Queen delivers check on e5 (but it's not mate, Black can block)
    CHECK(game.makeMove("h5e5") == MoveResult::CHECK);
    CHECK(game.getPieceAt("e5") == Piece::WHITE_QUEEN);
    CHECK(game.isInCheck(Color::BLACK));
}

TEST_CASE("Checkmate detection", "[checkmate]") {
    ChessGame game;
    
    game.makeMove("f2f3");
    game.makeMove("e7e5");
    game.makeMove("g2g4");
    
    auto result = game.makeMove("d8h4");
    CHECK(result == MoveResult::CHECKMATE);
    CHECK(game.isGameOver());
    CHECK(game.getWinner() == Color::BLACK);
}