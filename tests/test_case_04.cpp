#include "../catch2/catch_amalgamated.hpp"

TEST_CASE("Castling", "[special]") {
    ChessGame game;
    
    // Clear pieces between king and rook
    game.makeMove("Nf3");
    game.makeMove("Nc6");
    game.makeMove("g3");
    game.makeMove("b6");
    game.makeMove("Bg2");
    game.makeMove("Bb7");
    
    // Kingside castling
    CHECK(game.makeMove("O-O") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("g1") == Piece::WHITE_KING);
    CHECK(game.getPieceAt("f1") == Piece::WHITE_ROOK);
    
    // Queenside castling for black
    game.makeMove("O-O-O");
    CHECK(game.getPieceAt("c8") == Piece::BLACK_KING);
    CHECK(game.getPieceAt("d8") == Piece::BLACK_ROOK);
}

TEST_CASE("En passant", "[special]") {
    ChessGame game;
    
    game.makeMove("e4");
    game.makeMove("a6");
    game.makeMove("e5");
    game.makeMove("d5"); // Black pawn moves two squares
    
    // White captures en passant
    CHECK(game.makeMove("exd6") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("d6") == Piece::WHITE_PAWN);
    CHECK(game.getPieceAt("d5") == Piece::EMPTY);
}

TEST_CASE("Pawn promotion", "[special]") {
    ChessGame game;
    
    // Move pawn to 7th rank
    game.makeMove("a4");
    game.makeMove("b5");
    game.makeMove("axb5");
    game.makeMove("a6");
    game.makeMove("b6");
    game.makeMove("a5");
    game.makeMove("b7");
    
    // Promote to queen
    CHECK(game.makeMove("b8=Q") == MoveResult::SUCCESS);
    CHECK(game.getPieceAt("b8") == Piece::WHITE_QUEEN);
}