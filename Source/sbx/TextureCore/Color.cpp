/*
 * @file    Color.cpp
 * @author  David Gallardo Moreno
 */

#include "Color.h"

#include <math.h>

namespace sbx
{
    TColor8U TColor32F::AsColor8U()
    { 
        return TColor8U(uint8_t(fminf(r * 255.f, 255.f)), 
                        uint8_t(fminf(g * 255.f, 255.f)), 
                        uint8_t(fminf(b * 255.f, 255.f)), 
                        uint8_t(fminf(a * 255.f, 255.f))); 
    };

}