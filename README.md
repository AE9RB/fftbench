# Optimizing with C++11 templates

This code demonstrates uses recursive C++11 templates to improve
performance of the Cooley-Tukey algorithm for FFT. First, the
classical code is updated to use basic C++11 features. This makes
things simpler and easier to understand. Next, we convert the
looping structure into a recursive template. Finally, we upgrade
from radix 2 to radix 4 and apply various optimizations.

 * four1.hpp - Classical implementation using loops.
 * four1plus.hpp - Like four1 but using std::array and std::complex.
 * four1tmpl.hpp - Improved four1 using template recursion.
 * fft.hpp - Radix 4 version with lots of optimizations.

C++11 compilers use ISO rules for std::complex multiplication and division.
The extra checking for (nan,nan) results cause a significant performance
problem. Because this is not needed for DSP applications, the multiplication
and division operators in ::std are replaced with limited range versions.
This is done to ease the development process and becomes unnecessary in
the final fft.hpp code which uses its own multiply() method.

 * cxlr.hpp - Changes std::complex multiplication and division.

Benchmarking is done with a modified hayai library. Everything is included
in this repository and there is only one file to compile. No makefile is
used, simply compile and run with:

```g++ -o bench -std=c++11 -O3 main.cpp && ./bench```

Example output from GCC 4.9:

```
[==========] Running 4 benchmarks.
[ RUN      ] Float8192.four1 (100 runs, 1 iteration per run)
[     DONE ] Float8192.four1 (54.876165 ms)
[          ] Fastest time: 499.402 us
[ RUN      ] Float8192.four1plus (100 runs, 1 iteration per run)
[     DONE ] Float8192.four1plus (53.903960 ms)
[          ] Fastest time: 496.474 us
[ RUN      ] Float8192.four1tmpl (100 runs, 1 iteration per run)
[     DONE ] Float8192.four1tmpl (39.398755 ms)
[          ] Fastest time: 319.323 us
[ RUN      ] Float8192.fft (100 runs, 1 iteration per run)
[     DONE ] Float8192.fft (18.977589 ms)
[          ] Fastest time: 166.194 us
[==========] Ran 4 benchmarks.
```

## License

This project is designed as an educational tool to demonstrate a method
for optimizing algorithms in C++11. Some of the code, particularly the
four1.hpp example, is presented here under fair use. Depending on how
you plan to use this project, you may be required to obtain permission
with the following two exceptions:

The hayai library is open source. You may obtain an unmodified version
from: https://github.com/nickbruun/hayai

The fft.hpp file is MIT licensed.
