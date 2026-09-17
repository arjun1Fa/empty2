# Makefile for DebugCalc C Backend

CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2

# Detect OS
ifeq ($(OS),Windows_NT)
    TARGET = debugcalc.exe
    LDFLAGS = -lws2_32
    RM = del /Q /F
else
    TARGET = debugcalc
    LDFLAGS = -lm
    RM = rm -f
endif

SRCS = main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	$(RM) $(TARGET)

.PHONY: all clean
