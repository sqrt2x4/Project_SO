# Executable names
MANAGER = treasure_manager
HUB = treasure_hub
MONITOR = monitor

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

# Source files
MANAGER_SRC = treasure_manager.c
HUB_SRC = treasure_hub.c
MONITOR_SRC = monitor.c

# Build all
all: $(MANAGER) $(HUB) $(MONITOR)

# Build manager
$(MANAGER): $(MANAGER_SRC)
	$(CC) $(CFLAGS) -o $(MANAGER) $(MANAGER_SRC)

# Build hub
$(HUB): $(HUB_SRC)
	$(CC) $(CFLAGS) -o $(HUB) $(HUB_SRC) -lncurses

# Build monitor
$(MONITOR): $(MONITOR_SRC)
	$(CC) $(CFLAGS) -o $(MONITOR) $(MONITOR_SRC)

# Run the manager (example)
run_manager: $(MANAGER)
	./$(MANAGER) $(ARGS)

# Run the hub (example)
run_hub: $(HUB)
	./$(HUB)

# Clean all
clean:
	rm -f $(MANAGER) $(HUB) $(MONITOR)
