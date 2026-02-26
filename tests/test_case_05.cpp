TEST_CASE("Check detection", "[check]") {
    ChessGame game;
    
    // Scholar's mate attempt
    game.makeMove("e4");
    game.makeMove("e5");
    game.makeMove("Qh5");
    game.makeMove("Nc6");
    game.makeMove("Bc4");
    game.makeMove("Nf6");
    
    // Queen delivers check
    CHECK(game.makeMove("Qxf7") == MoveResult::CHECK);
    CHECK(game.getPieceAt("f7") == Piece::WHITE_QUEEN);
    CHECK(game.isInCheck(Color::BLACK));
}

TEST_CASE("Checkmate detection", "[checkmate]") {
    ChessGame game;
    
    // Fool's mate
    game.makeMove("f3");
    game.makeMove("e5");
    game.makeMove("g4");
    
    // This should be checkmate
    auto result = game.makeMove("Qh4");
    CHECK(result == MoveResult::CHECKMATE);
    CHECK(game.isGameOver());
    CHECK(game.getWinner() == Color::BLACK);
}