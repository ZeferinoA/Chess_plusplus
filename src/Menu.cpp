#include "Menu.h"
#include "ChessEngine.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <memory>
#include <map>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Initial position FEN
const std::string INITIAL_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Simple ASCII board representation
const std::string PIECE_SYMBOLS = " PNBRQK  pnbrqk";

// C++11 compatible make_unique implementation
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

Menu::Menu() : currentFen(INITIAL_FEN), gameActive(false), customPiecesEnabled(false), currentLessonIndex(0) {
    initializeLessons();
}

Menu::~Menu() = default;

void Menu::initializeLessons() {
    // Beginner lessons
    beginnerLessons = {
        {
            "Pawn Movement",
            "Pawns move forward one square, but capture diagonally. Move the white pawn from e2 to e4.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "e2e4",
            "Pawns can move two squares on their first move. Try e2 to e4.",
            false
        },
        {
            "Knight Movement",
            "Knights move in an L-shape: 2 squares in one direction, 1 square perpendicular. Move the white knight from g1 to f3.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "g1f3",
            "Knights are the only pieces that can jump over others. Try moving the knight from g1 to f3.",
            false
        },
        {
            "Bishop Movement",
            "Bishops move diagonally. Move the white bishop from f1 to c4.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "f1c4",
            "Bishops stay on the same color squares. The f1 bishop starts on a light square.",
            false
        },
        {
            "Rook Movement",
            "Rooks move horizontally and vertically. Move the white rook from a1 to a3.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "a1a3",
            "Rooks move in straight lines. Try moving the a1 rook forward.",
            false
        },
        {
            "Queen Movement",
            "Queens move like rooks and bishops combined. Move the white queen from d1 to d4.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "d1d4",
            "The queen is the most powerful piece. Try moving it forward.",
            false
        },
        {
            "King Movement",
            "Kings move one square in any direction. Move the white king from e1 to e2.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "e1e2",
            "The king moves slowly but is vital to protect. Try moving it forward one square.",
            false
        }
    };
    
    // Advanced lessons
    advancedLessons = {
        {
            "Italian Opening",
            "The Italian Game begins with 1.e4 e5 2.Nf3 Nc6 3.Bc4. Make the third move for White.",
            "r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 3",
            "Bc4",
            "Develop your bishop to c4, targeting Black's f7 pawn.",
            true
        },
        {
            "Spanish Opening (Ruy Lopez)",
            "The Ruy Lopez begins with 1.e4 e5 2.Nf3 Nc6 3.Bb5. Make the third move for White.",
            "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 0 3",
            "Bb5",
            "Pin the knight to the king by moving your bishop to b5.",
            true
        },
        {
            "Queen's Gambit",
            "The Queen's Gambit starts with 1.d4 d5 2.c4. Make the second move for White.",
            "rnbqkbnr/ppp1pppp/8/3p4/3P4/8/PPP1PPPP/RNBQKBNR b KQkq - 0 2",
            "c4",
            "Offer a pawn with c4 to gain center control.",
            true
        },
        {
            "Sicilian Defense",
            "The Sicilian Defense is 1.e4 c5. Make Black's first move.",
            "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2",
            "c5",
            "Black immediately challenges White's center from the c-file.",
            true
        },
        {
            "Caro-Kann Defense",
            "The Caro-Kann is 1.e4 c6 2.d4 d5. Make Black's second move.",
            "rnbqkbnr/pp2pppp/2p5/3p4/3PP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 3",
            "d5",
            "Black challenges the center while maintaining a solid pawn structure.",
            true
        },
        {
            "King's Indian Defense",
            "The King's Indian begins with 1.d4 Nf6 2.c4 g6. Make Black's second move.",
            "rnbqkb1r/pppppp1p/5np1/8/2PP4/8/PP2PPPP/RNBQKBNR w KQkq - 0 3",
            "g6",
            "Black prepares to fianchetto the king's bishop.",
            true
        }
    };
}

void Menu::displayHeader() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "        TERMINAL CHESS ENGINE - STOCKFISH" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    if (customPiecesEnabled) {
        std::cout << "     *** CUSTOM PIECES ENABLED ***" << std::endl;
    }
}

