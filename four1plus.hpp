// Cooley-Tukey algorithm in C++11.
// Uses std::complex type with static length.

#ifndef fftbench_four1plus_hpp
#define fftbench_four1plus_hpp

template<typename T, size_t N>
void four1plus(std::array<T, N> &data) {
    typedef typename T::value_type fp_type;
    static_assert((N > 1) & !(N & (N - 1)), "Array size must be a power of two.");
    // reverse-binary reindexing
    for (size_t i=0, j=0; i<N; ++i) {
        if (j>i) {
            std::swap(data[i], data[j]);
        }
        size_t m = N>>1;
        while (m>=1 && j>=m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    // Danielson-Lanczos
    for (size_t mmax=1; N>mmax; mmax <<= 1) {
        T wp(-2.0*pow(sin(M_PI/(mmax*2)),2), -sin(2*M_PI/(mmax*2)));
        T w(1.0,0);
        for (size_t m=0; m < mmax; ++m) {
            for (size_t i=m; i < N; i += mmax * 2) {
                size_t j = i + mmax;
                T temp = w * data[j];
                data[j] = data[i] - temp;
                data[i] += temp;
            }
            w += w * wp;
        }
    }
};

#endif
