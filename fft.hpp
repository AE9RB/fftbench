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
#include <vector>
#include <cmath>

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

    // Twiddle factors
    template<typename T, int D, size_t N>
    struct Twiddle {
        static const std::array<std::complex<T>, N/4> t1;
        static const std::array<std::complex<T>, N/4> t2;
        static const std::array<std::complex<T>, N/4> t3;
    };
    template<typename T>
    static std::vector<std::complex<T>> twiddles(double a, int d, size_t n) {
        std::vector<std::complex<T>> twids(n/4);
        double theta = M_PI*2*d/n;
        for (size_t i=0; i < n/4; ++i) {
            double phi = theta * a * i;
            twids[i] = std::complex<T>(cos(phi), sin(phi));
        }
        return twids;
    }
    template<typename T, int D, size_t N>
    const std::array<std::complex<T>, N/4> Twiddle<T, D, N>::t1(
        *reinterpret_cast<std::array<std::complex<T>, N/4>*>(twiddles<T>(1,D,N).data())
    );
    template<typename T, int D, size_t N>
    const std::array<std::complex<T>, N/4> Twiddle<T, D, N>::t2(
        *reinterpret_cast<std::array<std::complex<T>, N/4>*>(twiddles<T>(2,D,N).data())
    );
    template<typename T, int D, size_t N>
    const std::array<std::complex<T>, N/4> Twiddle<T, D, N>::t3(
        *reinterpret_cast<std::array<std::complex<T>, N/4>*>(twiddles<T>(3,D,N).data())
    );

    // Recursive template for butterfly mixing.
    template<typename T, int D, size_t N>
    class Butterfly {
        static const Twiddle<T, D, N> twiddle;
        static const size_t N4 = N/4;
        static Butterfly<T, D, N4> next;
        // Simplified multiplication for direction product.
        static std::complex<T> direction(const std::complex<T>& z)
        {
            if (D>0) return std::complex<T>(-z.imag(), z.real());
            else return std::complex<T>(z.imag(), -z.real());
        }
        // Fast multiplication (bypass NaN checking)
        static std::complex<T> mul(const std::complex<T>& z, const std::complex<T>& w)
        {
            return std::complex<T>(
                       z.real()*w.real() - z.imag()*w.imag(),
                       z.imag()*w.real() + z.real()*w.imag()
                   );
        }
    public:
        // Radix-4 mixer
        static void mix(std::complex<T>* data) {
            size_t i1 = N4;
            size_t i2 = N4 * 2;
            size_t i3 = N4 * 3;
            next.mix(data);
            next.mix(data+i1);
            next.mix(data+i2);
            next.mix(data+i3);
            // Index 0 twiddles are always (1+0i).
            std::complex<T> a0 = data[0];
            std::complex<T> a2 = data[i1];
            std::complex<T> a1 = data[i2];
            std::complex<T> a3 = data[i3];
            std::complex<T> b0 = a1 + a3;
            std::complex<T> b1 = direction(a1-a3);
            data[0] = a0 + a2 + b0;
            data[i1] = a0 - a2 + b1;
            data[i2] = a0 + a2 - b0;
            data[i3] = a0 - a2 - b1;
            // Index 1+ must multiply twiddles.
            for (size_t i0=1; i0 < N4; ++i0) {
                i1 = i0 + N4;
                i2 = i1 + N4;
                i3 = i2 + N4;
                a0 = data[i0];
                a2 = mul(data[i1], twiddle.t2[i0]);
                a1 = mul(data[i2], twiddle.t1[i0]);
                a3 = mul(data[i3], twiddle.t3[i0]);
                b0 = a1 + a3;
                b1 = direction(a1-a3);
                data[i0] = a0 + a2 + b0;
                data[i1] = a0 - a2 + b1;
                data[i2] = a0 + a2 - b0;
                data[i3] = a0 - a2 - b1;
            }
        }
    };

    // Terminates template recursion when not power of 4.
    template<typename T, int D>
    class Butterfly<T, D, 2> {
    public:
        // Radix-2 mixer
        static void mix(std::complex<T>* data) {
            std::complex<T> a0 = data[0];
            std::complex<T> a1 = data[1];
            data[0] = a0 + a1;
            data[1] = a0 - a1;
        }
    };

    // Terminates template recursion for powers of 4.
    template<typename T, int D>
    class Butterfly<T, D, 1> {
    public:
        static void mix(std::complex<T>* data) {
            // Do nothing.
        }
    };

    // Bit reversal pattern
    template<size_t N, bool ispow4>
    struct BitReverse {
        static const std::array<size_t, N> pattern;
    };
    static std::vector<size_t> bitpattern(size_t n, bool ispow4) {
        std::vector<size_t> l;
        n = (n*n) << 1;
        if (ispow4) n <<= 1;
        l.push_back(0);
        size_t m = 1;
        while ((m << 2) < n) {
            n >>= 1;
            for (size_t j = 0; j < m; j++) {
                l.push_back(l[j] + n);
            }
            m <<= 1;
        }
        return l;
    }
    template<size_t N, bool ispow4>
    const std::array<size_t, N> BitReverse<N, ispow4>::pattern(
        *reinterpret_cast<std::array<size_t, N>*>(bitpattern(N,ispow4).data())
    );

    // Start of Fourier Transforms.
    template<typename T, size_t N>
    class Transform {
        static_assert((N > 1) & !(N & (N - 1)), "Array size must be a power of two.");
        static constexpr bool ispow4_impl(size_t n, size_t m ) {
            return ((m << 2) < n) ? ispow4_impl(n >> 1, m << 1) : (m << 2) == n;
        }
        static constexpr size_t ispow4 = ispow4_impl(N,1);
        static constexpr size_t pattern_size_impl(size_t n, size_t m) {
            return ((m << 2) < n) ? pattern_size_impl(n >> 1, m << 1) : m;
        }
        static constexpr size_t pattern_size = pattern_size_impl(N,1);
        static const BitReverse<pattern_size, ispow4> bit;
        static void reindex(std::array<std::complex<T>, N> &data) {
            size_t j1, k1;
            constexpr size_t m = bit.pattern.size();
            constexpr size_t m2 = 2 * m;
            if (ispow4) {
                for (size_t k = 0; k < m; k++) {
                    for (size_t j = 0; j < k; j++) {
                        j1 = j + bit.pattern[k];
                        k1 = k + bit.pattern[j];
                        std::swap(data[j1], data[k1]);
                        j1 += m;
                        k1 += m2;
                        std::swap(data[j1], data[k1]);
                        j1 += m;
                        k1 -= m;
                        std::swap(data[j1], data[k1]);
                        j1 += m;
                        k1 += m2;
                        std::swap(data[j1], data[k1]);
                    }
                    j1 = k + m + bit.pattern[k];
                    k1 = j1 + m;
                    std::swap(data[j1], data[k1]);
                }
            } else {
                for (size_t k = 1; k < m; k++) {
                    for (size_t j = 0; j < k; j++) {
                        j1 = j + bit.pattern[k];
                        k1 = k + bit.pattern[j];
                        std::swap(data[j1], data[k1]);
                        j1 += m;
                        k1 += m;
                        std::swap(data[j1], data[k1]);
                    }
                }
            }
        }
        static void reindex(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
            size_t j1, k1;
            constexpr size_t m = bit.pattern.size();
            constexpr size_t m2 = 2 * m;
            if (ispow4) {
                for (size_t k = 0; k < m; k++) {
                    for (size_t j = 0; j < k; j++) {
                        j1 = j + bit.pattern[k];
                        k1 = k + bit.pattern[j];
                        out[j1] = in[k1];
                        out[k1] = in[j1];
                        j1 += m;
                        k1 += m2;
                        out[j1] = in[k1];
                        out[k1] = in[j1];
                        j1 += m;
                        k1 -= m;
                        out[j1] = in[k1];
                        out[k1] = in[j1];
                        j1 += m;
                        k1 += m2;
                        out[j1] = in[k1];
                        out[k1] = in[j1];
                    }
                    k1 = k + bit.pattern[k];
                    out[k1] = in[k1];
                    j1 = k1 + m;
                    k1 = j1 + m;
                    out[j1] = in[k1];
                    out[k1] = in[j1];
                    k1 += m;
                    out[k1] = in[k1];
                }
            } else {
                out[0] = in[0];
                out[m] = in[m];
                for (size_t k = 1; k < m; k++) {
                    for (size_t j = 0; j < k; j++) {
                        j1 = j + bit.pattern[k];
                        k1 = k + bit.pattern[j];
                        out[j1] = in[k1];
                        out[k1] = in[j1];
                        j1 += m;
                        k1 += m;
                        out[j1] = in[k1];
                        out[k1] = in[j1];
                    }
                    k1 = k + bit.pattern[k];
                    out[k1] = in[k1];
                    out[k1+m] = in[k1+m];
                }
            }
        }
    public:
        static void dft(std::array<std::complex<T>, N> &data) {
            reindex(data);
            Butterfly<T, -1, N>::mix(&data[0]);
        }
        static void idft(std::array<std::complex<T>, N> &data) {
            reindex(data);
            Butterfly<T, 1, N>::mix(&data[0]);
        }
        static void dft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
            reindex(in, out);
            Butterfly<T, -1, N>::mix(&out[0]);
        }
        static void idft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
            reindex(in, out);
            Butterfly<T, 1, N>::mix(&out[0]);
        }
    };

    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(std::array<std::complex<T>, N> &data) {
        Transform<T, N>::dft(data);
    }

    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
        Transform<T, N>::dft(in, out);
    }

    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(std::complex<T> (&data)[N]) {
        Transform<T, N>::dft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&data));
    }

    /// Discrete Fourier transform.
    template<typename T, size_t N>
    inline void dft(const std::complex<T> (&in)[N], std::complex<T> (&out)[N]) {
        Transform<T, N>::dft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&in),
                             *reinterpret_cast<std::array<std::complex<T>, N>*>(&out));
    }

    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(std::array<std::complex<T>, N> &data) {
        Transform<T, N>::idft(data);
    }

    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(const std::array<std::complex<T>, N> &in, std::array<std::complex<T>, N> &out) {
        Transform<T, N>::idft(in, out);
    }

    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(std::complex<T> (&data)[N]) {
        Transform<T, N>::idft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&data));
    }

    /// Inverse discrete Fourier transform.
    template<typename T, size_t N>
    inline void idft(const std::complex<T> (&in)[N], std::complex<T> (&out)[N]) {
        Transform<T, N>::idft(*reinterpret_cast<std::array<std::complex<T>, N>*>(&in),
                              *reinterpret_cast<std::array<std::complex<T>, N>*>(&out));
    }

}
#endif
