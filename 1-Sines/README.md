Calculation of the sum of sines of 10^7 values, equally spaced on the `[0, 2π]` range. The script has `float` and `double` modes, allowing to set the type of values during compilation.

# Build

## Float value calculation

```
    mkdir build
    cd build
    cmake -DUSE_FLOAT=ON ..
    make
```

## Double value calculation

```
    mkdir build
    cd build
    cmake -DUSE_FLOAT=OFF .. # Or simply remove -DUSE_FLOAT argument.
    make
```

# Run

```
    ./sine_sum
```

# Reported sum values

```
    Float sum: -0.16503
    Double sum: 3.68912e-10
```