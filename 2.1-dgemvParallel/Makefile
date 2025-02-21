CXX = gcc
CXXFLAGS = -fopenmp

TARGET = dgemv

all: $(TARGET)

$(TARGET): main.c
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)