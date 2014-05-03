//
//  cxlr.h
//  fftbench
//
//  Created by David Turnbull on 4/30/14.
//  Copyright (c) 2014 David Turnbull AE9RB. All rights reserved.
//

#ifndef fftbench_cxlr_hpp
#define fftbench_cxlr_hpp

// Specialize complex multiplication and
// division to bypass (nan,nan) checking.
// Same effect as GCC option -fcx-limited-range
//
// Not recommended for production code.

#include <complex>
namespace std {
    
    inline complex<double>
    operator/(const complex<double>& z, const complex<double>& w)
    {
        double a = z.real();
        double b = z.imag();
        double c = w.real();
        double d = w.imag();
        double denom = c * c + d * d;
        double x = (a * c + b * d) / denom;
        double y = (b * c - a * d) / denom;
        return complex<double>(x, y);
    }
    
    inline complex<float>
    operator/(const complex<float>& z, const complex<float>& w)
    {
        float a = z.real();
        float b = z.imag();
        float c = w.real();
        float d = w.imag();
        float denom = c * c + d * d;
        float x = (a * c + b * d) / denom;
        float y = (b * c - a * d) / denom;
        return complex<float>(x, y);
    }
    
    inline complex<double>
    operator*(const complex<double>& z, const complex<double>& w)
    {
        double a = z.real();
        double b = z.imag();
        double c = w.real();
        double d = w.imag();
        double ac = a * c;
        double bd = b * d;
        double ad = a * d;
        double bc = b * c;
        double x = ac - bd;
        double y = ad + bc;
        return complex<double>(x, y);
    }
    
    inline complex<float>
    operator*(const complex<float>& z, const complex<float>& w)
    {
        float a = z.real();
        float b = z.imag();
        float c = w.real();
        float d = w.imag();
        float ac = a * c;
        float bd = b * d;
        float ad = a * d;
        float bc = b * c;
        float x = ac - bd;
        float y = ad + bc;
        return complex<float>(x, y);
    }
    
}

#endif
