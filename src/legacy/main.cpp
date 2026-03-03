#include "Board.hpp"
#include "Tutorial.hpp"
#include <iostream>

int main() {
    int game_mode;
    std::cout << "Choose Mode " << std::endl;
    std::cout << "1. Tutorial \n2. Regular game \n3. Crazy mode" << std::endl;
    std::cin >> game_mode;
    if(game_mode == 1){
        tutorial_start();
    }else if(game_mode == 2){

    }else{

    }

    return 0;
}