void Menu::displayBoard(const std::string& fen) {
    // Parse FEN
    std::string boardPart = fen.substr(0, fen.find(' '));
    
    std::cout << "\n       a       b       c       d       e       f       g       h" << std::endl;
    std::cout << "   " << std::string(65, '-') << std::endl;
    
    int row = 8;
    size_t pos = 0;
    
    for (int r = 0; r < 8; r++) {
        std::cout << " " << row << " |";
        
        for (int c = 0; c < 8; c++) {
            if (pos >= boardPart.length()) break;
            
            char ch = boardPart[pos];
            if (isdigit(ch)) {
                int empty = ch - '0';
                for (int i = 0; i < empty; i++) {
                    // Empty square: 7 characters total (spaces + dot + spaces)
                    std::cout << "   .    ";
                }
                c += empty - 1;
                pos++;
            } else if (ch == '/') {
                pos++;
                c--;
            } else {
                // Check if this is a custom piece
                char displayChar = ch;
                if (customPiecesEnabled && customPieces.find(ch) != customPieces.end()) {
                    // Use custom piece symbol
                    displayChar = customPieces[ch].symbol;
                } else {
                    // Find standard piece symbol
                    int pieceIndex = 0;
                    for (int i = 0; i < 14; i++) {
                        if (PIECE_SYMBOLS[i] == ch) {
                            pieceIndex = i;
                            break;
                        }
                    }
                    displayChar = " PNBRQK  pnbrqk"[pieceIndex];
                    if (displayChar == ' ') displayChar = ch;
                }
                
                // Color pieces (uppercase for white, lowercase for black)
                if (isupper(ch)) {
                    // White pieces - exactly 7 characters total
                    std::cout << "   " << displayChar << "    ";
                } else {
                    // Black pieces - exactly 7 characters total
                    std::cout << "   " << (char)tolower(displayChar) << "    ";
                }
                
                pos++;
            }
        }
        
        std::cout << "|" << std::endl;
        std::cout << "   " << std::string(65, '-') << std::endl;
        row--;
    }
    
    std::cout << "       a       b       c       d       e       f       g       h" << std::endl;
    
    // Show game state
    size_t spacePos = fen.find(' ');
    if (spacePos != std::string::npos) {
        std::string turn = fen.substr(spacePos + 1, 1);
        std::cout << "\nTurn: " << (turn == "w" ? "White" : "Black") << std::endl;
    }
    
    if (!lastMove.empty()) {
        std::cout << "Last move: " << lastMove << std::endl;
    }
}

