# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Files
SRC = main.cpp  # Add all your .cpp source files here
OBJ = $(SRC:.cpp=.o)
DEPS = CommentRemove.h  # Add all your .h header files here

# Output
TARGET = main  # The name of your final executable

# Default target
all: $(TARGET)

# Rule to link object files and create the final executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the directory (remove object files and executable)
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean
