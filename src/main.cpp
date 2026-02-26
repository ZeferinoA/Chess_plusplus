#include "Board.hpp"
#include "Tutorial.hpp"
#include <iostream>

int main() {
    int game_mode;
    std::cout << "Choose Mode " << std::endl;
    std::cout << "1. Tutorial \n2. Regular game \n3. Crazy mode" << std::endl;
    std::cin >> game_mode;
    if(game_mode == 1){
        //call the tutorial file fxns
    }else if(game_mode == 2){

    }else{

    }
    
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