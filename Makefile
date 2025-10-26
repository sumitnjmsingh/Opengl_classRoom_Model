# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Include directories
INCLUDES = -Iinclude

# Library directories and libraries
LIBS = -lGL -lGLEW -lglfw -lm

# Source and build directories
SRCDIR = src
BUILDDIR = build
INCDIR = include

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Target executable
TARGET = $(BUILDDIR)/classroom

# Default target
all: $(TARGET)

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Link the executable
$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILDDIR)/*.o $(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential cmake
	sudo apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
	sudo apt-get install -y libglew-dev libglfw3-dev
	sudo apt-get install -y libglm-dev

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all         - Build the project (default)"
	@echo "  clean       - Remove build files"
	@echo "  install-deps - Install required dependencies"
	@echo "  run         - Build and run the program"
	@echo "  debug       - Build with debug symbols"
	@echo "  help        - Show this help message"

.PHONY: all clean install-deps run debug help