void Menu::displayMainMenu() {
    std::cout << "\nMAIN MENU:" << std::endl;
    std::cout << "1. New Game (Play vs Engine)" << std::endl;
    std::cout << "2. Set Custom Position" << std::endl;
    std::cout << "3. Analyze Position" << std::endl;
    std::cout << "4. Engine Info" << std::endl;
    std::cout << "5. Lessons" << std::endl;
    std::cout << "6. Custom Pieces" << std::endl;
    std::cout << "7. Local Game (with Custom Pieces)" << std::endl;
    std::cout << "8. Exit" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::displayGameMenu() {
    std::cout << "\nGAME MENU:" << std::endl;
    std::cout << "1. Make Move" << std::endl;
    std::cout << "2. Get Engine Move" << std::endl;
    std::cout << "3. Analyze Current Position" << std::endl;
    std::cout << "4. Show Legal Moves" << std::endl;
    std::cout << "5. New Game" << std::endl;
    std::cout << "6. Back to Main Menu" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::displayLessonMenu() {
    std::cout << "\nLESSON MENU:" << std::endl;
    std::cout << "1. Beginner Lessons" << std::endl;
    std::cout << "2. Advanced Lessons (Openings & Book Moves)" << std::endl;
    std::cout << "3. Back to Main Menu" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::newGame() {
    currentFen = INITIAL_FEN;
    lastMove = "";
    gameActive = true;
    
    std::cout << "\nStarting new game..." << std::endl;
    displayBoard(currentFen);
    playAgainstEngine();
}

std::string Menu::getStockfishPath() {
#ifdef _WIN32
    return "stockfish\\stockfish.exe";
#else
    return "./stockfish/stockfish";
#endif
}

void Menu::playAgainstEngine() {
    if (!engine) {
        std::string path = getStockfishPath();
        
        std::cout << "Starting engine from: " << path << "..." << std::endl;
        
        // Use explicit unique_ptr construction
        engine = std::unique_ptr<ChessEngine>(new ChessEngine(path));
        
        if (!engine->start()) {
            std::cout << "Failed to start Stockfish at: " << path << std::endl;
            std::cout << "Please make sure Stockfish is installed in the 'stockfish' directory." << std::endl;
            engine.reset();
            gameActive = false;
            return;
        }
        std::cout << "Engine started successfully!" << std::endl;
    }
    
    while (gameActive) {
        displayBoard(currentFen);
        displayGameMenu();
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                inputMove();
                break;
            case 2: {
                std::cout << "\nCalculating best move..." << std::endl;
                std::string bestMove = engine->getBestMove(currentFen, 2000);
                if (!bestMove.empty()) {
                    std::cout << "Engine suggests: " << bestMove << std::endl;
                    std::cout << "Apply this move? (y/n): ";
                    char apply;
                    std::cin >> apply;
                    std::cin.ignore();
                    if (apply == 'y' || apply == 'Y') {
                        // Validate and apply the engine's move
                        if (engine->isValidMove(currentFen, bestMove)) {
                            std::string newFen = engine->makeMove(currentFen, bestMove);
                            if (newFen != currentFen) {
                                lastMove = bestMove;
                                currentFen = newFen;
                                std::cout << "Engine move applied!" << std::endl;
                            } else {
                                std::cout << "Failed to apply engine move." << std::endl;
                            }
                        } else {
                            std::cout << "Invalid move suggested by engine?" << std::endl;
                        }
                    }
                } else {
                    std::cout << "Could not get move from engine" << std::endl;
                }
                break;
            }
            case 3:
                analyzePosition();
                break;
            case 4: {
                auto moves = engine->getLegalMoves(currentFen);
                std::cout << "\nLegal moves (" << moves.size() << "): ";
                for (const auto& move : moves) {
                    std::cout << move << " ";
                }
                std::cout << std::endl;
                break;
            }
            case 5:
                newGame();
                break;
            case 6:
                gameActive = false;
                break;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void Menu::inputMove() {
    std::cout << "\nEnter your move (e.g., e2e4) or 'q' to cancel: ";
    std::string move;
    std::getline(std::cin, move);
    
    if (move == "q" || move == "Q") {
        std::cout << "Move cancelled." << std::endl;
        return;
    }
    
    // First, let's see what the engine returns for legal moves
    std::cout << "Requesting legal moves from engine..." << std::endl;
    auto legalMoves = engine->getLegalMoves(currentFen);
    
    std::cout << "Engine returned " << legalMoves.size() << " legal moves." << std::endl;
    
    if (legalMoves.empty()) {
        std::cout << "No legal moves found. This could mean:" << std::endl;
        std::cout << "1. Game is over" << std::endl;
        std::cout << "2. Engine communication issue" << std::endl;
        std::cout << "3. Position is invalid" << std::endl;
        
        // Let's try a direct command to see if engine is responding
        std::cout << "\nTesting engine response..." << std::endl;
        std::string response = engine->sendCommand("uci");
        if (response.find("Stockfish") != std::string::npos) {
            std::cout << "Engine is responding normally." << std::endl;
        } else {
            std::cout << "Engine may not be responding correctly." << std::endl;
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
        return;
    }
    
    // Show legal moves
    std::cout << "\nLegal moves: ";
    for (const auto& m : legalMoves) {
        std::cout << m << " ";
    }
    std::cout << std::endl;
    
    // Check if user's move is legal
    bool found = false;
    std::string matchedMove;
    
    for (const auto& legalMove : legalMoves) {
        if (legalMove == move) {
            found = true;
            matchedMove = legalMove;
            break;
        }
    }
    
    if (!found) {
        std::cout << "Move '" << move << "' is not in the legal moves list." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
        return;
    }
    
    // Apply the move
    std::cout << "Applying move: " << matchedMove << std::endl;
    std::string newFen = engine->makeMove(currentFen, matchedMove);
    
    if (newFen != currentFen) {
        lastMove = matchedMove;
        currentFen = newFen;
        std::cout << "Move applied successfully!" << std::endl;
    } else {
        std::cout << "Failed to apply move." << std::endl;
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

void Menu::analyzePosition() {
    if (!engine) {
        std::string path = getStockfishPath();
        
        std::cout << "Starting engine from: " << path << "..." << std::endl;
        
        // Use explicit unique_ptr construction
        engine = std::unique_ptr<ChessEngine>(new ChessEngine(path));
        
        if (!engine->start()) {
            std::cout << "Failed to start Stockfish at: " << path << std::endl;
            std::cout << "Please make sure Stockfish is installed in the 'stockfish' directory." << std::endl;
            engine.reset();
            return;
        }
    }
    
    std::cout << "\nAnalyzing position (depth 15)..." << std::endl;
    std::string analysis = engine->analyzePosition(currentFen, 15);
    
    // Extract relevant info from analysis
    size_t infoPos = analysis.find("info");
    while (infoPos != std::string::npos) {
        size_t endLine = analysis.find('\n', infoPos);
        std::string line = analysis.substr(infoPos, endLine - infoPos);
        
        // Look for score
        size_t scorePos = line.find("score cp");
        if (scorePos != std::string::npos) {
            size_t valuePos = scorePos + 9;
            size_t valueEnd = line.find(' ', valuePos);
            std::string score = line.substr(valuePos, valueEnd - valuePos);
            int centipawns = std::stoi(score);
            float pawns = centipawns / 100.0f;
            std::cout << "Evaluation: " << (pawns > 0 ? "+" : "") << pawns << " pawns" << std::endl;
        }
        
        size_t pvPos = line.find(" pv ");
        if (pvPos != std::string::npos) {
            std::cout << "Best line: " << line.substr(pvPos + 4) << std::endl;
        }
        
        infoPos = analysis.find("info", endLine);
    }
}

void Menu::setCustomPosition() {
    std::cout << "\nEnter FEN string (or press Enter for initial position):" << std::endl;
    std::string fen;
    std::getline(std::cin, fen);
    
    if (fen.empty()) {
        currentFen = INITIAL_FEN;
    } else {
        currentFen = fen;
    }
    
    displayBoard(currentFen);
    gameActive = true;
}

void Menu::viewEngineInfo() {
    if (!engine) {
        std::string path = getStockfishPath();
        
        std::cout << "Starting engine from: " << path << "..." << std::endl;
        
        engine = std::unique_ptr<ChessEngine>(new ChessEngine(path));
        
        if (!engine->start()) {
            std::cout << "Failed to start Stockfish at: " << path << std::endl;
            std::cout << "Please make sure Stockfish is installed in the 'stockfish' directory." << std::endl;
            engine.reset();
            return;
        }
    }
    
    std::cout << "\nEngine Information:" << std::endl;
    std::cout << "Status: " << (engine->isEngineRunning() ? "Running" : "Stopped") << std::endl;
    
    // Get engine info
    std::string info = engine->sendCommand("uci");
    
    size_t idPos = info.find("id name");
    if (idPos != std::string::npos) {
        size_t endLine = info.find('\n', idPos);
        std::cout << info.substr(idPos, endLine - idPos) << std::endl;
    }
    
    idPos = info.find("id author");
    if (idPos != std::string::npos) {
        size_t endLine = info.find('\n', idPos);
        std::cout << info.substr(idPos, endLine - idPos) << std::endl;
    }
}

void Menu::showLessonMenu() {
    int choice;
    do {
        displayLessonMenu();
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                startBeginnerLessons();
                break;
            case 2:
                startAdvancedLessons();
                break;
            case 3:
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    } while (choice != 3);
}

void Menu::startBeginnerLessons() {
    std::cout << "\n=== BEGINNER LESSONS ===" << std::endl;
    std::cout << "You will learn the basic movements of each piece.\n" << std::endl;
    
    for (size_t i = 0; i < beginnerLessons.size(); i++) {
        std::cout << "\nLesson " << (i + 1) << ": " << beginnerLessons[i].title << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
        
        runLesson(beginnerLessons[i]);
    }
    
    std::cout << "\nCongratulations! You've completed all beginner lessons!" << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void Menu::startAdvancedLessons() {
    std::cout << "\n=== ADVANCED LESSONS ===" << std::endl;
    std::cout << "You will learn famous openings and book moves.\n" << std::endl;
    
    for (size_t i = 0; i < advancedLessons.size(); i++) {
        std::cout << "\nLesson " << (i + 1) << ": " << advancedLessons[i].title << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
        
        runLesson(advancedLessons[i]);
    }
    
    std::cout << "\nExcellent! You've completed all advanced lessons!" << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void Menu::runLesson(const Lesson& lesson) {
    if (!engine) {
        std::string path = getStockfishPath();
        engine = std::unique_ptr<ChessEngine>(new ChessEngine(path));
        if (!engine->start()) {
            std::cout << "Failed to start engine. Lessons cannot continue." << std::endl;
            return;
        }
    }
    
    std::string lessonFen = lesson.startFen;
    bool lessonComplete = false;
    int attempts = 0;
    
    while (!lessonComplete && attempts < 3) {
        displayBoard(lessonFen);
        
        std::cout << "\n=== LESSON: " << lesson.title << " ===" << std::endl;
        std::cout << lesson.description << std::endl;
        std::cout << "Hint: " << lesson.hint << std::endl;
        
        std::cout << "\nEnter your move (or 'hint' for another hint, 'skip' to skip): ";
        std::string move;
        std::getline(std::cin, move);
        
        if (move == "skip") {
            std::cout << "Lesson skipped. The correct move was: " << lesson.expectedMove << std::endl;
            std::cout << "Press Enter to continue...";
            std::cin.get();
            return;
        }
        
        if (move == "hint") {
            std::cout << "Hint: " << lesson.hint << std::endl;
            attempts++;
            continue;
        }
        
        // Check if move is legal
        auto legalMoves = engine->getLegalMoves(lessonFen);
        bool isLegal = false;
        for (const auto& legalMove : legalMoves) {
            if (legalMove == move) {
                isLegal = true;
                break;
            }
        }
        
        if (!isLegal) {
            std::cout << "\nInvalid move! Please enter a move in UCI format (e.g., e2e4)." << std::endl;
            std::cout << "Legal moves: ";
            for (const auto& legalMove : legalMoves) {
                std::cout << legalMove << " ";
            }
            std::cout << std::endl;
            attempts++;
            continue;
        }
        
        // Check if it's the expected move
        if (checkLessonMove(move, lesson.expectedMove)) {
            std::cout << "\nCorrect! Well done!" << std::endl;
            lessonComplete = true;
            
            // Apply the move to show result
            std::string newFen = engine->makeMove(lessonFen, move);
            displayBoard(newFen);
        } else {
            std::cout << "\nThat's not the expected move for this lesson." << std::endl;
            std::cout << "Remember: " << lesson.description << std::endl;
            attempts++;
        }
    }
    
    if (!lessonComplete) {
        std::cout << "\nThe correct move was: " << lesson.expectedMove << std::endl;
        std::cout << "Press Enter to continue to next lesson..." << std::endl;
        std::cin.get();
    }
}

bool Menu::checkLessonMove(const std::string& move, const std::string& expected) {
    // Convert both to lowercase for comparison
    std::string moveLower = move;
    std::string expectedLower = expected;
    std::transform(moveLower.begin(), moveLower.end(), moveLower.begin(), ::tolower);
    std::transform(expectedLower.begin(), expectedLower.end(), expectedLower.begin(), ::tolower);
    
    return moveLower == expectedLower;
}

void Menu::showCustomPieceMenu() {
    int choice;
    do {
        std::cout << "\n=== CUSTOM PIECES MENU ===" << std::endl;
        std::cout << "1. Create New Custom Piece" << std::endl;
        std::cout << "2. Enable/Disable Custom Pieces (Current: " 
                  << (customPiecesEnabled ? "ENABLED" : "DISABLED") << ")" << std::endl;
        std::cout << "3. List Custom Pieces" << std::endl;
        std::cout << "4. Back to Main Menu" << std::endl;
        std::cout << "Choice: ";
        
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                createCustomPiece();
                break;
            case 2:
                enableCustomPieces();
                break;
            case 3:
                std::cout << "\nCustom Pieces:" << std::endl;
                if (customPieces.empty()) {
                    std::cout << "No custom pieces created yet." << std::endl;
                } else {
                    for (const auto& pair : customPieces) {
                        std::cout << "Symbol: " << pair.second.symbol 
                                  << " - " << pair.second.name << std::endl;
                    }
                }
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    } while (choice != 4);
}

void Menu::createCustomPiece() {
    CustomPiece newPiece;
    
    std::cout << "\n=== CREATE CUSTOM PIECE ===" << std::endl;
    
    std::cout << "Enter piece name: ";
    std::getline(std::cin, newPiece.name);
    
    std::cout << "Enter display symbol (single character): ";
    std::cin >> newPiece.symbol;
    std::cin.ignore();
    
    std::cout << "\nSelect movement pattern:" << std::endl;
    std::cout << "1. King (one square any direction)" << std::endl;
    std::cout << "2. Queen (any direction, any distance)" << std::endl;
    std::cout << "3. Rook (horizontal/vertical, any distance)" << std::endl;
    std::cout << "4. Bishop (diagonal, any distance)" << std::endl;
    std::cout << "5. Knight (L-shape, jumps)" << std::endl;
    std::cout << "6. Pawn (forward one, captures diagonal)" << std::endl;
    
    int patternChoice;
    std::cin >> patternChoice;
    std::cin.ignore();
    
    std::vector<std::string> patterns;
    switch (patternChoice) {
        case 1: patterns = {"king"}; break;
        case 2: patterns = {"queen"}; break;
        case 3: patterns = {"rook"}; break;
        case 4: patterns = {"bishop"}; break;
        case 5: patterns = {"knight"}; break;
        case 6: patterns = {"pawn"}; break;
        default: patterns = {"king"}; break;
    }
    
    newPiece.movePatterns = patterns;
    newPiece.isEnabled = true;
    
    // Store the piece (using uppercase for white, lowercase for black versions)
    customPieces[newPiece.symbol] = newPiece;
    // Also create a black version (lowercase)
    char blackSymbol = tolower(newPiece.symbol);
    newPiece.symbol = blackSymbol;
    customPieces[blackSymbol] = newPiece;
    
    std::cout << "\nCustom piece '" << newPiece.name << "' created with symbol '" 
              << newPiece.symbol << "'!" << std::endl;
    std::cout << "Both white (uppercase) and black (lowercase) versions are available." << std::endl;
}

void Menu::enableCustomPieces() {
    customPiecesEnabled = !customPiecesEnabled;
    std::cout << "\nCustom pieces are now " 
              << (customPiecesEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Menu::playLocalGame() {
    std::cout << "\n=== LOCAL GAME (WITH CUSTOM PIECES) ===" << std::endl;
    std::cout << "Playing without engine - custom pieces are supported." << std::endl;
    
    currentFen = INITIAL_FEN;
    gameActive = true;
    
    while (gameActive) {
        displayBoard(currentFen);
        
        std::cout << "\nLOCAL GAME MENU:" << std::endl;
        std::cout << "1. Make Move" << std::endl;
        std::cout << "2. Show Legal Moves (basic validation)" << std::endl;
        std::cout << "3. New Game" << std::endl;
        std::cout << "4. Back to Main Menu" << std::endl;
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                std::cout << "Enter move (e.g., e2e4): ";
                std::string move;
                std::getline(std::cin, move);
                
                // Basic move validation (simplified for local play)
                if (move.length() >= 4) {
                    lastMove = move;
                    // Simple FEN update for demo purposes
                    // In a real implementation, you'd update the board state
                    std::cout << "Move applied (simplified local game)." << std::endl;
                }
                break;
            }
            case 2:
                std::cout << "Local game: Basic move validation only." << std::endl;
                std::cout << "Custom pieces are displayed but not fully validated." << std::endl;
                break;
            case 3:
                currentFen = INITIAL_FEN;
                break;
            case 4:
                gameActive = false;
                break;
        }
    }
}

void Menu::run() {
    int choice;
    
    do {
        displayHeader();
        displayMainMenu();
        
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                newGame();
                break;
            case 2:
                setCustomPosition();
                playAgainstEngine();
                break;
            case 3:
                analyzePosition();
                break;
            case 4:
                viewEngineInfo();
                break;
            case 5:
                showLessonMenu();
                break;
            case 6:
                showCustomPieceMenu();
                break;
            case 7:
                playLocalGame();
                break;
            case 8:
                std::cout << "\nGoodbye!" << std::endl;
                break;
            default:
                std::cout << "\nInvalid choice! Please try again." << std::endl;
        }
    } while (choice != 8);
}