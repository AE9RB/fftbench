// Run with:
// g++ -o bench -std=c++11 -O3 main.cpp && ./bench
//
// Include FFTW3 results with:
// g++ -o bench -std=c++11 -O3 main.cpp -lfftw3f -DUSEFFTW && ./bench

#include <complex>
#include <array>

#define HAYAI_NO_COLOR
#include "hayai/hayai.hpp"
#include "cxlr.hpp"
#include "four1.hpp"
#include "four1plus.hpp"
#include "four1tmpl.hpp"
#include "fft.hpp"

int main() {
    hayai::ConsoleOutputter consoleOutputter;
    hayai::Benchmarker::AddOutputter(consoleOutputter);
    hayai::Benchmarker::RunAllTests();
    return 0;
}

// This fixture performs a basic validation to ensure
// the FFT implementation is actually working.
template<typename T, size_t N>
class FFTfixture : public hayai::Fixture {
public:
    // Data to be processed.
    std::array<std::complex<T>, N> data;
    // Zero the array and add a single impulse at position 1.
    virtual void SetUp() {
        for (auto &d : data) {d=0;}
        data[1] = 1;
    }
    // A successful FFT will distribute the impulse evenly
    // across the entire output. Verify the magnitude is
    // close to 1.00000 for every element.
    virtual void TearDown() {
        for (auto d : data) {
            if (round(abs(d)*100000) != 100000) {
                throw std::runtime_error( "FFT result error" );
            }
        }
    }
};
// typdefs are needed for benchmark macros
typedef FFTfixture<float, 8192> Float8192;

BENCHMARK_F(Float8192, four1, 100, 1) {
    four1((float*)&data[0], data.size());
}

BENCHMARK_F(Float8192, four1plus, 100, 1) {
    four1plus(data);
}

BENCHMARK_F(Float8192, four1tmpl, 100, 1) {
    Four1tmpl<std::complex<float>, 8192>::fft(data);
}

BENCHMARK_F(Float8192, fft, 100, 1) {
    FFT::dft(data);
}


// Optional benchmark for FFTW3
#ifdef USEFFTW
#include <fftw3.h>
template<size_t N>
class FFTW3fixture : public hayai::Fixture {
public:
    fftwf_complex* in;
    fftwf_complex* out;
    fftwf_plan plan;
    virtual void SetUp() {
        in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
        out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
        plan = fftwf_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_MEASURE);
        for (size_t i=0; i < N; ++i) {
            in[i][0] = 0;
            in[i][1] = 0;
        }
        in[1][0] = 1;
    }
    virtual void TearDown() {
        for (size_t i=0; i < N; ++i) {
            if (round(abs(std::complex<float>(out[i][0], out[i][1]))*100000) != 100000) {
                throw std::runtime_error( "FFT result error" );
            }
        }
        fftwf_destroy_plan(plan);
        fftwf_free(in);
        fftwf_free(out);
    }
};
typedef FFTW3fixture<8192> FFTW3float8192;
BENCHMARK_F(FFTW3float8192, aligned, 100, 1) {
    fftwf_execute(plan);
}
#endif // USEFFTW
