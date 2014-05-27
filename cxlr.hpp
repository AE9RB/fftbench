//
//  cxlr.h
//  fftbench
//
//  Created by David Turnbull on 4/30/14.
//  Copyright (c) 2014 David Turnbull AE9RB. All rights reserved.
//

#ifndef fftbench_cxlr_hpp
#define fftbench_cxlr_hpp

// Specialize complex multiplication to bypass (nan,nan) checking.
// Same effect as GCC option -fcx-limited-range

#include <complex>
namespace std {
    
    inline complex<double>
    operator*(const complex<double>& z, const complex<double>& w)
    {
        double a = z.real();
        double b = z.imag();
        double c = w.real();
        double d = w.imag();
        return std::complex<double>(a*c - b*d, a*d + b*c);
    }
    
    inline complex<float>
    operator*(const complex<float>& z, const complex<float>& w)
    {
        float a = z.real();
        float b = z.imag();
        float c = w.real();
        float d = w.imag();
        return std::complex<float>(a*c - b*d, a*d + b*c);
    }
    
}

#endif
