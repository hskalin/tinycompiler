# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -I./include -Wall -Wextra -std=c++17

# Source Files
SRCS = src/main.cpp src/parse.cpp src/lexer.cpp src/emit.cpp

# Object Files
OBJS = $(SRCS:.cpp=.o)

# Executable Name
TARGET = tinycompiler

# Default Rule
all: $(TARGET)

# Link Object Files to Create Executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile Source Files into Object Files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean Up Build Files
clean:
	rm -f $(OBJS)

# Phony Targets
.PHONY: all clean
