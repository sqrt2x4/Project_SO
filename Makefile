# Makefile for Treasure Hunt Phase 3

# Executables
TARGET_HUB = treasure_hub
TARGET_MONITOR = monitor
TARGET_SCORE = score_calculator

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

# Source files
SRC_HUB = treasure_hub.c
SRC_MONITOR = monitor.c
SRC_SCORE = score_calculator.c

# Default target
all: $(TARGET_HUB) $(TARGET_MONITOR) $(TARGET_SCORE)

$(TARGET_HUB): $(SRC_HUB)
	$(CC) $(CFLAGS) $(SRC_HUB) -o $(TARGET_HUB) -lncurses

$(TARGET_MONITOR): $(SRC_MONITOR)
	$(CC) $(CFLAGS) $(SRC_MONITOR) -o $(TARGET_MONITOR)

$(TARGET_SCORE): $(SRC_SCORE)
	$(CC) $(CFLAGS) $(SRC_SCORE) -o $(TARGET_SCORE)

run: all
	./$(TARGET_HUB)

clean:
	rm -f $(TARGET_HUB) $(TARGET_MONITOR) $(TARGET_SCORE)
