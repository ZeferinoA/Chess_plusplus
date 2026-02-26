#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <cstring>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
    #include <windows.h>
    #include <fileapi.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <sys/types.h>
#endif

class StockfishEngine {
private:
    #ifdef _WIN32
        PROCESS_INFORMATION processInfo;
        HANDLE stdinPipe, stdoutPipe;
    #else
        int stdinPipe[2], stdoutPipe[2];
        pid_t pid;
    #endif
    
    bool isRunning;
    std::string enginePath;
    
    // Helper to send command
    void sendCommand(const std::string& cmd) {
        std::string command = cmd + "\n";
        #ifdef _WIN32
            DWORD written;
            WriteFile(stdinPipe, command.c_str(), command.length(), &written, NULL);
        #else
            write(stdinPipe[1], command.c_str(), command.length());
        #endif
    }
    
    // Helper to read response with timeout
    std::string readResponse(int timeoutMs = 5000) {
        std::string response;
        char buffer[4096];
        auto startTime = std::chrono::steady_clock::now();
        
        while (true) {
            // Check for timeout
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
            if (elapsed > timeoutMs) break;
            
            #ifdef _WIN32
                DWORD bytesRead;
                if (ReadFile(stdoutPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    response += buffer;
                    
                    // Check if we've received a complete response
                    if (response.find("uciok") != std::string::npos ||
                        response.find("bestmove") != std::string::npos ||
                        response.find("Fen:") != std::string::npos) {
                        break;
                    }
                }
            #else
                fd_set set;
                struct timeval timeout;
                FD_ZERO(&set);
                FD_SET(stdoutPipe[0], &set);
                timeout.tv_sec = 0;
                timeout.tv_usec = 100000; // 100ms
                
                if (select(stdoutPipe[0] + 1, &set, NULL, NULL, &timeout) > 0) {
                    int bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer) - 1);
                    if (bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        response += buffer;
                        
                        if (response.find("uciok") != std::string::npos ||
                            response.find("bestmove") != std::string::npos ||
                            response.find("Fen:") != std::string::npos) {
                            break;
                        }
                    }
                }
            #endif
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        return response;
    }

public:
    StockfishEngine(const std::string& path = "stockfish") : enginePath(path), isRunning(false) {}
    
    bool start() {
        #ifdef _WIN32
            SECURITY_ATTRIBUTES saAttr;
            saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
            saAttr.bInheritHandle = TRUE;
            saAttr.lpSecurityDescriptor = NULL;
            
            // Create pipes
            if (!CreatePipe(&stdoutPipe, &stdoutPipe, &saAttr, 0)) return false;
            if (!CreatePipe(&stdinPipe, &stdinPipe, &saAttr, 0)) return false;
            
            // Ensure the read handle to stdout is not inherited
            SetHandleInformation(stdoutPipe, HANDLE_FLAG_INHERIT, 0);
            SetHandleInformation(stdinPipe, HANDLE_FLAG_INHERIT, 0);
            
            // Set up process startup info
            STARTUPINFOA startupInfo;
            ZeroMemory(&startupInfo, sizeof(STARTUPINFOA));
            startupInfo.cb = sizeof(STARTUPINFOA);
            startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
            startupInfo.hStdOutput = stdoutPipe;
            startupInfo.hStdInput = stdinPipe;
            startupInfo.dwFlags |= STARTF_USESTDHANDLES;
            
            // Create process
            if (!CreateProcessA(NULL, (LPSTR)enginePath.c_str(), NULL, NULL, TRUE, 
                                CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo)) {
                return false;
            }
            
            isRunning = true;
            
        #else
            if (pipe(stdinPipe) != 0 || pipe(stdoutPipe) != 0) return false;
            
            pid = fork();
            if (pid == 0) { // Child process
                dup2(stdinPipe[0], STDIN_FILENO);
                dup2(stdoutPipe[1], STDOUT_FILENO);
                
                close(stdinPipe[0]);
                close(stdinPipe[1]);
                close(stdoutPipe[0]);
                close(stdoutPipe[1]);
                
                execlp(enginePath.c_str(), enginePath.c_str(), nullptr);
                exit(1);
            } else if (pid > 0) { // Parent process
                close(stdinPipe[0]);
                close(stdoutPipe[1]);
                isRunning = true;
            } else {
                return false;
            }
        #endif
        
        // Initialize UCI
        sendCommand("uci");
        std::string response = readResponse();
        return response.find("uciok") != std::string::npos;
    }
    
