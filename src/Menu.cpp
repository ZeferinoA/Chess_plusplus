#include "Menu.h"
#include "ChessEngine.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <memory>

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

Menu::Menu() : currentFen(INITIAL_FEN), gameActive(false) {
    // Engine will be initialized when needed
}

Menu::~Menu() = default;

void Menu::displayHeader() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "        TERMINAL CHESS ENGINE - STOCKFISH" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
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
                // Find piece symbol
                int pieceIndex = 0;
                for (int i = 0; i < 14; i++) {
                    if (PIECE_SYMBOLS[i] == ch) {
                        pieceIndex = i;
                        break;
                    }
                }
                
                char displayChar = " PNBRQK  pnbrqk"[pieceIndex];
                if (displayChar == ' ') displayChar = ch;
                
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
    std::cout << "5. Exit" << std::endl;
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
                std::cout << "\nGoodbye!" << std::endl;
                break;
            default:
                std::cout << "\nInvalid choice! Please try again." << std::endl;
        }
    } while (choice != 5);
}

