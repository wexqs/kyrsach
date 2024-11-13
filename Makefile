.PHONY: all clean format doxy test

# Compiler and flags
CXX = g++
CXXFLAGS = -O2 -Wall -DNDEBUG -std=c++17
LDFLAGS = -lcryptopp -lboost_program_options -lUnitTest++ -pthread

# Project details
TARGET = server
SOURCES = Communicate.cpp Calculator.cpp Connector.cpp Interface.cpp Logger.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Test specifics
TEST_SOURCES = Communicate.cpp Calculator.cpp Connector.cpp Interface.cpp Logger.cpp test.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TEST_TARGET = test

# Build all target
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build and run tests
test: $(TEST_OBJECTS)
	$(CXX) $(TEST_OBJECTS) $(LDFLAGS) -o $(TEST_TARGET)
	./$(TEST_TARGET)

# Format source and header files
format:
	astyle *.cpp *.h

# Run Doxygen to generate documentation
doxy:
	doxygen

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o
