#include "Tutorial.hpp"
#include <limits>
#include <cmath>
#include <iostream>

// Safely get 2-char input like e2
std::string getPlayerInput() {
    std::string input;
    std::getline(std::cin, input);
    if (input.length() != 2) {
        std::cout << "Invalid input. Use format like e2\n";
        return getPlayerInput();
    }
    return input;
}

// Piece-specific move validators
bool isValidKingMove(int s_col, int s_row, int m_col, int m_row) {
    return (std::abs(m_col - s_col) <= 1 && std::abs(m_row - s_row) <= 1);
}

bool isValidRookMove(int s_col, int s_row, int m_col, int m_row) {
    return (s_col == m_col || s_row == m_row);
}

bool isValidBishopMove(int s_col, int s_row, int m_col, int m_row) {
    return (std::abs(m_col - s_col) == std::abs(m_row - s_row));
}

bool isValidQueenMove(int s_col, int s_row, int m_col, int m_row) {
    return isValidRookMove(s_col, s_row, m_col, m_row) || isValidBishopMove(s_col, s_row, m_col, m_row);
}

bool isValidKnightMove(int s_col, int s_row, int m_col, int m_row) {
    int dx = std::abs(m_col - s_col);
    int dy = std::abs(m_row - s_row);
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

bool isValidPawnMove(int s_col, int s_row, int m_col, int m_row, bool isWhite=true) {
    int dir = isWhite ? 1 : -1;
    return (s_col == m_col && m_row - s_row == dir) || // forward move
           (std::abs(m_col - s_col) == 1 && m_row - s_row == dir); // capture diagonally
}

// Special move validators
bool isValidCastlingMove(int s_col, int s_row, int m_col, int m_row, bool kingside=true) {
    if (s_col != 5 || s_row != 1) return false; // King starting at e1
    if (kingside) return (m_col == 7 && m_row == 1);  // King moves two squares to g1
    else return (m_col == 3 && m_row == 1);           // King moves two squares to c1
}

bool isValidEnPassant(int s_col, int s_row, int m_col, int m_row, int target_col, int target_row) {
    return (std::abs(m_col - s_col) == 1 && m_row - s_row == 1 &&
            m_col == target_col && m_row == target_row);
}

// Generalized tutorial step for any piece
void tutorialStep(Board &board, const std::string &piece, int s_col, int s_row,
                  int t_col, int t_row, bool (*validMoveFunc)(int,int,int,int)) {

    board.setPiece(s_col, s_row, piece);   // place tutorial piece
    board.setPiece(t_col, t_row, "[p]");   // place target
    board.display();

    bool captured = false;
    while (!captured) {
        std::cout << "Move the " << piece << " to capture the enemy piece: ";
        std::string move = getPlayerInput();
        int m_col = move[0] - 'a' + 1;
        int m_row = move[1] - '1' + 1;

        if (validMoveFunc(s_col, s_row, m_col, m_row)) {
            board.movePiece(s_col, s_row, m_col, m_row);
            
            // Check if piece reached target
            if (m_col == t_col && m_row == t_row) {
                captured = true;
                std::cout << "Well done! You captured the piece!\n";
            } else {
                std::cout << "Valid move, but you haven't reached the target yet. Keep Trying!\n";
            }
        } else {
            std::cout << "Invalid move. Try again.\n";
        }

        board.display();
        s_col = m_col;
        s_row = m_row; // update piece position in case of multiple moves
    }
}

// Tutorial with special moves included
void tutorial_start() {
    Board board;
    board.clearBoard();

    std::cout << "Welcome to the tutorial! Let's begin.\n";
    std::cout << "In Chess, you're given an 8x8 grid.\n"
                 "Your objective is to capture your opponent's king.\n"
                 "We'll introduce each piece step by step.\n\n"
                 "Hit Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  
    std::cin.get();

    // Step 1: King
    std::cout << "\nStep 1: The King can move 1 square in any direction.\n";
    tutorialStep(board, "[K]", 4, 1, 4, 2, isValidKingMove);
    board.clearBoard();

    // Step 2: Rook
    std::cout << "\nStep 2: The Rook moves in straight lines, horizontally or vertically.\n";
    tutorialStep(board, "[R]", 1, 1, 1, 4, isValidRookMove);
    board.clearBoard();

    // Step 3: Knight
    std::cout << "\nStep 3: The Knight moves in an L-shape: 2 squares one way, 1 square perpendicular.\n";
    tutorialStep(board, "[N]", 2, 1, 3, 3, isValidKnightMove);
    board.clearBoard();

    // Step 4: Bishop
    std::cout << "\nStep 4: The Bishop moves diagonally any number of squares.\n";
    tutorialStep(board, "[B]", 3, 1, 6, 4, isValidBishopMove);
    board.clearBoard();

    // Step 5: Queen
    std::cout << "\nStep 5: The Queen moves horizontally, vertically, or diagonally.\n";
    tutorialStep(board, "[Q]", 4, 1, 4, 4, isValidQueenMove);
    board.clearBoard();

    // Step 6: Castling
    std::cout << "\nStep 6: Castling allows the King to move 2 squares and Rook jumps over it.\n";
    board.setPiece(5,1,"[K]"); // e1
    board.setPiece(8,1,"[R]"); // h1
    board.display();
    std::cout << "Move King from e1 to g1 to castle: ";
    std::string castleMove = getPlayerInput();
    int k_col = castleMove[0]-'a'+1, k_row = castleMove[1]-'1'+1;
    if(isValidCastlingMove(5,1,k_col,k_row)) {
        board.setPiece(7,1,"[K]");
        board.setPiece(6,1,"[R]");
        board.setPiece(5,1,"[ ]");
        board.setPiece(8,1,"[ ]");
        std::cout << "Castling completed!\n";
        board.display();
    }

    // Step 7: Pawn promotion
    std::cout << "\nStep 7: Pawn Promotion. Move your pawn to the last rank to promote.\n";
    board.clearBoard();
    board.setPiece(2,7,"[P]"); // Pawn at b7
    board.display();
    std::cout << "Move pawn to b8: ";
    std::string promoMove = getPlayerInput();
    if(promoMove=="b8") {
        std::string choice;
        std::cout << "Promote pawn to (Q,R,B,N): ";
        std::cin >> choice;
        board.setPiece(2,8,"["+choice+"]");
        board.setPiece(2,7,"[ ]");
        std::cout << "Pawn promoted!\n";
        board.display();
    }

    // Step 8: En Passant
    std::cout << "\nStep 8: En Passant. A pawn can capture an enemy pawn that just moved 2 squares from its starting position.\n";
    board.clearBoard();

    // Setup pawns for en passant
    board.setPiece(5,5,"[P]");   // Your pawn at e5
    board.setPiece(6,7,"[p]");   // Enemy pawn at f7
    board.display();

    std::cout << "Hit Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  
    std::cin.get();

    std::cout << "Enemy pawn moves from f7 to f5 (2 squares forward)\n";
    board.movePiece(6,7,6,5); // Move enemy pawn f7->f5
    board.display();

    std::cout << "Now, capture it en passant with your pawn at e5 (move to f6): ";
    bool finished = false;
    while(finished == false){    
        std::string epMove = getPlayerInput();
        int ep_col = epMove[0]-'a'+1;
        int ep_row = epMove[1]-'1'+1;
        if(isValidEnPassant(5,5,ep_col,ep_row,6,6)) {
            board.setPiece(ep_col,ep_row,"[P]"); // Move your pawn
            board.setPiece(5,5,"[ ]");           // Remove your pawn from original
            board.setPiece(6,5,"[ ]");           // Remove captured pawn
            std::cout << "En Passant captured!\n";
            board.display();
            finished = true;
        }else {
            std::cout << "Invalid En Passant\n";
        }
    }
}
