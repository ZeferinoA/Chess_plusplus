#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <memory>

class ChessEngine;

class Menu {
private:
    std::unique_ptr<ChessEngine> engine;
    std::string currentFen;
    std::string lastMove;
    bool gameActive;
    
    void displayHeader();
    void displayMainMenu();
    void displayGameMenu();
    void displayBoard(const std::string& fen);
    std::string getStockfishPath();
    
    void newGame();
    void playAgainstEngine();
    void analyzePosition();
    void inputMove();
    void setCustomPosition();
    void viewEngineInfo();
    
public:
    Menu();
    ~Menu();
    
    void run();
};

#endif // MENU_H