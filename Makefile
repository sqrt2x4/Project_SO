# Name of the executable
TARGET = treasure_manager

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

# Source files
SRC = treasure_manager.c

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET) $(ARGS)

# Clean up build artifacts
clean:
	rm -f $(TARGET)
