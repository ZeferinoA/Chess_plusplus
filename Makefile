CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = 

# Source files
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/ChessEngine.cpp $(SRCDIR)/Menu.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = chess

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    # Linux specific settings
endif
ifeq ($(UNAME_S),Darwin)
    # macOS specific settings
endif
ifeq ($(OS),Windows_NT)
    # Windows specific settings
    TARGET := $(TARGET).exe
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run