CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
LDFLAGS = -pthread

TARGET = dgemv
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)