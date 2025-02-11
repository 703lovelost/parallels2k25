#include <iostream>
#include <cmath>
#include <vector>

const size_t ARRAY_SIZE = 10000000;

template <typename T>
T calculateSineSum() {
    T sum = 0;

    std::vector<T> sine_array = {}; 

    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        T sine = std::sin(static_cast<T>(i * 2 * M_PI / ARRAY_SIZE));
        sine_array.push_back(sine);

        sum += sine;
    }

    return sum;
}

int main() {
#ifdef USE_FLOAT
    using DataType = float;
#else
    using DataType = double;
#endif

    DataType sum = calculateSineSum<DataType>();
    std::cout << "Sum of sine values: " << sum << std::endl;

    return 0;
}