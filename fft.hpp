// Copyright (c) 2014 David Turnbull
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef fft_hpp
#define fft_hpp

#include <complex>
#include <array>

/// \brief FFT - discrete Fourier transforms
/// \author David Turnbull
/// \copyright MIT License
///
/// Uses C++11 features and template recursion for a reasonably
/// efficient implementation of a radix 4 Fast Fourier Transform.
///
/// Works on complex arrays with power-of-two sizes.
/// Does not throw errors.
///
/// Example usage:
///
///     std::array<std::complex<float>,512> data;
///     FFT::dft(data);
///
/// C-style arrays are supported:
///
///     std::complex<float> data[512];
///     FFT::dft(data);
///
/// Out-of-place transform:
///
///     std::array<std::complex<double>,64> in;
///     std::array<std::complex<double>,64> out;
///     FFT::dft(in, out);

namespace FFT {
    
    /// Recursive template for mixing.
    template<typename T, size_t N, int D>
    class Radix4 {
        static std::array<std::complex<T>, N/4> t1;
        static std::array<std::complex<T>, N/4> t2;
        static std::array<std::complex<T>, N/4> t3;
        static const size_t N4 = N/4;
        Radix4<T, N4, D> next;
        /// Limited range (fast) multiplication of complex numbers.
        static inline std::complex<T> multiply(const std::complex<T>& z, const std::complex<T>& w)
        {
            T a = z.real();
            T b = z.imag();
            T c = w.real();
            T d = w.imag();
            T ac = a * c;
            T bd = b * d;
            T ad = a * d;
            T bc = b * c;
            T x = ac - bd;
            T y = ad + bc;
            return std::complex<T>(x, y);
        }
        static inline std::complex<T> direction(const std::complex<T>& z)
        {
            if (D>0) return std::complex<T>(-z.imag(), z.real());
            else return std::complex<T>(z.imag(), -z.real());
        }
    public:
        Radix4() {
            // Initialize twiddle factors on first instance.
            if (!t1[0].real()) {
                T theta = M_PI*2*D/N;
                for (size_t i=0; i < N4; ++i) {
                    T phi = i*theta;
                    t1[i] = std::complex<T>(cos(phi), sin(phi));
                    t2[i] = std::complex<T>(cos(phi*2), sin(phi*2));
                    t3[i] = std::complex<T>(cos(phi*3), sin(phi*3));
                }
            }
        }
        void mix(std::complex<T>* data) {
            next.mix(data);
            next.mix(data+N4);
            next.mix(data+N4*2);
            next.mix(data+N4*3);
            // Index 0 twiddles are always (1+0i).
            size_t i1 = N4;
            size_t i2 = N4 + N4;
            size_t i3 = i2 + N4;
            std::complex<T> a0 = data[0];
            std::complex<T> a1 = data[i2];
            std::complex<T> a2 = data[i1];
            std::complex<T> a3 = data[i3];
            std::complex<T> b0 = (a1+a3);
            std::complex<T> b1 = direction(a1-a3);
            data[0] = a0 + a2 + b0;
            data[i1] = a0 - a2 + b1;
            data[i2] = a0 + a2 - b0;
            data[i3] = a0 - a2 - b1;
            // Index 1+ must multiply twiddles.
            for (size_t i0=1; i0 < N4; ++i0) {
                size_t i1 = i0 + N4;
                size_t i2 = i1 + N4;
                size_t i3 = i2 + N4;
                std::complex<T> a0 = data[i0];
                std::complex<T> a1 = data[i2];
                std::complex<T> a2 = data[i1];
                std::complex<T> a3 = data[i3];
                a1 = multiply(a1, t1[i0]);
                a2 = multiply(a2, t2[i0]);
                a3 = multiply(a3, t3[i0]);
                std::complex<T> b0 = (a1+a3);
                std::complex<T> b1 = direction(a1-a3);
                data[i0] = a0 + a2 + b0;
                data[i1] = a0 - a2 + b1;
                data[i2] = a0 + a2 - b0;
                data[i3] = a0 - a2 - b1;
            }
        }
    };
    
    // Storage for twiddle factors
    template<typename T, size_t N, int D>
    std::array<std::complex<T>, N/4> Radix4<T, N, D>::t1 {0};
    template<typename T, size_t N, int D>
    std::array<std::complex<T>, N/4> Radix4<T, N, D>::t2;
    template<typename T, size_t N, int D>
    std::array<std::complex<T>, N/4> Radix4<T, N, D>::t3;
    
    /// Terminates template recursion for powers of 2.
    template<typename T, int D>
    class Radix4<T, 2, D> {
    public:
        void mix(std::complex<T>* data) {
            std::complex<T> a0 = data[0];
            std::complex<T> a1 = data[1];
            data[0] = a0 + a1;
            data[1] = a0 - a1;
        }
    };
    
    /// Terminates template recursion for powers of 4.
    template<typename T, int D>
    class Radix4<T, 1, D> {
    public:
        void mix(std::complex<T>* data) {
            // Do nothing.
        }
    };
    
    /// Initiates a transform.
    template<typename T, size_t N>
    class FFT {
        static void reindex(std::array<std::complex<T>, N> &data) {
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
        static void reindex(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
            for (size_t i=0, j=0; i<N; ++i) {
                out[i] = in[j];
                size_t m = N>>1;
                while (m>=1 && j>=m) {
                    j -= m;
                    m >>= 1;
                }
                j += m;
            }
        }
        static void validate_template() {
            static_assert((N > 1) & !(N & (N - 1)), "Array size must be a power of two.");
        }
    public:
        static void dft(std::array<std::complex<T>, N> &data) {
            validate_template();
            Radix4<T, N, -1> mixer;
            reindex(data);
            mixer.mix(&data[0]);
        }
        static void dft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
            validate_template();
            Radix4<T, N, -1> mixer;
            reindex(in, out);
            mixer.mix(&out[0]);
        }
        static void idft(std::array<std::complex<T>, N> &data) {
            validate_template();
            Radix4<T, N, 1> mixer;
            reindex(data);
            mixer.mix(&data[0]);
        }
        static void idft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
            validate_template();
            Radix4<T, N, 1> mixer;
            reindex(in, out);
            mixer.mix(&out[0]);
        }
    };
    
    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(std::array<std::complex<T>, N> &data) {
        FFT<T, N>::dft(data);
    }
    
    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
        FFT<T, N>::dft(in, out);
    }
    
    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(std::complex<T> (&data)[N]) {
        FFT<T, N>::dft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&data));
    }
    
    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(const std::complex<T> (&in)[N], std::complex<T> (&out)[N]) {
        FFT<T, N>::dft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&in), *reinterpret_cast<std::array<std::complex<T>, N>*>(&out));
    }
    
    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(std::array<std::complex<T>, N> &data) {
        FFT<T, N>::idft(data);
    }
    
    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
        FFT<T, N>::idft(in, out);
    }
    
    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(std::complex<T> (&data)[N]) {
        FFT<T, N>::idft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&data));
    }
    
    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(const std::complex<T> (&in)[N], std::complex<T> (&out)[N]) {
        FFT<T, N>::idft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&in), *reinterpret_cast<std::array<std::complex<T>, N>*>(&out));
    }
    
}
#endif
