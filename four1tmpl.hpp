// FFT algorithm as a C++11 recursive template.

#ifndef fftbench_four1tmpl_hpp
#define fftbench_four1tmpl_hpp

template<typename T, size_t N>
class Four1tmplMixer {
    typedef typename T::value_type fp_type;
    static const size_t N2 = N/2;
    Four1tmplMixer<T, N2> next;
public:
    // Danielson-Lanczos mixer
    void mix(T* data) {
        next.mix(data);
        next.mix(data+N2);
        T wp(-2.0*pow(sin(M_PI/N),2), -sin(2*M_PI/N));
        T w(1.0,0);
        for (size_t i=0; i < N2; ++i) {
            size_t j = i + N2;
            T temp = w * data[j];
            data[j] = data[i] - temp;
            data[i] += temp;
            w += w * wp;
        }
    }
};

template<typename T>
class Four1tmplMixer<T, 1> {
public:
    void mix(T* data) {
    }
};

template<typename T, size_t N>
class Four1tmpl {
    Four1tmplMixer<T, N> mixer;
    // reverse-binary reindexer
    static void reindex(std::array<T, N> &data) {
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
    }
public:
    void fft(std::array<T, N> &data) {
        static_assert((N > 1) & !(N & (N - 1)), "Array size must be a power of two.");
        reindex(data);
        mixer.mix(&data[0]);
    }
};

#endif
