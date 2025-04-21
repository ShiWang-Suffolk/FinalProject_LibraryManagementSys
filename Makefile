# Makefile for Library Management System (C++ + C compatibility)

CXX = g++          # C++ Compiler
CC  = gcc          # C Compiler (for .c files like sqlite3.c)
CFLAGS = -Wall -Wextra -pedantic -g
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
LDFLAGS = -lsqlite3
INCLUDES = -Iheaders

CXX_SRCS = sources/main.cpp sources/core.cpp sources/ui.cpp
C_SRCS   = sources/sqlite3.c
SRCS = $(CXX_SRCS) $(C_SRCS)

CXX_OBJS = $(CXX_SRCS:.cpp=.o)
C_OBJS   = $(C_SRCS:.c=.o)
OBJS = $(CXX_OBJS) $(C_OBJS)

TARGET = ls

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f sources/*.o $(TARGET)

.PHONY: all clean
