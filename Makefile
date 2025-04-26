# Makefile for Library Management System
# Objects are placed in a separate build/ directory

# Compilers
CXX       = g++
CC        = gcc

# Flags
CXXFLAGS  = -std=c++17 -Wall -Wextra -pedantic -g -MMD -MP
CFLAGS    = -Wall -Wextra -pedantic -g -MMD -MP

# Include dirs
INCLUDES  = -Iheaders

# Libraries (note FLTK link order)
LDFLAGS   = -lfltk_images -lfltk_forms -lfltk -lsqlite3

# Source files
CXX_SRCS  = sources/main.cpp sources/core.cpp sources/ui.cpp
C_SRCS    = sources/sqlite3.c

# Object directory
OBJDIR    = build

# Object files
CXX_OBJS  = $(patsubst sources/%.cpp,$(OBJDIR)/%.o,$(CXX_SRCS))
C_OBJS    = $(patsubst sources/%.c,$(OBJDIR)/%.o,$(C_SRCS))
OBJS      = $(CXX_OBJS) $(C_OBJS)

# Dependency files
DEPS      = $(OBJS:.o=.d)

# Final executable
TARGET    = app

.PHONY: all clean run

all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(OBJS) $(LDFLAGS)

# Run
run: $(TARGET)
	./$(TARGET)

# Include generated dependency files
-include $(DEPS)

# Ensure build directory exists
$(OBJDIR):
	mkdir -p $@

# Compile C++ sources into build/
$(OBJDIR)/%.o: sources/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C sources into build/
$(OBJDIR)/%.o: sources/%.c | $(OBJDIR)
	$(CC)  $(CFLAGS)  $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET)
