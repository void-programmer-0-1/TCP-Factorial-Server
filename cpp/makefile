# ---------------- Compiler ----------------
CXX := g++

# ---------------- Flags ----------------
CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wpedantic -pthread
LDFLAGS  :=

# ---------------- Target ----------------
TARGET := factorial_server

# ---------------- Sources ----------------
SRCS := server.cpp
HDRS := big_factorial.hpp thread_pool.hpp

# ---------------- Default ----------------
all: $(TARGET)

# ---------------- Build ----------------
$(TARGET): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# ---------------- Clean ----------------
clean:
	rm -f $(TARGET)

# ---------------- Rebuild ----------------
rebuild: clean all

# ---------------- Run ----------------
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run
