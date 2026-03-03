#include "Menu.h"
#include "ChessEngine.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <memory>
#include <map>
#include <thread>
#include <chrono>

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

Menu::Menu() : 
    engine(nullptr),
    currentFen(INITIAL_FEN), 
    lastMove(""),
    gameActive(false), 
    customPiecesEnabled(false),
    currentDifficulty(MEDIUM),
    engineMoveTimeMs(1000),
    currentLessonIndex(0) {
    initializeLessons();
    initializeFenLessons();
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

void Menu::initializeFenLessons() {
    fenLessons = {
        {
            "FEN Structure - Basic Components",
            "FEN (Forsyth-Edwards Notation) is a standard notation for describing chess positions. "
            "It consists of 6 fields separated by spaces:\n"
            "1. Piece placement (from rank 8 to 1)\n"
            "2. Active color (w or b)\n"
            "3. Castling availability (KQkq or -)\n"
            "4. En passant target square (or -)\n"
            "5. Halfmove clock\n"
            "6. Fullmove number\n\n"
            "Example: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            
            "How many fields are in a FEN string?",
            "6",
            "Count the parts separated by spaces in the example.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        },
        {
            "Piece Placement - Ranks and Files",
            "The first field shows piece placement from rank 8 to rank 1. "
            "Uppercase letters = White pieces, lowercase = Black pieces.\n"
            "P/p = Pawn, N/n = Knight, B/b = Bishop, R/r = Rook, Q/q = Queen, K/k = King\n"
            "Numbers represent consecutive empty squares.\n"
            "Slashes (/) separate ranks.\n\n"
            "Example: 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR'",
            
            "In the initial position, how many empty squares are on rank 6?",
            "8",
            "Look at the third field: '8' represents 8 empty squares on rank 6.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
        },
        {
            "Reading Piece Placement",
            "Each character represents a piece or empty square count.\n"
            "Let's decode: 'r' = black rook, 'n' = black knight, 'b' = black bishop, etc.",
            
            "What piece does 'Q' represent in FEN notation?",
            "white queen",
            "Uppercase letters are White pieces, lowercase are Black. Q = Queen.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
        },
        {
            "Active Color",
            "The second field indicates which player's turn it is: 'w' for White, 'b' for Black.",
            
            "In the FEN 'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 4', whose turn is it?",
            "white",
            "The 'w' after the board position means White to move.",
            "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 4"
        },
        {
            "Castling Rights",
            "The third field shows castling availability:\n"
            "K = White kingside, Q = White queenside\n"
            "k = Black kingside, q = Black queenside\n"
            "- = No castling available",
            
            "In 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', what castling rights exist?",
            "both sides can castle on both wings",
            "KQ means White can castle both sides, kq means Black can castle both sides.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        },
        {
            "En Passant Target",
            "The fourth field shows the en passant target square (or '-' if none). "
            "This square is behind a pawn that just moved two squares.",
            
            "In 'rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1', what is the en passant target?",
            "e3",
            "The square after 'e3' indicates en passant is available on e3.",
            "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
        },
        {
            "Halfmove and Fullmove Counters",
            "The fifth field is the halfmove clock (moves since last pawn move or capture).\n"
            "The sixth field is the fullmove number (starts at 1 and increments after Black's move).",
            
            "In 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', what is the fullmove number?",
            "1",
            "The last number is the fullmove counter.",
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        },
        {
            "Reading a Complete FEN",
            "Now let's read a complete FEN: "
            "'r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 4'",
            
            "After 1.e4 e5 2.Nf3 Nc6, what is the active color?",
            "white",
            "The 'w' after the board tells us it's White's turn.",
            "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 4"
        },
        {
            "Creating Your Own FEN - Part 1",
            "Create a FEN for an empty board (no pieces).\n"
            "Hint: 8 ranks, each with 8 empty squares.",
            
            "What is the piece placement string for an empty board?",
            "8/8/8/8/8/8/8/8",
            "Each rank is represented by '8' for 8 empty squares, separated by slashes.",
            "8/8/8/8/8/8/8/8 w - - 0 1"
        },
        {
            "Creating Your Own FEN - Part 2",
            "Create a FEN for a position with:\n"
            "- White king on e1\n"
            "- White pawn on e2\n"
            "- Black king on e8\n"
            "- Black pawn on e7\n"
            "All other squares empty. It's White's turn.",
            
            "Write the complete FEN string:",
            "8/4k3/8/8/8/8/4P3/4K3 w - - 0 1",
            "Rank 8: '4k3' (4 empty, black king, 3 empty)\n"
            "Rank 7: '8' (all empty)\n"
            "Rank 2: '4P3' (4 empty, white pawn, 3 empty)\n"
            "Rank 1: '4K3' (4 empty, white king, 3 empty)",
            "8/4k3/8/8/8/8/4P3/4K3 w - - 0 1"
        },
        {
            "Practical Application - Reading Positions",
            "Look at this FEN: '8/5k2/8/3K4/8/8/8/8 w - - 0 1'\n"
            "Where are the kings?",
            
            "Where is the white king?",
            "d5",
            "K at position d5 means white king on d5. '3K4' in rank 4 means: 3 empty, King, 4 empty.",
            "8/5k2/8/3K4/8/8/8/8 w - - 0 1"
        },
        {
            "Advanced FEN - Castling Rights",
            "In this position: 'r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1'\n"
            "What castling rights exist?",
            
            "Can White castle queenside?",
            "yes",
            "'KQ' in the castling field means White can castle both sides. Q = queenside.",
            "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
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
    
    // Show difficulty if playing vs engine
    std::string diffStr;
    switch (currentDifficulty) {
        case BEGINNER: diffStr = "Beginner"; break;
        case EASY: diffStr = "Easy"; break;
        case MEDIUM: diffStr = "Medium"; break;
        case HARD: diffStr = "Hard"; break;
        case EXPERT: diffStr = "Expert"; break;
    }
    std::cout << "Difficulty: " << diffStr << std::endl;
}

void Menu::displayMainMenu() {
    std::cout << "\nMAIN MENU:" << std::endl;
    std::cout << "1. Play vs Computer (Engine)" << std::endl;
    std::cout << "2. Local PvP (2 Players)" << std::endl;
    std::cout << "3. Set Custom Position" << std::endl;
    std::cout << "4. Analyze Position" << std::endl;
    std::cout << "5. Engine Info" << std::endl;
    std::cout << "6. Lessons" << std::endl;
    std::cout << "7. Custom Pieces" << std::endl;
    std::cout << "8. Local Game (with Custom Pieces)" << std::endl;
    std::cout << "9. Set Difficulty" << std::endl;
    std::cout << "10. Exit" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::displayVsEngineMenu() {
    std::cout << "\nVS COMPUTER MENU:" << std::endl;
    std::cout << "1. Make Your Move" << std::endl;
    std::cout << "2. Show Legal Moves" << std::endl;
    std::cout << "3. New Game" << std::endl;
    std::cout << "4. Change Difficulty" << std::endl;
    std::cout << "5. Back to Main Menu" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::displayGameMenu() {
    std::cout << "\nLOCAL PVP MENU:" << std::endl;
    std::cout << "1. Make Move" << std::endl;
    std::cout << "2. Get Engine Suggestion" << std::endl;
    std::cout << "3. Analyze Current Position" << std::endl;
    std::cout << "4. Show Legal Moves" << std::endl;
    std::cout << "5. New Game" << std::endl;
    std::cout << "6. Back to Main Menu" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::displayDifficultyMenu() {
    std::cout << "\n=== DIFFICULTY SETTINGS ===" << std::endl;
    std::cout << "Current difficulty: ";
    switch (currentDifficulty) {
        case BEGINNER: std::cout << "Beginner"; break;
        case EASY: std::cout << "Easy"; break;
        case MEDIUM: std::cout << "Medium"; break;
        case HARD: std::cout << "Hard"; break;
        case EXPERT: std::cout << "Expert"; break;
    }
    std::cout << " (" << engineMoveTimeMs << "ms thinking time)" << std::endl;
    std::cout << "\nSelect difficulty:" << std::endl;
    std::cout << "1. Beginner (very weak, fast responses)" << std::endl;
    std::cout << "2. Easy (weak)" << std::endl;
    std::cout << "3. Medium (moderate)" << std::endl;
    std::cout << "4. Hard (strong)" << std::endl;
    std::cout << "5. Expert (very strong, slower)" << std::endl;
    std::cout << "6. Back" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::displayLessonMenu() {
    std::cout << "\n=== LESSON MENU ===" << std::endl;
    std::cout << "1. Beginner Lessons (Piece Movement)" << std::endl;
    std::cout << "2. Advanced Lessons (Openings & Book Moves)" << std::endl;
    std::cout << "3. FEN Notation Lessons (Read & Write Positions)" << std::endl;
    std::cout << "4. Back to Main Menu" << std::endl;
    std::cout << "\nChoice: ";
}

void Menu::displayFenLessonMenu() {
    std::cout << "\nFEN NOTATION LESSONS:" << std::endl;
    std::cout << "Learn to read and write Forsyth-Edwards Notation (FEN)" << std::endl;
    std::cout << "Total lessons: " << fenLessons.size() << std::endl;
    std::cout << "\n1. Start FEN Lessons" << std::endl;
    std::cout << "2. Back to Lesson Menu" << std::endl;
    std::cout << "\nChoice: ";
}

std::string Menu::getStockfishPath() {
#ifdef _WIN32
    return "stockfish\\stockfish.exe";
#else
    return "./stockfish/stockfish";
#endif
}

void Menu::setDifficulty() {
    int choice;
    do {
        displayDifficultyMenu();
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                currentDifficulty = BEGINNER;
                engineMoveTimeMs = 100;
                std::cout << "\nDifficulty set to Beginner (100ms thinking time)" << std::endl;
                break;
            case 2:
                currentDifficulty = EASY;
                engineMoveTimeMs = 250;
                std::cout << "\nDifficulty set to Easy (250ms thinking time)" << std::endl;
                break;
            case 3:
                currentDifficulty = MEDIUM;
                engineMoveTimeMs = 500;
                std::cout << "\nDifficulty set to Medium (500ms thinking time)" << std::endl;
                break;
            case 4:
                currentDifficulty = HARD;
                engineMoveTimeMs = 1000;
                std::cout << "\nDifficulty set to Hard (1000ms thinking time)" << std::endl;
                break;
            case 5:
                currentDifficulty = EXPERT;
                engineMoveTimeMs = 2000;
                std::cout << "\nDifficulty set to Expert (2000ms thinking time)" << std::endl;
                break;
            case 6:
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
        
        // Apply skill level to engine if it's running
        if (engine && engine->isEngineRunning()) {
            // Set UCI_Elo or skill level based on difficulty
            int skillLevel;
            switch (currentDifficulty) {
                case BEGINNER: skillLevel = 0; break;
                case EASY: skillLevel = 5; break;
                case MEDIUM: skillLevel = 10; break;
                case HARD: skillLevel = 15; break;
                case EXPERT: skillLevel = 20; break;
                default: skillLevel = 10;
            }
            engine->sendCommand("setoption name Skill Level value " + std::to_string(skillLevel));
        }
    } while (choice != 6);
}

void Menu::newGame() {
    currentFen = INITIAL_FEN;
    lastMove = "";
    gameActive = true;
    
    std::cout << "\nStarting new game..." << std::endl;
    displayBoard(currentFen);
}

void Menu::playVsEngine() {
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
        
        // Set initial skill level
        int skillLevel;
        switch (currentDifficulty) {
            case BEGINNER: skillLevel = 0; break;
            case EASY: skillLevel = 5; break;
            case MEDIUM: skillLevel = 10; break;
            case HARD: skillLevel = 15; break;
            case EXPERT: skillLevel = 20; break;
            default: skillLevel = 10;
        }
        engine->sendCommand("setoption name Skill Level value " + std::to_string(skillLevel));
        
        std::cout << "Engine started successfully!" << std::endl;
    }
    
    newGame();
    
    while (gameActive) {
        displayBoard(currentFen);
        
        // Check if game is over
        auto legalMoves = engine->getLegalMoves(currentFen);
        if (legalMoves.empty()) {
            std::cout << "\nGame Over! No legal moves available." << std::endl;
            break;
        }
        
        // Determine whose turn it is
        size_t spacePos = currentFen.find(' ');
        std::string turn = (spacePos != std::string::npos) ? currentFen.substr(spacePos + 1, 1) : "w";
        
        if (turn == "w") {
            // Player's turn (White)
            std::cout << "\nYour turn (White)." << std::endl;
            displayVsEngineMenu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: {
                    // Make player move
                    std::cout << "\nEnter your move (e.g., e2e4) or 'q' to cancel: ";
                    std::string move;
                    std::getline(std::cin, move);
                    
                    if (move == "q" || move == "Q") {
                        std::cout << "Move cancelled." << std::endl;
                        continue;
                    }
                    
                    // Check if move is legal
                    bool isLegal = false;
                    for (const auto& legalMove : legalMoves) {
                        if (legalMove == move) {
                            isLegal = true;
                            break;
                        }
                    }
                    
                    if (!isLegal) {
                        std::cout << "\nInvalid move! Legal moves: ";
                        for (const auto& legalMove : legalMoves) {
                            std::cout << legalMove << " ";
                        }
                        std::cout << std::endl;
                        std::cout << "Press Enter to continue...";
                        std::cin.get();
                        continue;
                    }
                    
                    // Apply player move
                    std::string newFen = engine->makeMove(currentFen, move);
                    if (newFen != currentFen) {
                        lastMove = move;
                        currentFen = newFen;
                        std::cout << "\nMove applied!" << std::endl;
                    }
                    break;
                }
                case 2:
                    std::cout << "\nLegal moves: ";
                    for (const auto& move : legalMoves) {
                        std::cout << move << " ";
                    }
                    std::cout << std::endl;
                    std::cout << "Press Enter to continue...";
                    std::cin.get();
                    break;
                case 3:
                    newGame();
                    break;
                case 4:
                    setDifficulty();
                    break;
                case 5:
                    gameActive = false;
                    break;
                default:
                    std::cout << "Invalid choice!" << std::endl;
            }
        } else {
            // Engine's turn (Black)
            std::cout << "\nEngine is thinking";
            for (int i = 0; i < 3; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::cout << ".";
                std::flush(std::cout);
            }
            std::cout << std::endl;
            
            std::string bestMove = engine->getBestMove(currentFen, engineMoveTimeMs);
            
            if (!bestMove.empty()) {
                std::cout << "\nEngine plays: " << bestMove << std::endl;
                
                // Validate and apply the engine's move
                if (engine->isValidMove(currentFen, bestMove)) {
                    std::string newFen = engine->makeMove(currentFen, bestMove);
                    if (newFen != currentFen) {
                        lastMove = bestMove;
                        currentFen = newFen;
                        std::cout << "Engine move applied!" << std::endl;
                    }
                }
            } else {
                std::cout << "Engine couldn't find a move. Game may be over." << std::endl;
            }
            
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }
    }
}

void Menu::playLocalPvp() {
    newGame();
    
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
                if (!engine) {
                    std::string path = getStockfishPath();
                    engine = std::unique_ptr<ChessEngine>(new ChessEngine(path));
                    if (!engine->start()) {
                        std::cout << "Failed to start engine for suggestion." << std::endl;
                        break;
                    }
                }
                std::cout << "\nCalculating suggestion..." << std::endl;
                std::string bestMove = engine->getBestMove(currentFen, 1000);
                if (!bestMove.empty()) {
                    std::cout << "Engine suggests: " << bestMove << std::endl;
                }
                std::cout << "Press Enter to continue...";
                std::cin.get();
                break;
            }
            case 3:
                analyzePosition();
                break;
            case 4: {
                if (!engine) {
                    std::string path = getStockfishPath();
                    engine = std::unique_ptr<ChessEngine>(new ChessEngine(path));
                    if (!engine->start()) {
                        std::cout << "Failed to start engine." << std::endl;
                        break;
                    }
                }
                auto moves = engine->getLegalMoves(currentFen);
                std::cout << "\nLegal moves (" << moves.size() << "): ";
                for (const auto& move : moves) {
                    std::cout << move << " ";
                }
                std::cout << std::endl;
                std::cout << "Press Enter to continue...";
                std::cin.get();
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
    
    if (!engine) {
        std::string path = getStockfishPath();
        engine = std::unique_ptr<ChessEngine>(new ChessEngine(path));
        if (!engine->start()) {
            std::cout << "Failed to start engine for move validation." << std::endl;
            return;
        }
    }
    
    auto legalMoves = engine->getLegalMoves(currentFen);
    
    if (legalMoves.empty()) {
        std::cout << "No legal moves found. Game may be over." << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
        return;
    }
    
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
        std::cout << "Move '" << move << "' is not legal." << std::endl;
        std::cout << "Legal moves: ";
        for (const auto& legalMove : legalMoves) {
            std::cout << legalMove << " ";
        }
        std::cout << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
        return;
    }
    
    // Apply the move
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
        
        // Look for mate
        size_t matePos = line.find("score mate");
        if (matePos != std::string::npos) {
            size_t valuePos = matePos + 11;
            size_t valueEnd = line.find(' ', valuePos);
            std::string mateIn = line.substr(valuePos, valueEnd - valuePos);
            std::cout << "Evaluation: Mate in " << mateIn << std::endl;
        }
        
        size_t pvPos = line.find(" pv ");
        if (pvPos != std::string::npos) {
            std::cout << "Best line: " << line.substr(pvPos + 4) << std::endl;
        }
        
        infoPos = analysis.find("info", endLine);
    }
    
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
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
    
    std::cout << "\nCurrent difficulty setting: ";
    switch (currentDifficulty) {
        case BEGINNER: std::cout << "Beginner"; break;
        case EASY: std::cout << "Easy"; break;
        case MEDIUM: std::cout << "Medium"; break;
        case HARD: std::cout << "Hard"; break;
        case EXPERT: std::cout << "Expert"; break;
    }
    std::cout << " (" << engineMoveTimeMs << "ms)" << std::endl;
    
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
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
                startFenLessons();
                break;
            case 4:
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    } while (choice != 4);
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

void Menu::startFenLessons() {
    std::cout << "\n=== FEN NOTATION LESSONS ===" << std::endl;
    std::cout << "You will learn how to read and write FEN strings.\n" << std::endl;
    std::cout << "FEN (Forsyth-Edwards Notation) is the standard way to record chess positions.\n";
    std::cout << "It's used in databases, engines, and for sharing positions.\n" << std::endl;
    
    for (size_t i = 0; i < fenLessons.size(); i++) {
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << "Lesson " << (i + 1) << ": " << fenLessons[i].title << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        std::cout << "Press Enter to continue...";
        std::cin.get();
        
        runFenLesson(fenLessons[i]);
    }
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Congratulations! You've completed all FEN notation lessons!" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
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
    const int maxAttempts = 3;
    
    while (!lessonComplete && attempts < maxAttempts) {
        displayBoard(lessonFen);
        
        std::cout << "\n=== LESSON: " << lesson.title << " ===" << std::endl;
        std::cout << lesson.description << std::endl;
        
        std::cout << "\nEnter your move (or 'hint' for help, 'skip' to skip): ";
        std::string move;
        std::getline(std::cin, move);
        
        // Trim whitespace
        move.erase(0, move.find_first_not_of(" \t\n\r"));
        move.erase(move.find_last_not_of(" \t\n\r") + 1);
        
        // Convert to lowercase for command checking
        std::string moveLower = move;
        std::transform(moveLower.begin(), moveLower.end(), moveLower.begin(), ::tolower);
        
        if (moveLower == "skip") {
            std::cout << "Lesson skipped. The correct move was: " << lesson.expectedMove << std::endl;
            std::cout << "Press Enter to continue...";
            std::cin.get();
            return;
        }
        
        if (moveLower == "hint") {
            if (attempts < maxAttempts - 1) {
                std::cout << "\nHINT: " << lesson.hint << std::endl;
                std::cout << "Attempts remaining: " << (maxAttempts - attempts - 1) << std::endl;
                attempts++;
                std::cout << "\nPress Enter to try again...";
                std::cin.get();
                continue;
            } else {
                std::cout << "\nNo more hints available. Try to make a move or type 'skip'." << std::endl;
                continue;
            }
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
            std::cout << "\nInvalid move! Please enter a legal move in UCI format (e.g., e2e4)." << std::endl;
            std::cout << "Legal moves: ";
            for (const auto& legalMove : legalMoves) {
                std::cout << legalMove << " ";
            }
            std::cout << std::endl;
            attempts++;
            
            if (attempts < maxAttempts) {
                std::cout << "Attempts remaining: " << (maxAttempts - attempts) << std::endl;
                if (attempts == maxAttempts - 1) {
                    std::cout << "Hint: " << lesson.hint << std::endl;
                }
            }
            continue;
        }
        
        // Check if it's the expected move
        if (checkLessonMove(move, lesson.expectedMove)) {
            std::cout << "\n✓ Correct! Well done!" << std::endl;
            lessonComplete = true;
            
            // Apply the move to show result
            std::string newFen = engine->makeMove(lessonFen, move);
            displayBoard(newFen);
        } else {
            std::cout << "\n✗ That's not the expected move for this lesson." << std::endl;
            std::cout << "Remember: " << lesson.description << std::endl;
            attempts++;
            
            if (attempts < maxAttempts) {
                std::cout << "Attempts remaining: " << (maxAttempts - attempts) << std::endl;
                if (attempts == maxAttempts - 1) {
                    std::cout << "Hint: " << lesson.hint << std::endl;
                }
            }
        }
    }
    
    if (!lessonComplete) {
        std::cout << "\nThe correct move was: " << lesson.expectedMove << std::endl;
        std::cout << "Press Enter to continue to next lesson..." << std::endl;
        std::cin.get();
    }
}

void Menu::runFenLesson(const FenLesson& lesson) {
    bool lessonComplete = false;
    int attempts = 0;
    const int maxAttempts = 3;
    
    while (!lessonComplete && attempts < maxAttempts) {
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << "LESSON: " << lesson.title << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        std::cout << lesson.description << std::endl;
        
        if (!lesson.exampleFen.empty()) {
            std::cout << "\nExample FEN: " << lesson.exampleFen << std::endl;
            std::cout << "\nCorresponding board position:" << std::endl;
            displayBoard(lesson.exampleFen);
        }
        
        std::cout << "\nQUESTION: " << lesson.question << std::endl;
        std::cout << "Your answer (or type 'hint' for help, 'skip' to skip): ";
        
        std::string answer;
        std::getline(std::cin, answer);
        
        // Trim whitespace from answer
        answer.erase(0, answer.find_first_not_of(" \t\n\r"));
        answer.erase(answer.find_last_not_of(" \t\n\r") + 1);
        
        // Convert to lowercase for easier comparison
        std::string answerLower = answer;
        std::transform(answerLower.begin(), answerLower.end(), answerLower.begin(), ::tolower);
        
        // Check for special commands first
        if (answerLower == "skip") {
            std::cout << "\nLesson skipped. The correct answer was: " << lesson.expectedAnswer << std::endl;
            std::cout << "Press Enter to continue...";
            std::cin.get();
            return;
        }
        
        if (answerLower == "hint") {
            if (attempts < maxAttempts - 1) {
                std::cout << "\nHINT: " << lesson.hint << std::endl;
                std::cout << "Attempts remaining: " << (maxAttempts - attempts - 1) << std::endl;
                attempts++;
                std::cout << "\nPress Enter to try again...";
                std::cin.get();
                continue;
            } else {
                std::cout << "\nNo more hints available. Try to answer or type 'skip'." << std::endl;
                continue;
            }
        }
        
        // Check the answer
        if (checkFenAnswer(answer, lesson.expectedAnswer)) {
            std::cout << "\n✓ Correct! Well done!" << std::endl;
            lessonComplete = true;
            
            if (!lesson.exampleFen.empty()) {
                std::cout << "\nReview the position:" << std::endl;
                displayBoard(lesson.exampleFen);
            }
        } else {
            std::cout << "\n✗ That's not correct." << std::endl;
            attempts++;
            
            if (attempts < maxAttempts) {
                std::cout << "Attempts remaining: " << (maxAttempts - attempts) << std::endl;
                if (attempts == maxAttempts - 1) {
                    std::cout << "Last chance! Hint: " << lesson.hint << std::endl;
                }
            }
        }
    }
    
    if (!lessonComplete) {
        std::cout << "\nThe correct answer was: " << lesson.expectedAnswer << std::endl;
        std::cout << "Review the lesson material and try again later." << std::endl;
    }
    
    std::cout << "\nPress Enter to continue to next lesson...";
    std::cin.get();
}

bool Menu::checkLessonMove(const std::string& move, const std::string& expected) {
    // Convert both to lowercase for comparison
    std::string moveLower = move;
    std::string expectedLower = expected;
    std::transform(moveLower.begin(), moveLower.end(), moveLower.begin(), ::tolower);
    std::transform(expectedLower.begin(), expectedLower.end(), expectedLower.begin(), ::tolower);
    
    return moveLower == expectedLower;
}

bool Menu::checkFenAnswer(const std::string& answer, const std::string& expected) {
    std::string answerLower = answer;
    std::string expectedLower = expected;
    std::transform(answerLower.begin(), answerLower.end(), answerLower.begin(), ::tolower);
    std::transform(expectedLower.begin(), expectedLower.end(), expectedLower.begin(), ::tolower);
    
    // Check for exact match
    if (answerLower == expectedLower) {
        return true;
    }
    
    // Check for numeric answers (like "6" vs "six")
    if (expectedLower == "6" && (answerLower == "six" || answerLower == "6")) {
        return true;
    }
    
    // Check for "white" vs "w" variations
    if (expectedLower == "white" && (answerLower == "white" || answerLower == "w")) {
        return true;
    }
    if (expectedLower == "black" && (answerLower == "black" || answerLower == "b")) {
        return true;
    }
    
    // Check for yes/no variations
    if (expectedLower == "yes" && (answerLower == "yes" || answerLower == "y")) {
        return true;
    }
    if (expectedLower == "no" && (answerLower == "no" || answerLower == "n")) {
        return true;
    }
    
    // Check for common FEN answers
    if (expectedLower == "8/8/8/8/8/8/8/8" && answerLower == "8/8/8/8/8/8/8/8") {
        return true;
    }
    
    return false;
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
                std::cout << "Press Enter to continue...";
                std::cin.get();
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
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void Menu::enableCustomPieces() {
    customPiecesEnabled = !customPiecesEnabled;
    std::cout << "\nCustom pieces are now " 
              << (customPiecesEnabled ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.get();
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
                std::cout << "Press Enter to continue...";
                std::cin.get();
                break;
            }
            case 2:
                std::cout << "Local game: Basic move validation only." << std::endl;
                std::cout << "Custom pieces are displayed but not fully validated." << std::endl;
                std::cout << "Press Enter to continue...";
                std::cin.get();
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
                playVsEngine();
                break;
            case 2:
                playLocalPvp();
                break;
            case 3:
                setCustomPosition();
                playLocalPvp();
                break;
            case 4:
                analyzePosition();
                break;
            case 5:
                viewEngineInfo();
                break;
            case 6:
                showLessonMenu();
                break;
            case 7:
                showCustomPieceMenu();
                break;
            case 8:
                playLocalGame();
                break;
            case 9:
                setDifficulty();
                break;
            case 10:
                std::cout << "\nGoodbye!" << std::endl;
                break;
            default:
                std::cout << "\nInvalid choice! Please try again." << std::endl;
        }
    } while (choice != 10);
}