# Matrix Multiplication Performance Study
 
A performance comparison of three square matrix multiplication implementations in C++: a naive triple-loop baseline, a hand-written AVX SIMD version using Agner Fog's Vector Class Library, and the Eigen linear algebra library. Each was profiled with `perf` and analysed at the assembly level to explain the results.
 
**Key results (1024×1024 matrices)**
 
- SIMD implementation: **6× faster** than the naive baseline (1184 ms → 196 ms)
- SIMD cut the L1 cache load miss rate from **66% to 24%**
- Eigen was a further **4.7× faster** than SIMD, explained by its cache-blocked algorithm (6.5% L1 miss rate)
## Implementations
 
| Implementation | Description |
| --- | --- |
| Basic | Naive i-j-k triple loop with a scalar accumulator ([adapted from GeeksforGeeks](https://www.geeksforgeeks.org/dsa/c-program-multiply-two-matrices/)) |
| SIMD | Hand-written AVX implementation using Agner Fog's [Vector Class Library](https://github.com/vectorclass/version2). Uses i-k-j loop order: broadcasts `A[i][k]` into a `Vec4d` and updates 4 output values at a time while streaming contiguous rows of `B`. Requires N to be a multiple of 4. |
| Eigen | [Eigen](https://eigen.tuxfamily.org/) library's built-in matrix product |
 
## Method
 
**Inputs.** Matrices of random doubles between -1 and 1, tested at 128×128, 512×512, and 1024×1024.
 
**Measurements.** Runtime was measured as process CPU time with `clock_gettime`, after three warmup runs. `perf` was used to measure L1 data cache load misses, last-level cache (LLC) load misses, and branch mispredictions. Compiler output was inspected in Compiler Explorer where possible, and through the generated `.s` files where external libraries were required.
 
**Build.** g++ with `-O3 -march=haswell` (enables AVX2 and FMA), C++17.
 
## Results
 
| Implementation | Size | Time (ms) | L1 Load Miss (%) | LLC Load Miss (%) | Branch Miss (%) |
| --- | --- | ---: | ---: | ---: | ---: |
| Basic | 128 | 2.8 | 47.63 | 23.19 | 1.37 |
| Basic | 512 | 146.9 | 64.31 | 0.01 | 0.39 |
| Basic | 1024 | 1183.5 | 65.62 | 0.13 | 0.20 |
| Eigen | 128 | 0.9 | 5.56 | 13.24 | 1.52 |
| Eigen | 512 | 5.9 | 6.48 | 3.19 | 0.34 |
| Eigen | 1024 | 42.1 | 6.51 | 12.97 | 0.32 |
| SIMD | 128 | 1.4 | 12.72 | 13.19 | 1.09 |
| SIMD | 512 | 23.4 | 22.56 | 1.83 | 0.73 |
| SIMD | 1024 | 196.3 | 23.90 | 1.79 | 0.38 |
 
**Speedup over Basic**
 
| Size | SIMD | Eigen |
| --- | ---: | ---: |
| 128 | 2.0× | 3.1× |
| 512 | 6.3× | 24.9× |
| 1024 | 6.0× | 28.1× |

<p>
  <img width="420" height="280" alt="Runtime vs matrix size" src="https://github.com/user-attachments/assets/eea7c106-600f-4a10-a010-2adca47ce241" />
  <img width="420" height="280" alt="L1 cache load miss rate vs matrix size" src="https://github.com/user-attachments/assets/2dd0f666-f90f-4e72-84c2-90b7f131adce" />
</p>



 
## Findings
 
**Why the naive version is slowest.** In the compiler-generated assembly, the inner loop's accumulator dependency prevented full autovectorization. The compiler could only partially vectorize using 128-bit `xmm` registers rather than 256-bit `ymm` registers.
 
**Why SIMD helps.** The AVX implementation processes four doubles per instruction in `ymm` registers. Its i-k-j loop order also reads `B` row by row instead of striding down columns like the naive version, which cut the L1 miss rate from roughly 66% to 24% at 1024×1024.
 
**Why Eigen still wins.** Eigen also uses AVX (`ymm` registers in its assembly), but it adds a block-based algorithm that keeps working sets in cache. Its L1 miss rate stayed around 6.5% at every size, while the SIMD version's rose with matrix size.
 
**Branch prediction is not a factor.** Matrix multiplication has no data-dependent branches, and mispredictions stayed below 1.6% in every case, decreasing as matrix size grew.
 
**LLC misses were noisy** and did not track runtime clearly; performance was dominated by L1 locality.
 
## Conclusion
 
My hypothesis was that hand-written SIMD would outperform both alternatives. It beat the naive baseline by 6×, but Eigen was faster still. Vectorization alone isn't enough at larger sizes: cache blocking is what keeps the vector units fed.
 
## Project structure
 
| File | Purpose |
| --- | --- |
| `matrix.h` / `matrix.cpp` | The three multiplication implementations and the matrix generators (random, incremental, alternating) |
| `tests.cpp` | Correctness check: multiplies two small matrices with all three implementations and prints the results for comparison |
| `timing.cpp` | Benchmark: times each implementation at a given size (set `N` in `main`) |
 
## Dependencies
 
Not included in this repo; download them into the project folder:
 
- [Eigen 5.0.0](https://eigen.tuxfamily.org/) → `eigen-5.0.0/`
- [Vector Class Library](https://github.com/vectorclass/version2) → `vectorclass/`
## Build and run
 
Requires g++ with C++17, a CPU with AVX2 (Haswell or newer), and Linux with `perf` for profiling.
 
**Correctness tests**
 
```bash
g++ -Wall -Wpedantic -std=c++17 -march=haswell -O3 -Ivectorclass -Ieigen-5.0.0 -c matrix.cpp tests.cpp
g++ matrix.o tests.o -o tests
./tests
```
 
**Benchmarks**
 
```bash
g++ -Wall -Wpedantic -std=c++17 -march=haswell -O3 -Ivectorclass -Ieigen-5.0.0 -c matrix.cpp timing.cpp
g++ matrix.o timing.o -o timing
./timing
```
 
**Profiling**
 
```bash
perf stat -e L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses,branches,branch-misses ./timing
```
 

 
