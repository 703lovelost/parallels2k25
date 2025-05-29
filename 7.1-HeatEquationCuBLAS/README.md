Heat equation solving by using Jacobi iteration with Linf-norm error calculation.

There are CuBLAS calculation methods being used to speed up the process.

![](./matrix_10x10_heatmap.png)

[**Click here to open the report.**](https://docs.google.com/document/d/1K6WTe-WgsHO4tY2BC-wii79cg_bZIyy-jjPoaRZ1ruk/edit?usp=sharing)

# Build

```
    scripts/build.sh
```

# Run

```
    scripts/run.sh 512 1e-6 1000000

    # To make your own visualized matrix.
    # Make sure you have numpy and matplotlib installed.
    scripts/run.sh 10 1e-6 1000
    python3 show_matrix.py
```

# Generate Nsight Systems report

```
    scripts/clean.sh
    scripts/build.sh
    nsys profile --trace=cuda,openacc --force-overwrite true -o heat2d_report ./build/heat2d --size 256 --tol 1e-6 --max-iter 10000

    # To print in CLI
    nsys stats heat2d_report.nsys-rep

    # To open in GUI
    nsys-ui heat2d_report.nsys-rep
```

# Clean

```
    scripts/clean.sh
```