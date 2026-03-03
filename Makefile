CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = 

# Source files
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/ChessEngine.cpp $(SRCDIR)/Menu.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = chess

# Test files
TESTDIR = tests
TEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp)
CATCH_SOURCE = catch2/catch_amalgamated.cpp
TEST_TARGET = test_runner

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    TEST_TARGET := $(TEST_TARGET).exe
endif

# Build main program
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ========================
# Build tests
# ========================
test:
	$(CXX) $(CXXFLAGS) \
	$(SRCDIR)/ChessEngine.cpp \
	$(SRCDIR)/Menu.cpp \
	$(TEST_SOURCES) \
	$(CATCH_SOURCE) \
	-o $(TEST_TARGET)

run-tests: test
	./$(TEST_TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run test run-tests