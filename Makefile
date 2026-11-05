EXE := .exe # swap to `EXE := ` for linux

CXX := g++
CC := gcc

CXXFLAGS := -std=c++20
CFLAGS := -std=c17

BIN_DIR := bin
SRC_DIR := src

INCLUDE_DIRS := C:/msys64/ucrt64/include C:/glfw-3.4/include cgame/include include
INCLUDE := $(addprefix -I,$(INCLUDE_DIRS))

LIBS := -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32

rwildcard = $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2)) $(wildcard $1/$2)

CPP_SOURCES := $(call rwildcard,$(SRC_DIR),*.cpp)
C_SOURCES   := $(call rwildcard,$(SRC_DIR),*.c)

CPP_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(CPP_SOURCES))
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(C_SOURCES))

OBJECTS := $(CPP_OBJECTS) $(C_OBJECTS)

CGAME_DIR := cgame
CGAME_LIB := $(CGAME_DIR)/bin/libcgame.a

TARGET := $(BIN_DIR)/main$(EXE)

.PHONY: all clean $(CGAME_LIB)

all: $(TARGET)

# compile main.cpp to main.exe
$(TARGET): $(OBJECTS) $(CGAME_LIB) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@ $(LIBS)

$(CGAME_LIB):
	$(MAKE) -C $(CGAME_DIR)

# compile all cpp files to object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# compile all c files to object files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)