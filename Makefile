# Makefile for linked-list-insertion-sort-cpp

CXX      := clang++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -pedantic
TARGET   := ll_isort
SRC      := src/main.cpp
INC      := -Iinclude

# Enable TRACE with: make run TRACE=1
ifdef TRACE
	CXXFLAGS += -DTRACE
endif

.PHONY: all run clean

all: clean $(TARGET)

$(TARGET): $(SRC) include/trace_ui.hpp
	$(CXX) $(CXXFLAGS) $(INC) $(SRC) -o $(TARGET)

run: clean $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)