// Run with:
// g++ -o bench -std=c++11 -O3 main.cpp && ./bench

#include <array>

#include "benchtest/benchtest.hpp"
#include "cxlr.hpp"
#include "four1.hpp"
#include "four1plus.hpp"
#include "four1tmpl.hpp"
#include "fft.hpp"

int main() {
    testing::reporter(new testing::DefaultReporter);
    return testing::Runner::RunAll();
}

const std::array<std::complex<double>, 8> ref0 {{
    std::complex<double>(-0.82993510256513270,0.78322255460971535),
    std::complex<double>(-0.62062045620071216,-0.20398322370742217),
    std::complex<double>(0.48702490306452950,0.12077985630401211),
    std::complex<double>(0.61913330685474266,0.02342510560093802),
    std::complex<double>(0.99016909661405061,0.93322272660158068),
    std::complex<double>(-0.14789834511540456,0.30599745382135302),
    std::complex<double>(0.92306621915157949,0.71597467817430172),
    std::complex<double>(-0.41194770159675098,-0.17071084234348244)
}};

const std::array<std::complex<double>, 8> ref1 {{
    std::complex<double>(1.00899192020690176,2.50792830906099606),
    std::complex<double>(-3.70198435453455810,-0.60666385918657428),
    std::complex<double>(-1.00055716131071581,1.85539515330709071),
    std::complex<double>(-0.38489594400606586,-0.48297521012410377),
    std::complex<double>(2.13165831232315206,2.59847132231822364),
    std::complex<double>(-1.12861368756438774,1.17874614737694361),
    std::complex<double>(-1.49915709502366634,-0.09601365984112620),
    std::complex<double>(-2.06492281061172145,-0.68910776603372681)
}};


template<typename T>
class FFTfixture : public testing::Test {
    std::array<std::complex<T>, 8> *test8;
    std::array<std::complex<T>, 8192> *data;
protected:
    FFTfixture() :
        test8(new std::array<std::complex<T>, 8>),
        data(new std::array<std::complex<T>, 8192>) {
    }
    ~FFTfixture() {
        delete data;
        delete test8;
    }
    void SetUp() {
        for (size_t i=0; i<8; ++i) {
            (*test8)[i] = ref0[i];
        }
        for (size_t i=0; i<8192; ++i) {
            (*data)[i] = ref0[i%8];
        }
    }
    void Validate() {
        for (size_t i=0; i<8; ++i) {
            SCOPED_TRACE() << "i=" << i;
            ASSERT_EQ(std::complex<T>(ref1[i]), (*test8)[i]);
        }
    }
    
    void four1() {
        ::four1((T*)test8, test8->size());
        ASSERT_NO_FATAL_FAILURE(Validate());
        while (Benchmark()) {
            ::four1((T*)data, data->size());
        }
    }

    void four1plus() {
        ::four1plus(*test8);
        ASSERT_NO_FATAL_FAILURE(Validate());
        while (Benchmark()) {
            ::four1plus(*data);
        }
    }

    void four1tmpl() {
        Four1tmpl<std::complex<T>, 8>::fft(*test8);
        ASSERT_NO_FATAL_FAILURE(Validate());
        while (Benchmark()) {
            Four1tmpl<std::complex<T>, 8192>::fft(*data);
        }
    }

    void fft() {
        FFT::dft(*test8);
        ASSERT_NO_FATAL_FAILURE(Validate());
        while (Benchmark()) {
            FFT::dft(*data);
        }
    }

};

TEST_T(FFTfixture, float, four1);
TEST_T(FFTfixture, float, four1plus);
TEST_T(FFTfixture, float, four1tmpl);
TEST_T(FFTfixture, float, fft);
