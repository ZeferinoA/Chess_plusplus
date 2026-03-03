Chess++
Terminal Based Chess Implementation

Chess++ is a terminal-based chess application that interfaces with the Stockfish chess engine, designed for both beginners learning the game and advanced players seeking to improve their skills. The program provides a minimalist, text-based interface for playing against the engine, analyzing positions, and exploring chess variations.

Idea

This program simulates a chess game for players of all skill levels who want to take the game into their own hands:

    For Beginners: Learn the basics of how each piece moves, understand legal moves, and gradually work up to complex openings and book moves. The program displays the board clearly and shows all legal moves for any position.

    For Advanced Players: Improve your game with deep position analysis, engine evaluation, and the ability to analyze specific positions. The program interfaces with Stockfish, one of the strongest chess engines in the world, providing professional-grade analysis.

    Future Additions: The long-term goal includes creating new pieces and new ways to play, as well as local 2 player control

Goal

The primary goals of Chess++ are to:

    Teach users how to play chess through interactive gameplay

    Provide powerful analysis tools for players looking to improve

    Create a foundation for future expansion, including custom pieces and game variations

    Enable both single-player practice against the engine and local multiplayer functionality

Features

    Full Chess Gameplay: Play complete games against the Stockfish engine

    Position Analysis: Get detailed analysis of any position with evaluation in pawns

    Legal Move Display: View all legal moves for the current position

    Custom Positions: Set up any position using FEN (Forsyth-Edwards Notation)

    Engine Information: View details about the Stockfish engine

    Interactive Menu: User-friendly terminal interface with clear navigation

    Cross-Platform: Works on both Windows and Linux/Unix systems

Prerequisites
Required Software

    C++ Compiler: GCC (g++) 4.8.1 or later, or MSVC 2015 or later

    Make: Build automation tool

    Stockfish Chess Engine: Already included in the stockfish/ directory

## Quick Start
in Chess_plusplus directory use:

make run

This single command will:

    Compile all source files (main.cpp, Menu.cpp, ChessEngine.cpp)

    Link them into the chesspp executable

    Run the program automatically

Available Make Commands

    make: Compiles the program to create the chesspp executable

    make run: Compiles (if needed) and runs the program

    make clean: Removes compiled object files and executable

Usage Guide
Main Menu Options

When you run the program, you'll see the main menu:
text

==================================================
        TERMINAL CHESS ENGINE - STOCKFISH
==================================================

MAIN MENU:
1. New Game (Play vs Engine)
2. Set Custom Position
3. Analyze Position
4. Engine Info
5. Exit

Choice:

    New Game (Play vs Engine)

        Starts a new game from the initial position

        You play as White, engine plays as Black

        Enter moves in UCI format (e.g., e2e4)

    Set Custom Position

        Enter any valid FEN string

        Press Enter for the initial position

        Useful for studying specific scenarios

    Analyze Position

        Get deep analysis (depth 15) of the current position

        Shows evaluation in pawns (e.g., "+0.5" means advantage of half a pawn)

        Displays the best line of play

    Engine Info

        Shows Stockfish version and author information

        Confirms engine is running properly

    Exit

        Quits the program

Game Menu Options

During active gameplay, you'll see the game menu:
text

GAME MENU:
1. Make Move
2. Get Engine Move
3. Analyze Current Position
4. Show Legal Moves
5. New Game
6. Back to Main Menu

Choice:

    Make Move

        Enter your move in UCI format (e.g., e2e4, g1f3)

        The program validates if the move is legal

        Shows all legal moves for reference

    Get Engine Move

        Engine calculates the best move (2 seconds思考时间)

        Option to apply the suggested move

        Good for learning or when stuck

    Analyze Current Position

        Get real-time analysis of the current game state

        Helps understand position strengths/weaknesses

    Show Legal Moves

        Displays all legal moves for the current position

        Excellent learning tool for beginners

    New Game

        Restart from initial position

    Back to Main Menu

        Return to main menu while keeping the current game state

Board Display

The board is displayed with coordinates for easy reference:
text

       a       b       c       d       e       f       g       h
   -----------------------------------------------------------------
 8 |   r    |   n    |   b    |   q    |   k    |   b    |   n    |   r    |
   -----------------------------------------------------------------
 7 |   p    |   p    |   p    |   p    |   p    |   p    |   p    |   p    |
   -----------------------------------------------------------------
 6 |   .    |   .    |   .    |   .    |   .    |   .    |   .    |   .    |
   -----------------------------------------------------------------
 5 |   .    |   .    |   .    |   .    |   .    |   .    |   .    |   .    |
   -----------------------------------------------------------------
 4 |   .    |   .    |   .    |   .    |   .    |   .    |   .    |   .    |
   -----------------------------------------------------------------
 3 |   .    |   .    |   .    |   .    |   .    |   .    |   .    |   .    |
   -----------------------------------------------------------------
 2 |   P    |   P    |   P    |   P    |   P    |   P    |   P    |   P    |
   -----------------------------------------------------------------
 1 |   R    |   N    |   B    |   Q    |   K    |   B    |   N    |   R    |
   -----------------------------------------------------------------
       a       b       c       d       e       f       g       h

Turn: White

    Uppercase letters (P,N,B,R,Q,K) represent White pieces

    Lowercase letters (p,n,b,r,q,k) represent Black pieces

    Dots (.) represent empty squares

FEN Notation

The program uses FEN (Forsyth-Edwards Notation) to represent board positions. A FEN string contains:

    Piece placement (from White's perspective)

    Active color

    Castling availability

    En passant target square

    Halfmove clock

    Fullmove number

Example (initial position):
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
UCI Move Format

Moves should be entered in Universal Chess Interface (UCI) format:

    Pawn moves: e2e4 (from square to square)

    Knight moves: g1f3

    Castling: e1g1 (king-side) or e1c1 (queen-side)

    Promotion: a7a8q (appending promotion piece: q/r/b/n)

Troubleshooting
Engine Won't Start

The Stockfish executable is included, but if you encounter issues:

    Windows: Ensure stockfish/stockfish.exe exists

    Linux/Unix: Ensure stockfish/stockfish has execute permissions:
    bash

    chmod +x stockfish/stockfish

No Legal Moves Displayed

    Verify the position is valid

    Check engine communication by selecting "Engine Info" from the main menu

    The game might be over (checkmate or stalemate)

Compilation Errors

    Ensure you have a C++11-compatible compiler

    Run make clean then make to force a fresh compilation

This project is for educational purposes. Stockfish is licensed under GPLv3.
Acknowledgments

    Stockfish chess engine team

    The open-source chess community
