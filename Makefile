# ===============================
# Project
# ===============================
TARGET := library_system.exe

# ===============================
# Directories
# ===============================
SRC_DIR      := src
INCLUDE_DIR  := include
IMGUI_DIR    := imgui
IMGUI_BACKEND:= imgui/backends
BUILD_DIR    := build
BIN_DIR      := bin
LIB_DIR      := lib

# ===============================
# Compiler
# ===============================
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra \
	-I$(INCLUDE_DIR) \
	-I$(IMGUI_DIR) \
	-I$(IMGUI_BACKEND)

# ===============================
# Libraries (GLFW + OpenGL)
# ===============================
LDFLAGS := -L$(LIB_DIR) -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32

# ===============================
# Auto-detect ALL .cpp files
# ===============================
SRC_FILES := \
	$(wildcard $(SRC_DIR)/*.cpp) \
	$(wildcard $(IMGUI_DIR)/*.cpp) \
	$(wildcard $(IMGUI_BACKEND)/imgui_impl_glfw.cpp) \
	$(wildcard $(IMGUI_BACKEND)/imgui_impl_opengl3.cpp)

# ===============================
# Object files (mirrored structure)
# ===============================
OBJ_FILES := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

# ===============================
# Default target
# ===============================
all: $(BIN_DIR)/$(TARGET)

# ===============================
# Link
# ===============================
$(BIN_DIR)/$(TARGET): $(OBJ_FILES)
	@echo Linking...
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

# ===============================
# Compile rule (auto-create folders)
# ===============================
$(BUILD_DIR)/%.o: %.cpp
	@echo Compiling $<
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===============================
# Clean
# ===============================
clean:
	@echo Cleaning...
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# ===============================
# Run
# ===============================
run: all
	./$(BIN_DIR)/$(TARGET)
