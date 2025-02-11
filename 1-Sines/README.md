Calculation of the sum of sines of 10^7 values, equally spaced on the `[0, 2π]` range. The script has `float` and `double` modes, allowing to set the type of values during compilation.

# Build

## Float value calculation

```
    make PRECISION=float
```

## Double value calculation

```
    make PRECISION=double # Or simply remove PRECISION argument.
```

# Run

```
    ./sine_sum
```

Use `make clean` to remove the executable file.

# Reported sum values

```
    Float sum: 0.094364
    Double sum: 4.89582e-11
```