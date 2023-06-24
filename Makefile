# Makefile

# Compiler options
CC = gcc
CFLAGS = -Iinclude
LDFLAGS = -Llib -lcsfml-window -lcsfml-graphics -lcsfml-audio -lcsfml-system

# Source files
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/screens/*.c)

# Output directory
BINDIR = bin
OUTPUT = $(BINDIR)/monopoly.exe

# Targets
all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

.PHONY: all clean