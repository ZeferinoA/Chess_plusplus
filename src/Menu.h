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
    
    // Difficulty settings - define enum before using it
public:
    enum Difficulty {
        BEGINNER = 0,
        EASY = 5,
        MEDIUM = 10,
        HARD = 15,
        EXPERT = 20
    };

private:
    Difficulty currentDifficulty;
    int engineMoveTimeMs; // Time in milliseconds for engine to think
    
    // Lesson system
    struct Lesson {
        std::string title;
        std::string description;
        std::string startFen;
        std::string expectedMove;
        std::string hint;
        bool isAdvanced;
    };
    
    struct FenLesson {
        std::string title;
        std::string description;
        std::string question;
        std::string expectedAnswer;
        std::string hint;
        std::string exampleFen;
    };
    
    std::vector<Lesson> beginnerLessons;
    std::vector<Lesson> advancedLessons;
    std::vector<FenLesson> fenLessons;
    int currentLessonIndex;
    
    void displayHeader();
    void displayMainMenu();
    void displayGameMenu();
    void displayVsEngineMenu();
    void displayLessonMenu();
    void displayFenLessonMenu();
    void displayDifficultyMenu();
    void displayBoard(const std::string& fen);
    std::string getStockfishPath();
    
    void newGame();
    void playVsEngine(); // Renamed from playAgainstEngine
    void playLocalPvp(); // New local PvP mode without engine
    void analyzePosition();
    void inputMove();
    void setCustomPosition();
    void viewEngineInfo();
    void setDifficulty();
    
    // Methods for lessons and custom pieces
    void initializeLessons();
    void initializeFenLessons();
    void showLessonMenu();
    void startBeginnerLessons();
    void startAdvancedLessons();
    void startFenLessons();
    void runLesson(const Lesson& lesson);
    void runFenLesson(const FenLesson& lesson);
    bool checkLessonMove(const std::string& move, const std::string& expected);
    bool checkFenAnswer(const std::string& answer, const std::string& expected);
    
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