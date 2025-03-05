CXX = g++
CXXFLAGS = -fopenmp -std=c++17 -O2

FILENAME = main.cpp
TARGET = dgemv

all: $(TARGET)

$(TARGET): $(FILENAME)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(FILENAME)

clean:
	rm -f $(TARGET)