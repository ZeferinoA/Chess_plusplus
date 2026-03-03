#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include <string>
#include <vector>
#include <memory>

class ChessEngine {
private:
    void* pipeHandle;  // Platform-specific pipe handle
    bool isRunning;
    std::string stockfishPath;
    
    // Platform-specific methods
    bool openPipe();
    void closePipe();
    bool writeToEngine(const std::string& command);
    std::string readFromEngine(int timeoutMs = 1000);
    
public:
    ChessEngine(const std::string& path);
    ~ChessEngine();
    
    bool start();
    void stop();
    bool isEngineRunning() const { return isRunning; }
    
    std::string sendCommand(const std::string& command);
    std::string getBestMove(const std::string& fen, int moveTimeMs = 1000);
    std::string analyzePosition(const std::string& fen, int depth = 15);
    bool setPosition(const std::string& fen);
    std::vector<std::string> getLegalMoves(const std::string& fen);
    
    // New methods for move validation and FEN manipulation
    bool isValidMove(const std::string& fen, const std::string& move);
    std::string makeMove(const std::string& fen, const std::string& move);
    std::string getFenAfterMove(const std::string& fen, const std::string& move);
};

#endif // CHESS_ENGINE_H