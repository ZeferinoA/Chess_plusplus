# Super Simple Makefile with Debug
CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = chess_game
SOURCES = main.cpp Board.cpp Tutorial.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default build (release)
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp Board.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS += -g -O0
debug: clean $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: debug clean
