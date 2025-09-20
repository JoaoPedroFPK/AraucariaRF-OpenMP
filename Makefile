# Makefile for AraucariaRF-OpenMP Project

# Compiler settings
CC = gcc
CXX = g++

# Detect OS and set appropriate OpenMP flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS with Homebrew OpenMP
    OPENMP_FLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/include
    OPENMP_LIBS = -L/opt/homebrew/lib -lomp
else
    # Linux with standard OpenMP
    OPENMP_FLAGS = -fopenmp
    OPENMP_LIBS = -fopenmp
endif

CFLAGS = -Wall -Wextra -O3 $(OPENMP_FLAGS) -std=c99
CXXFLAGS = -Wall -Wextra -O3 $(OPENMP_FLAGS) -std=c++17
LDFLAGS = $(OPENMP_LIBS) -lm

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Create directories if they don't exist
$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

# Source files
SEQUENTIAL_SOURCES = $(wildcard $(SRC_DIR)/sequential/*.c)
PARALLEL_SOURCES = $(wildcard $(SRC_DIR)/parallel/*.c)
UTILS_SOURCES = $(wildcard $(SRC_DIR)/utils/*.c)

# Object files
SEQUENTIAL_OBJECTS = $(SEQUENTIAL_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
PARALLEL_OBJECTS = $(PARALLEL_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
UTILS_OBJECTS = $(UTILS_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Executables
SEQUENTIAL_TARGET = $(BIN_DIR)/rf_sequential
PARALLEL_TARGET = $(BIN_DIR)/rf_parallel

# Default target
all: $(SEQUENTIAL_TARGET) $(PARALLEL_TARGET)

# Sequential version (without OpenMP)
$(SEQUENTIAL_TARGET): $(SEQUENTIAL_OBJECTS) $(UTILS_OBJECTS)
	$(CC) $(SEQUENTIAL_OBJECTS) $(UTILS_OBJECTS) -o $@ -lm

# Parallel version
$(PARALLEL_TARGET): $(PARALLEL_OBJECTS) $(UTILS_OBJECTS)
	$(CC) $(PARALLEL_OBJECTS) $(UTILS_OBJECTS) -o $@ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile sequential files without OpenMP
$(BUILD_DIR)/sequential/%.o: $(SRC_DIR)/sequential/%.c
	@mkdir -p $(dir $@)
	$(CC) -Wall -Wextra -O3 -std=c99 -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/utils/%.o: $(SRC_DIR)/utils/%.c
	@mkdir -p $(dir $@)
	$(CC) -Wall -Wextra -O3 -std=c99 -I$(INCLUDE_DIR) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Performance testing
test-performance: $(PARALLEL_TARGET) $(SEQUENTIAL_TARGET)
	@echo "Running performance tests..."
	./scripts/benchmark/run_performance_tests.sh

# VTune profiling
profile: $(PARALLEL_TARGET)
	@echo "Running VTune profiling..."
	./scripts/vtune/run_vtune_analysis.sh

# Install dependencies (if needed)
install-deps:
	@echo "Installing dependencies..."
	# Add dependency installation commands here

.PHONY: all clean test-performance profile install-deps
