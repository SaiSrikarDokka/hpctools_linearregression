# Task — Sequential Baseline: Linear Regression via Normal Equations

## Context

Given a dataset of `N` observations, each described by `p` predictor variables, linear regression seeks the coefficient vector `β` (size `p`) that best fits the data in a least-squares sense:

```
minimize  ||Xβ - y||²
```

where `X` is the `N × p` matrix of observations and `y` is the `N × 1` vector of target values. This problem has a closed-form solution obtained by setting the gradient to zero, which leads to the **normal equations**:

```
(XᵀX) β = Xᵀy
```

`XᵀX` is a `p × p` symmetric positive-definite matrix, and `Xᵀy` is a `p × 1` vector. Solving for `β` therefore requires two steps:

1. **Matrix multiplication**: compute `XᵀX` (and `Xᵀy`).
2. **Linear system solve**: solve the resulting `p × p` system for `β`.

## Task

Implement a **sequential, non-optimized** baseline version of this linear regression pipeline: naive (ijk) matrix multiplication for `XᵀX`/`Xᵀy`, followed by Gaussian elimination (with partial pivoting) and back substitution to solve the resulting system. No blocking, vectorization, or parallelization: clarity and correctness first. This baseline is your reference for later optimization stages.

Add (and try) an alternative solver for the linear equation system, using Gauss-Jordan instead of simple Gaussian elimination. Modify the code to allow the switching between both (non-optimizes so far) solvers.

You are given a routine that generates synthetic data: a random `X`, a known ground-truth `β_true`, and `y = X·β_true + noise`. This lets you verify your computed `β` against the known ground truth.

## Configurations to Test

The executable accepts an optional solver selector immediately after `N p`
or after the optional seed and noise arguments. It defaults to Gaussian
elimination:

```text
./linreg N p [gaussian|gauss_jordan] [seed] [noise_std]
./linreg N p [seed] [noise_std] [gaussian|gauss_jordan]
```

For example, `./linreg 20000 50 gauss_jordan` and
`./linreg 20000 50 42 0.5 gauss_jordan` both select the Gauss-Jordan solver.
Submit `benchmark_gcc.sh`, `benchmark_icc.sh`, and `benchmark_icx.sh` as
separate Slurm jobs. Each runs one compiler and uses its own build and results
directories. The program reports the elapsed time for computing `XᵀX`,
computing `Xᵀy`, solving the system, and all compute kernels combined.

You must run and report results for the following three `(N, p)` configurations, chosen to stress the two kernels differently:

| Config | N (observations) | p (variables) | What it illustrates |
|---|---|---|---|
| 1 | 20,000 | 50 | Matmul-dominated; the resulting system is tiny and cheap to solve |
| 2 | 50,000 | 300 | Matmul-dominated, but `X` itself is now too large to fit in cache |
| 3 | 2,000 | 2,000 | Matmul and system-solve costs become comparable — both kernels matter |

## Deliverables

1. Source code implementing the baseline.
2. A short benchmarking report in FT3, including: verification that your `β` matches `β_true` within tolerance, and execution time for each configuration above (averaged over multiple runs, as usual).
  - The benchmarking will show results with binaries built with `gcc-10.1.0`, `icc 2021.3.0` and `icx 2021.3.0` using these optimization levels: `-O0`, `-O2`, `-O3` and `-Ofast` (include -march=native in every level greater than `O0`).
  - Consider the total execution time for the linear regression computation, after allocating and generating the data structures.
