TEST_CASE("Invalid moves", [invalid]") {
    ChessGame game;
    
    // Moving through pieces
    CHECK(game.makeMove("e4") == MoveResult::SUCCESS);
    CHECK(game.makeMove("Qh5") == MoveResult::INVALID); // Queen can't jump
    
    // Moving opponent's piece
    CHECK(game.makeMove("e5") == MoveResult::SUCCESS);
    CHECK(game.makeMove("e6") == MoveResult::INVALID); // Not black's turn
    
    // Moving to square occupied by own piece
    CHECK(game.makeMove("Nf3") == MoveResult::SUCCESS);
    CHECK(game.makeMove("Ng1") == MoveResult::INVALID); // Square occupied
    
    // Invalid knight move
    CHECK(game.makeMove("Nf6") == MoveResult::INVALID); // Not L-shaped
}