CXX = g++
CXXFLAGS = -fopenmp -std=c++17 -O2


FILENAME_1 = main_1.cpp
FILENAME_2 = main_2.cpp
TARGET_1 = dgemv_whole
TARGET_2 = dgemv_fors

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): $(FILENAME_1)
	$(CXX) $(CXXFLAGS) -o $(TARGET_1) $(FILENAME_1)

$(TARGET_2): $(FILENAME_2)
	$(CXX) $(CXXFLAGS) -o $(TARGET_2) $(FILENAME_2)

clean:
	rm -f $(TARGET_1) $(TARGET_2)