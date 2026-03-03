#include "ChessEngine.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <sstream>
#include <algorithm>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

ChessEngine::ChessEngine(const std::string& path) 
    : pipeHandle(nullptr), isRunning(false), stockfishPath(path) {
}

ChessEngine::~ChessEngine() {
    stop();
}

#ifdef _WIN32
// Windows implementation (keep your existing Windows code here)
bool ChessEngine::openPipe() {
    HANDLE hPipeRead, hPipeWrite;
    HANDLE hChildStdinRd, hChildStdinWr;
    HANDLE hChildStdoutRd, hChildStdoutWr;
    
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    
    if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &sa, 0)) {
        return false;
    }
    if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &sa, 0)) {
        CloseHandle(hChildStdoutRd);
        CloseHandle(hChildStdoutWr);
        return false;
    }
    
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;
    si.hStdInput = hChildStdinRd;
    
    std::string cmd = stockfishPath;
    if (!CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, 
                       CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hChildStdoutRd);
        CloseHandle(hChildStdoutWr);
        CloseHandle(hChildStdinRd);
        CloseHandle(hChildStdinWr);
        return false;
    }
    
    CloseHandle(hChildStdoutWr);
    CloseHandle(hChildStdinRd);
    
    pipeHandle = (void*)new HANDLE[2];
    ((HANDLE*)pipeHandle)[0] = hChildStdoutRd;
    ((HANDLE*)pipeHandle)[1] = hChildStdinWr;
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return true;
}

void ChessEngine::closePipe() {
    if (pipeHandle) {
        CloseHandle(((HANDLE*)pipeHandle)[0]);
        CloseHandle(((HANDLE*)pipeHandle)[1]);
        delete[] (HANDLE*)pipeHandle;
        pipeHandle = nullptr;
    }
}

bool ChessEngine::writeToEngine(const std::string& command) {
    if (!pipeHandle) return false;
    
    DWORD written;
    std::string cmd = command + "\n";
    return WriteFile(((HANDLE*)pipeHandle)[1], cmd.c_str(), cmd.length(), &written, NULL);
}

std::string ChessEngine::readFromEngine(int timeoutMs) {
    if (!pipeHandle) return "";
    
    std::string result;
    char buffer[4096];
    DWORD bytesRead;
    
    auto startTime = std::chrono::steady_clock::now();
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::steady_clock::now() - startTime).count() < timeoutMs) {
        
        if (PeekNamedPipe(((HANDLE*)pipeHandle)[0], NULL, 0, NULL, &bytesRead, NULL) && bytesRead > 0) {
            if (ReadFile(((HANDLE*)pipeHandle)[0], buffer, sizeof(buffer)-1, &bytesRead, NULL)) {
                buffer[bytesRead] = '\0';
                result += buffer;
            }
        }
        
        if (result.find("bestmove") != std::string::npos || 
            result.find("readyok") != std::string::npos) {
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return result;
}

#else
// Linux/Unix implementation
bool ChessEngine::openPipe() {
    int stdin_pipe[2];
    int stdout_pipe[2];
    
    if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
        return false;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        return false;
    }
    
    if (pid == 0) {
        // Child process
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stdout_pipe[1], STDERR_FILENO);
        
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        
        execlp(stockfishPath.c_str(), stockfishPath.c_str(), NULL);
        exit(1);
    }
    
    // Parent process
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    
    int* handles = new int[2];
    handles[0] = stdout_pipe[0];  // Read from engine
    handles[1] = stdin_pipe[1];    // Write to engine
    pipeHandle = handles;
    
    return true;
}

void ChessEngine::closePipe() {
    if (pipeHandle) {
        close(((int*)pipeHandle)[0]);
        close(((int*)pipeHandle)[1]);
        delete[] (int*)pipeHandle;
        pipeHandle = nullptr;
    }
}

bool ChessEngine::writeToEngine(const std::string& command) {
    if (!pipeHandle) return false;
    
    std::string cmd = command + "\n";
    ssize_t written = write(((int*)pipeHandle)[1], cmd.c_str(), cmd.length());
    return written == (ssize_t)cmd.length();
}

std::string ChessEngine::readFromEngine(int timeoutMs) {
    if (!pipeHandle) return "";
    
    std::string result;
    char buffer[4096];
    
    auto startTime = std::chrono::steady_clock::now();
    bool bestmoveFound = false;
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::steady_clock::now() - startTime).count() < timeoutMs && !bestmoveFound) {
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(((int*)pipeHandle)[0], &readfds);
        
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
        
        if (select(((int*)pipeHandle)[0] + 1, &readfds, NULL, NULL, &tv) > 0) {
            ssize_t bytesRead = read(((int*)pipeHandle)[0], buffer, sizeof(buffer)-1);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                result += buffer;
                
                // Check if we've received a complete response
                if (result.find("bestmove") != std::string::npos) {
                    bestmoveFound = true;
                }
            }
        }
    }
    
    return result;
}
#endif

bool ChessEngine::start() {
    if (isRunning) return true;
    
    if (!openPipe()) {
        return false;
    }
    
    // Initialize Stockfish
    writeToEngine("uci");
    std::string response = readFromEngine(2000);
    
    if (response.find("id name Stockfish") != std::string::npos) {
        isRunning = true;
        writeToEngine("isready");
        readFromEngine(1000);
        return true;
    }
    
    closePipe();
    return false;
}

