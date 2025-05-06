###############################################################################
# Makefile for RayTracer Project
#
# - Object files are placed in "./build/obj/"
# - Main executable is placed in "./build/release/"
# - Test runner is placed in "./build/tests/"
# - "make tests" compiles and runs all tests.
###############################################################################

# Compiler settings
CXX      := g++
CXXFLAGS := -Wall -std=c++17 -Iinclude -Ilib -pthread -O3

# Directories
OBJ_DIR      := build/obj
RELEASE_DIR  := build/release
TESTS_DIR    := build/tests
OUT_DIR      := outputs

# Executables
TARGET    := $(RELEASE_DIR)/raytracer
TEST_BIN  := $(TESTS_DIR)/test_runner

# Source files
SRC_FILES    := $(wildcard src/*.cpp)
LIB_FILES    := $(wildcard lib/*.cpp)
TEST_SRC     := $(wildcard tests/*.cpp)

# Object files
SRC_OBJ  := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LIB_OBJ  := $(patsubst lib/%.cpp,$(OBJ_DIR)/%.o,$(LIB_FILES))
TEST_OBJ := $(patsubst tests/%.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC))

###############################################################################
# Default target: build main executable only
###############################################################################
all: $(TARGET)

###############################################################################
# Build main raytracer executable
###############################################################################
$(TARGET): $(RELEASE_DIR) $(SRC_OBJ) $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) $(SRC_OBJ) $(LIB_OBJ) -o $@

###############################################################################
# Build test runner binary
###############################################################################
$(TEST_BIN): $(TESTS_DIR) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(TEST_OBJ) -o $@

###############################################################################
# Object file rules
###############################################################################
$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: lib/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: tests/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

###############################################################################
# Directory creation rules
###############################################################################
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(RELEASE_DIR):
	mkdir -p $(RELEASE_DIR)

$(TESTS_DIR):
	mkdir -p $(TESTS_DIR)

###############################################################################
# Clean only object files
###############################################################################
clean_obj:
	rm -rf $(OBJ_DIR)/*

###############################################################################
# Clean output images
###############################################################################
clean_outputs:
	rm -rf $(OUT_DIR)

###############################################################################
# Clean all build files
###############################################################################
clean: clean_obj clean_outputs
	rm -rf $(TARGET) $(TEST_BIN) $(RELEASE_DIR) $(TESTS_DIR) build

###############################################################################
# Run main executable
###############################################################################
run: $(TARGET)
	./$(TARGET)

###############################################################################
# Run tests
###############################################################################
tests: $(TEST_BIN) $(TARGET)
	@echo "Running system tests..."
	@./$(TEST_BIN)

.PHONY: all clean clean_obj clean_outputs run tests