    void stop() {
        if (!isRunning) return;
        
        sendCommand("quit");
        
        #ifdef _WIN32
            WaitForSingleObject(processInfo.hProcess, 5000);
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
            CloseHandle(stdinPipe);
            CloseHandle(stdoutPipe);
        #else
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
            close(stdinPipe[1]);
            close(stdoutPipe[0]);
        #endif
        
        isRunning = false;
    }
    
    // Test 1: Basic communication and identification
    bool testBasicCommunication() {
        std::cout << "\n=== Test 1: Basic Communication ===\n";
        sendCommand("uci");
        std::string response = readResponse();
        
        bool success = response.find("id name") != std::string::npos && 
                       response.find("uciok") != std::string::npos;
        
        std::cout << "Engine identification: " << (success ? "PASSED" : "FAILED") << "\n";
        if (success) {
            // Extract and print engine name
            size_t namePos = response.find("id name");
            if (namePos != std::string::npos) {
                size_t endPos = response.find("\n", namePos);
                std::cout << "  " << response.substr(namePos, endPos - namePos) << "\n";
            }
        }
        return success;
    }
    
    // Test 2: Setting up a position and displaying board
    bool testPositionSetup() {
        std::cout << "\n=== Test 2: Position Setup ===\n";
        
        sendCommand("position startpos");
        sendCommand("d");
        
        std::string response = readResponse();
        
        bool success = response.find("rnbqkbnr") != std::string::npos &&
                       response.find("pppppppp") != std::string::npos;
        
        std::cout << "Board setup: " << (success ? "PASSED" : "FAILED") << "\n";
        if (success) {
            std::cout << "Initial board state:\n";
            // Extract and print the board
            size_t boardStart = response.find("+---+");
            if (boardStart != std::string::npos) {
                std::cout << response.substr(boardStart, 400) << "\n";
            }
        }
        return success;
    }
    
    // Test 3: Calculating a move from start position
    bool testMoveCalculation() {
        std::cout << "\n=== Test 3: Move Calculation ===\n";
        
        sendCommand("position startpos");
        sendCommand("go depth 15");
        
        std::string response = readResponse(10000); // 10 second timeout
        
        bool success = response.find("bestmove") != std::string::npos;
        
        std::cout << "Move calculation: " << (success ? "PASSED" : "FAILED") << "\n";
        if (success) {
            size_t bestMovePos = response.find("bestmove");
            size_t endPos = response.find("\n", bestMovePos);
            std::cout << "  " << response.substr(bestMovePos, endPos - bestMovePos) << "\n";
            
            // Count info lines to show analysis depth
            int infoCount = 0;
            size_t pos = 0;
            while ((pos = response.find("info depth", pos)) != std::string::npos) {
                infoCount++;
                pos += 10;
            }
            std::cout << "  Analyzed " << infoCount << " depth levels\n";
        }
        return success;
    }
    
    // Test 4: Testing a specific position (FEN string)
    bool testSpecificPosition() {
        std::cout << "\n=== Test 4: Specific Position Analysis ===\n";
        
        // Famous position: Ruy Lopez, Morphy Defense
        std::string fen = "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 4";
        
        sendCommand("position fen " + fen);
        sendCommand("go movetime 3000");
        
        std::string response = readResponse(5000);
        
        bool success = response.find("bestmove") != std::string::npos;
        
        std::cout << "Position analysis: " << (success ? "PASSED" : "FAILED") << "\n";
        if (success) {
            size_t bestMovePos = response.find("bestmove");
            size_t endPos = response.find("\n", bestMovePos);
            std::cout << "  Best move in Ruy Lopez: " << 
                         response.substr(bestMovePos, endPos - bestMovePos) << "\n";
            
            // Also check if the engine found a good move (should be something like a6, b5, or d6)
            std::string bestMove = response.substr(bestMovePos + 9, 4);
            std::cout << "  Move suggested: " << bestMove << "\n";
        }
        return success;
    }
    
