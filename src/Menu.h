#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <memory>
#include <map>

class ChessEngine;

// Custom piece structure
struct CustomPiece {
    std::string name;
    char symbol;           // Character to display on board
    std::vector<std::string> movePatterns; // e.g., "king", "rook", "knight", etc.
    bool isEnabled;
};

class Menu {
private:
    std::unique_ptr<ChessEngine> engine;
    std::string currentFen;
    std::string lastMove;
    bool gameActive;
    bool customPiecesEnabled;
    std::map<char, CustomPiece> customPieces; // Map symbol -> piece definition
    
    // Lesson system
    struct Lesson {
        std::string title;
        std::string description;
        std::string startFen;
        std::string expectedMove;
        std::string hint;
        bool isAdvanced;
    };
    
    std::vector<Lesson> beginnerLessons;
    std::vector<Lesson> advancedLessons;
    int currentLessonIndex;
    
    void displayHeader();
    void displayMainMenu();
    void displayGameMenu();
    void displayLessonMenu();
    void displayBoard(const std::string& fen);
    std::string getStockfishPath();
    
    void newGame();
    void playAgainstEngine();
    void analyzePosition();
    void inputMove();
    void setCustomPosition();
    void viewEngineInfo();
    
    // New methods for lessons and custom pieces
    void initializeLessons();
    void showLessonMenu();
    void startBeginnerLessons();
    void startAdvancedLessons();
    void runLesson(const Lesson& lesson);
    bool checkLessonMove(const std::string& move, const std::string& expected);
    
    // Custom piece methods
    void showCustomPieceMenu();
    void createCustomPiece();
    void enableCustomPieces();
    void playLocalGame(); // Local game without engine for custom pieces
    
public:
    Menu();
    ~Menu();
    
    void run();
};

#endif // MENU_H