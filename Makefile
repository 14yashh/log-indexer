# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = .

# Target executable
TARGET = $(BIN_DIR)/log_indexer

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/index-builder.c \
       $(SRC_DIR)/query.c \
       $(SRC_DIR)/cli.c

# Object files
OBJS = $(BUILD_DIR)/main.o \
       $(BUILD_DIR)/index-builder.o \
       $(BUILD_DIR)/query.o \
       $(BUILD_DIR)/cli.o

# Header files
HEADERS = $(INC_DIR)/index.h \
          $(INC_DIR)/cli.h

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete! Executable: $(TARGET)"

# Compile source files to object files
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/index-builder.o: $(SRC_DIR)/index-builder.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/index-builder.c -o $(BUILD_DIR)/index-builder.o

$(BUILD_DIR)/query.o: $(SRC_DIR)/query.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/query.c -o $(BUILD_DIR)/query.o

$(BUILD_DIR)/cli.o: $(SRC_DIR)/cli.c $(HEADERS)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/cli.c -o $(BUILD_DIR)/cli.o

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete!"

# Clean and rebuild
rebuild: clean all

# Run tests
test: $(TARGET)
	@echo "Building index..."
	./$(TARGET) build sample.log logs.idx
	@echo ""
	@echo "Running stats..."
	./$(TARGET) stats logs.idx
	@echo ""
	@echo "Querying ERROR logs..."
	./$(TARGET) query-level sample.log logs.idx ERROR

# Run interactive query mode
run: $(TARGET)
	./$(TARGET) query sample.log logs.idx

# Install (optional - copy to /usr/local/bin)
install: $(TARGET)
	@echo "Installing to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete!"

# Uninstall
uninstall:
	@echo "Removing from /usr/local/bin..."
	sudo rm -f /usr/local/bin/log_indexer
	@echo "Uninstall complete!"

# Help
help:
	@echo "Log File Indexer - Makefile Targets:"
	@echo ""
	@echo "Build Targets:"
	@echo "  make          - Build the project"
	@echo "  make all      - Build the project (same as make)"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make rebuild  - Clean and rebuild"
	@echo ""
	@echo "Run Targets:"
	@echo "  make run      - Run interactive query mode"
	@echo "  make test     - Build index and run basic tests"
	@echo ""
	@echo "Install Targets:"
	@echo "  make install  - Install to /usr/local/bin (requires sudo)"
	@echo "  make uninstall- Remove from /usr/local/bin (requires sudo)"
	@echo ""
	@echo "  make help     - Show this help message"

.PHONY: all clean rebuild test run install uninstall help