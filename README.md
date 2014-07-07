# Optimizing with C++11 templates

This code demonstrates using recursive C++11 templates to improve
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
and division operators in ::std can be replaced with limited range versions.
This is done to ease the development process and becomes unnecessary in
the final fft.hpp code which uses its own multiply() method.

 * cxlr.hpp - Changes std::complex multiplication.

Everything is included in this repository and there is only one file to
compile. No makefile is used, simply compile and run with:

```g++ -o bench -std=c++11 -O3 main.cpp && ./bench```

Example output from GCC 4.9:

```
[ RUN      ] FFTfixture<float>.four1
[   TIME   ] 27 iterations, 504.25 us
[       OK ] FFTfixture<float>.four1 (13 ms)
[ RUN      ] FFTfixture<float>.four1plus
[   TIME   ] 21 iterations, 504.75 us
[       OK ] FFTfixture<float>.four1plus (10 ms)
[ RUN      ] FFTfixture<float>.four1tmpl
[   TIME   ] 22 iterations, 324.333 us
[       OK ] FFTfixture<float>.four1tmpl (11 ms)
[ RUN      ] FFTfixture<float>.fft
[   TIME   ] 24 iterations, 132.167 us
[       OK ] FFTfixture<float>.fft (8 ms)
```

## License

This project is designed as an educational tool to demonstrate a method
for optimizing algorithms in C++11. Some of the code, particularly the
four1.hpp example, is presented here under fair use. Depending on how
you plan to use this project, you may be required to obtain permission
with the following two exceptions:

The benchtest library is a snapshot from my dspp project. If you want to
try it in your project, it is best to get from there:
https://github.com/AE9RB/dspp

The fft.hpp file is MIT licensed. License and documentation are in the file.
Simply copy it to your project. It has no dependencies.
