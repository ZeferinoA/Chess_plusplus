#include "Board.h"
#include <iostream>

int main() {
    Board board;
    int turn = 0;

    do {
        board.display();
        
        std::cout << "Player " << turn + 1 << ": ";
        board.receiveInputs();
        
        turn = (turn == 0) ? 1 : 0;
    } while (true);

    return 0;
}