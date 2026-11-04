CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic
TARGET := airports
SOURCES := $(wildcard src/*.cpp)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

test: $(TARGET)
	bash tests/run_tests.sh

clean:
	$(RM) $(TARGET)
