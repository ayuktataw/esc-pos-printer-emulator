#====================================================================
# Project Makefile
# ===================================================================

# Compiler
CC := gcc


# Directories
PROJECT_DIR := .
LIB_DIR := $(PROJECT_DIR)/lib
INC_DIR := $(PROJECT_DIR)/include
SRC_DIR := $(PROJECT_DIR)/src
BUILD_DIR := $(PROJECT_DIR)/build
# Library search path 
LDFLAGS := -L$(LIB_DIR)
#Target executable
TARGET := emulator
# Libraries to link
#
# libBitBmp -> -lBitBmp etc...
LDLIBS := 

#Runtime shared library path
#
#This embeds ./lib as an rpath so the executable can 
#locate the .so files at runtime
RPATH := -Wl,-rpath,'$$ORIGIN/lib'

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Include paths
CFLAGS := -Wall -Wextra -I$(INC_DIR) 

#==============================================================================
# Default target
# =============================================================================
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

#==============================================================================
# Link executable
# =============================================================================
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS) $(RPATH)

#==============================================================================
#Compile .c files into .o files
#==============================================================================
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

#==============================================================================
#Run the program
#==============================================================================
run: $(TARGET)
	./$(TARGET) > /dev/null

#==============================================================================
#Clean the build files
#==============================================================================
clean:
	rm -f $(PROJECT_DIR)/build/$(OBJECTS) $(TARGET)
info:
	@echo "SOURCES: $(SOURCES)"
	@echo "OBJECTS: $(OBJECTS)"
	@echo "TARGET: $(TARGET)"
.PHONY: all run clean info

