#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void move_piece(int s_col, int s_row, int m_col, int m_row, std::string board[][9]){
    
    std::string temp_space = board[m_row][m_col];
    board[m_row][m_col] = board[s_row][s_col];
    board[s_row][s_col] = temp_space;
}

void select_piece(int col, int row, std::string board[][9]){
    std::cout << col << " " << row << std::endl;
    
    std::string move;
    std::getline(std::cin, move);

    int m_col = move[0] - 'a' + 1;
    int m_row = move[1] - '1' + 1;

    move_piece(col, row, m_col, m_row, board);
}

void recieve_inputs(std::string board[][9]){
    std::string select;
    std::getline(std::cin, select);

    int col = select[0] - 'a' + 1;
    int row = select[1] - '1' + 1;

    select_piece(col, row, board);
}

int main(){
    std::string board[9][9] = {
        {"   ", " a ", " b ", " c ", " d ", " e ", " f ", " g ", " h "},
        {" 1 ", "[R]", "[K]", "[B]", "[Q]", "[K]", "[B]", "[K]", "[R]"},
        {" 2 ", "[P]", "[P]", "[P]", "[P]", "[P]", "[P]", "[P]", "[P]"},
        {" 3 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 4 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 5 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 6 ", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]", "[ ]"},
        {" 7 ", "[p]", "[p]", "[p]", "[p]", "[p]", "[p]", "[p]", "[p]",},
        {" 8 ", "[r]", "[k]", "[b]", "[k]", "[q]", "[b]", "[k]", "[r]"}
    }; 

    int turn = 0;

    do{
        for(int i = 0; i < 9; i++){
            for(int j = 0; j < 8; j++){
                std::cout << board[i][j];
            }
            std::cout << std::endl;
        }

        std::cout << "player " << turn + 1<< ": ";
        recieve_inputs(board);
        if(turn == 0){
            turn++;
        }else{
            turn--;
        }
    }while(true);
}
