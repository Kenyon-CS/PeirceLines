# Makefile

# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

# Target executable name
TARGET = PeirceSim

# Source files
SRCS = PeirceSimulation.cpp

# Default rule to build the target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Rule to clean the build (remove the executable)
clean:
	rm -f $(TARGET)
