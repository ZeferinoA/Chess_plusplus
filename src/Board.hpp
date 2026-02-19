#pragma once
#include <string>

class Board {
private:
    static const int ROWS = 9;
    static const int COLS = 9;
    std::string grid[ROWS][COLS];
    
    void movePiece(int s_col, int s_row, int m_col, int m_row);
    void selectPiece(int col, int row);
    
public:
    Board();
    void display() const;
    void receiveInputs();
};