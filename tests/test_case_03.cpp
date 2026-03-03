#include "../catch2/catch_amalgamated.hpp"

TEST_CASE("Piece captures", "[capture]") {
    ChessGame game;
    
    // Setup capture scenario
    game.makeMove("e4");
    game.makeMove("d5");
    
    // Pawn capture
    CHECK(game.makeMove("exd5") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("d5") == Piece::WHITE_PAWN);
    CHECK(game.getPieceAt("e4") == Piece::EMPTY);
    
    // Knight capture
    game.makeMove("Nf6");
    game.makeMove("Nxf6"); // Needs disambiguation if multiple knights
    CHECK(game.getPieceAt("f6") == Piece::WHITE_KNIGHT);
}