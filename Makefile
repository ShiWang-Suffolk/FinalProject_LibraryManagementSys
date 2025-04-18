# Makefile for Library Management System

CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
LDFLAGS = -lsqlite3

SRCS = main.cpp core.cpp ui.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = ls

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
