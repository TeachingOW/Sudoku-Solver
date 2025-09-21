# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Target executable
TARGET = sudoku_client

# Source files
SRC = sudoku_client.cpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean target
clean:
	rm -f $(TARGET)
