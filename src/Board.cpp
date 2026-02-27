#include "Board.hpp"
#include <iostream>
#include <string>

Board::Board() {
    std::string initialBoard[ROWS][COLS] = {
        {"   ", " a ", " b ", " c ", " d ", " e ", " f ", " g ", " h "},
        {" 1 ", "[R]", "[K]", "[B]", "[Q]", "[K]", "[B]", "[K]", "[R]"},
        {" 2 ", "[P]", "[P]", "[P]", "[P]", "[P]", "[P]", "[P]", "[P]"},
        {" 3 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 4 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 5 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 6 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 7 ", "[p]", "[p]", "[p]", "[p]", "[p]", "[p]", "[p]", "[p]"},
        {" 8 ", "[r]", "[k]", "[b]", "[k]", "[q]", "[b]", "[k]", "[r]"}
    };

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = initialBoard[i][j];
        }
    }
}

void Board::display() const {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            std::cout << grid[i][j];
        }
        std::cout << std::endl;
    }
}

void Board::movePiece(int s_col, int s_row, int m_col, int m_row) {
    std::string temp = grid[m_row][m_col];
    grid[m_row][m_col] = grid[s_row][s_col];
    grid[s_row][s_col] = temp;
}

void Board::selectPiece(int col, int row) {
    std::cout << "Move to (e.g., e4): ";
    std::string move;
    std::getline(std::cin, move);

    if (move.length() != 2) {
        std::cout << "Invalid move format.\n";
        return;
    }

    int m_col = move[0] - 'a' + 1;
    int m_row = move[1] - '1' + 1;

    if (m_col < 1 || m_col > 8 || m_row < 1 || m_row > 8) {
        std::cout << "Move out of bounds!\n";
        return;
    }

    movePiece(col, row, m_col, m_row);
}

void Board::receiveInputs() {
    std::cout << "Select piece to move (e.g., e2): ";
    std::string select;
    std::getline(std::cin, select);

    if (select.length() != 2) {
        std::cout << "Invalid input format.\n";
        return;
    }

    int col = select[0] - 'a' + 1;
    int row = select[1] - '1' + 1;

    if (col < 1 || col > 8 || row < 1 || row > 8) {
        std::cout << "Selection out of bounds!\n";
        return;
    }

    selectPiece(col, row);
}

void Board::clearBoard() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0) {
                // Column headers
                if (j == 0) grid[i][j] = "   ";
                else grid[i][j] = " " + std::string(1, 'a' + j - 1) + " ";
            } else if (j == 0) {
                // Row headers
                grid[i][j] = " " + std::to_string(i) + " ";
            } else {
                // Empty space
                grid[i][j] = "[ ]";
            }
        }
    }
}

void Board::setPiece(int col, int row, const std::string &piece) {
    if (col >= 1 && col <= 8 && row >= 1 && row <= 8) {
        grid[row][col] = piece;
    }
}