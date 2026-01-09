# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Output executable
TARGET = shell.out

# Default rule
all: $(TARGET)

# Compile executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Run (optional)
run: $(TARGET)
	./$(TARGET)