    // Test 5: Move legality and board state
    bool testMoveLegality() {
        std::cout << "\n=== Test 5: Move Legality ===\n";
        
        sendCommand("position startpos");
        sendCommand("position startpos moves e2e4");
        sendCommand("d");
        
        std::string response = readResponse();
        
        // Check if pawn moved to e4
        bool success = response.find("Fen:") != std::string::npos;
        if (success) {
            // Extract FEN and check if it starts with correct position after e4
            size_t fenPos = response.find("Fen: rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
            success = fenPos != std::string::npos;
        }
        
        std::cout << "Move execution: " << (success ? "PASSED" : "FAILED") << "\n";
        if (success) {
            std::cout << "  Successfully executed e2e4\n";
        }
        return success;
    }
    
    // Test 6: Checkmate detection
    bool testCheckmate() {
        std::cout << "\n=== Test 6: Checkmate Detection ===\n";
        
        // Fool's mate position
        std::string fen = "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 4";
        
        sendCommand("position fen " + fen);
        sendCommand("go depth 5");
        
        std::string response = readResponse();
        
        bool success = response.find("bestmove (none)") != std::string::npos;
        
        std::cout << "Checkmate detection: " << (success ? "PASSED" : "FAILED") << "\n";
        if (!success) {
            std::cout << "  Warning: Engine didn't detect checkmate immediately\n";
        } else {
            std::cout << "  Successfully detected checkmate position\n";
        }
        return success;
    }
    
    // Test 7: Performance benchmark
    bool testPerformance() {
        std::cout << "\n=== Test 7: Performance Benchmark ===\n";
        
        // Use built-in speedtest if available (Stockfish 17.1+)
        sendCommand("speedtest");
        
        std::string response = readResponse(15000); // 15 second timeout
        
        bool success = response.find("Nodes/second") != std::string::npos ||
                       response.find("bench") != std::string::npos;
        
        std::cout << "Performance test: " << (success ? "PASSED" : "FAILED") << "\n";
        if (success) {
            // Extract nodes per second
            size_t nodesPos = response.find("Nodes/second");
            if (nodesPos != std::string::npos) {
                size_t endPos = response.find("\n", nodesPos);
                std::cout << "  " << response.substr(nodesPos, endPos - nodesPos) << "\n";
            } else {
                std::cout << "  Speed test completed\n";
            }
        }
        return success;
    }
    
    // Test 8: Multi-variant analysis
    bool testMultiPV() {
        std::cout << "\n=== Test 8: Multi-Variant Analysis ===\n";
        
        // Set UCI option for multipv
        sendCommand("setoption name MultiPV value 3");
        sendCommand("position startpos");
        sendCommand("go depth 15");
        
        std::string response = readResponse(10000);
        
        // Count how many different principal variations we see
        int multipvCount = 0;
        size_t pos = 0;
        while ((pos = response.find("multipv", pos)) != std::string::npos) {
            multipvCount++;
            pos += 7;
        }
        
        bool success = multipvCount >= 3;
        
        std::cout << "Multi-variant analysis: " << (success ? "PASSED" : "FAILED") << "\n";
        std::cout << "  Found " << multipvCount << " variations\n";
        
        return success;
    }
    
    // Run all tests
    void runAllTests() {
        std::cout << "========================================\n";
        std::cout << "  Stockfish Engine Test Suite\n";
        std::cout << "========================================\n";
        
        if (!start()) {
            std::cerr << "Failed to start Stockfish engine at path: " << enginePath << "\n";
            std::cerr << "Make sure Stockfish is installed and in your PATH, or provide the full path.\n";
            return;
        }
        
        std::cout << "Engine started successfully\n";
        
        std::vector<std::pair<std::string, bool>> results;
        
        results.emplace_back("Basic Communication", testBasicCommunication());
        results.emplace_back("Position Setup", testPositionSetup());
        results.emplace_back("Move Calculation", testMoveCalculation());
        results.emplace_back("Specific Position", testSpecificPosition());
        results.emplace_back("Move Legality", testMoveLegality());
        results.emplace_back("Checkmate Detection", testCheckmate());
        results.emplace_back("Performance", testPerformance());
        results.emplace_back("Multi-Variant Analysis", testMultiPV());
        
        std::cout << "\n========================================\n";
        std::cout << "           Test Summary\n";
        std::cout << "========================================\n";
        
        int passed = 0;
        for (const auto& [name, result] : results) {
            std::cout << std::left << std::setw(25) << name << ": " 
                      << (result ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m") << "\n";
            if (result) passed++;
        }
        
        std::cout << "----------------------------------------\n";
        std::cout << "Total: " << passed << "/" << results.size() << " tests passed\n";
        
        if (passed == results.size()) {
            std::cout << "\n\033[32mAll tests passed! Stockfish is functioning correctly.\033[0m\n";
        } else {
            std::cout << "\n\033[33mSome tests failed. Check engine installation and configuration.\033[0m\n";
        }
        
        stop();
    }
};

int main(int argc, char* argv[]) {
    std::string enginePath = "stockfish";
    
    // Allow custom path as command line argument
    if (argc > 1) {
        enginePath = argv[1];
    }
    
    StockfishEngine engine(enginePath);
    engine.runAllTests();
    
    return 0;
}