void ChessEngine::stop() {
    if (isRunning) {
        writeToEngine("quit");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        closePipe();
        isRunning = false;
    }
}

std::string ChessEngine::sendCommand(const std::string& command) {
    if (!isRunning) return "";
    
    writeToEngine(command);
    return readFromEngine(1000);
}

bool ChessEngine::setPosition(const std::string& fen) {
    if (!isRunning) return false;
    
    writeToEngine("position fen " + fen);
    return true;
}

std::vector<std::string> ChessEngine::getLegalMoves(const std::string& fen) {
    std::vector<std::string> moves;
    if (!isRunning) return moves;
    
    // Set the position
    setPosition(fen);
    
    // Use the "go perft 1" command which is the most reliable way to get all legal moves
    writeToEngine("go perft 1");
    std::string response = readFromEngine(2000);
    
    // Split into lines
    std::istringstream iss(response);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Look for lines that contain moves (they have a colon and a number)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            // Extract the move part (before the colon)
            std::string move = line.substr(0, colonPos);
            
            // Trim whitespace
            move.erase(0, move.find_first_not_of(" \t\n\r"));
            move.erase(move.find_last_not_of(" \t\n\r") + 1);
            
            // Valid UCI moves are 4-5 characters and contain only alphanumeric chars
            if (move.length() >= 4 && move.length() <= 5) {
                bool valid = true;
                for (char c : move) {
                    if (!isalnum(c)) {
                        valid = false;
                        break;
                    }
                }
                if (valid) {
                    moves.push_back(move);
                }
            }
        }
    }
    
    // If we didn't get any moves, try a different approach - ask for best move
    if (moves.empty()) {
        writeToEngine("go depth 1");
        response = readFromEngine(1000);
        
        // Look for bestmove
        size_t bestmovePos = response.find("bestmove");
        if (bestmovePos != std::string::npos) {
            size_t start = response.find(' ', bestmovePos) + 1;
            size_t end = response.find(' ', start);
            if (end == std::string::npos) {
                end = response.find('\n', start);
            }
            if (end != std::string::npos && start < end) {
                std::string bestMove = response.substr(start, end - start);
                if (bestMove != "(none)") {
                    moves.push_back(bestMove);
                }
            }
        }
    }
    
    return moves;
}

bool ChessEngine::isValidMove(const std::string& fen, const std::string& move) {
    auto legalMoves = getLegalMoves(fen);
    
    // Convert both to lowercase for comparison
    std::string lowerMove = move;
    std::transform(lowerMove.begin(), lowerMove.end(), lowerMove.begin(), ::tolower);
    
    for (const auto& legalMove : legalMoves) {
        std::string lowerLegal = legalMove;
        std::transform(lowerLegal.begin(), lowerLegal.end(), lowerLegal.begin(), ::tolower);
        
        if (lowerLegal == lowerMove) {
            return true;
        }
    }
    
    return false;
}

std::string ChessEngine::getBestMove(const std::string& fen, int moveTimeMs) {
    if (!isRunning) return "";
    
    setPosition(fen);
    writeToEngine("go movetime " + std::to_string(moveTimeMs));
    
    std::string response = readFromEngine(moveTimeMs + 500);
    
    // Parse bestmove from response
    size_t bestmovePos = response.find("bestmove");
    if (bestmovePos != std::string::npos) {
        size_t start = response.find(' ', bestmovePos) + 1;
        size_t end = response.find(' ', start);
        if (end == std::string::npos) {
            end = response.find('\n', start);
        }
        
        if (end != std::string::npos && start < end) {
            std::string bestMove = response.substr(start, end - start);
            if (bestMove != "(none)") {
                return bestMove;
            }
        }
    }
    
    return "";
}

std::string ChessEngine::analyzePosition(const std::string& fen, int depth) {
    if (!isRunning) return "";
    
    setPosition(fen);
    writeToEngine("go depth " + std::to_string(depth));
    
    return readFromEngine(10000);
}

std::string ChessEngine::makeMove(const std::string& fen, const std::string& move) {
    if (!isRunning || !isValidMove(fen, move)) {
        return fen;
    }
    
    // Set the position and make the move
    writeToEngine("position fen " + fen + " moves " + move);
    
    // Now get the new FEN
    writeToEngine("d");
    std::string response = readFromEngine(500);
    
    // Look for FEN in the response
    size_t fenPos = response.find("Fen: ");
    if (fenPos == std::string::npos) {
        fenPos = response.find("FEN: ");
    }
    
    if (fenPos != std::string::npos) {
        size_t start = response.find(':', fenPos) + 1;
        while (start < response.length() && isspace(response[start])) {
            start++;
        }
        
        size_t end = response.find('\n', start);
        if (end != std::string::npos) {
            std::string newFen = response.substr(start, end - start);
            
            // Trim whitespace
            newFen.erase(0, newFen.find_first_not_of(" \t\n\r"));
            newFen.erase(newFen.find_last_not_of(" \t\n\r") + 1);
            
            if (!newFen.empty() && newFen.find('/') != std::string::npos) {
                return newFen;
            }
        }
    }
    
    return fen;
}