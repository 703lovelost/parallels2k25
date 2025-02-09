CXX = g++
CXXFLAGS = -Wall -O3 -std=c++17

PRECISION ?= double

ifeq ($(PRECISION), float)
    CXXFLAGS += -DUSE_FLOAT
endif

TARGET = sine_sum

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp -lm

clean:
	rm -f $(TARGET)