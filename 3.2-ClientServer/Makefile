CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
LDFLAGS = -pthread

TARGET = client_server
SRC = src/Main.cpp src/Client.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)