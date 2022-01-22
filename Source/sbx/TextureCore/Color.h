/*
 * @file    Color.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_COLOR_H__
#define __SBX_COLOR_H__

#include <stdint.h>

namespace sbx
{
    struct TColor8U;
    struct TColor32F
    {
        union
        {
            struct
            {
                float r, g, b, a;
            };
            float mBuffer[4];
        };

        TColor32F ()
            : r(0), g(0), b(0), a(0)
        {;}

        TColor32F (float aR, float aG, float aB, float aA = 1.0f)
            : r(aR), g(aG), b(aB), a(aA)
        {;}

        TColor8U AsColor8U();
    };

	inline TColor32F const operator*(float n, TColor32F const & c) { return TColor32F(n * c.r, n * c.g, n * c.b, n * c.a); };
	inline TColor32F const operator/(float n, TColor32F const & c) { return TColor32F(n / c.r, n / c.g, n / c.b, n / c.a); };
	inline TColor32F const operator*(TColor32F const & c, float n) { return n * c; };
	inline TColor32F const operator/(TColor32F const & c, float n) { return TColor32F(c.r / n, c.g / n, c.b / n, c.a / n); };

	inline TColor32F const operator+(TColor32F const & ca, TColor32F const & cb) { return TColor32F(ca.r + cb.r, ca.g + cb.g, ca.b + cb.b, ca.a + cb.a); };
	inline TColor32F const operator-(TColor32F const & ca, TColor32F const & cb) { return TColor32F(ca.r - cb.r, ca.g - cb.g, ca.b - cb.b, ca.a - cb.a); };
	inline TColor32F const operator*(TColor32F const & ca, TColor32F const & cb) { return TColor32F(ca.r * cb.r, ca.g * cb.g, ca.b * cb.b, ca.a * cb.a); };
	inline TColor32F const operator/(TColor32F const & ca, TColor32F const & cb) { return TColor32F(ca.r / cb.r, ca.g / cb.g, ca.b / cb.b, ca.a / cb.a); };

	inline TColor32F const & operator+(TColor32F & c, float n) { c.r += n; c.g += n; c.b += n; c.a += n; return c; };
	inline TColor32F const & operator-(TColor32F & c, float n) { c.r -= n; c.g -= n; c.b -= n; c.a -= n; return c; };
	inline TColor32F const & operator*(TColor32F & c, float n) { c.r *= n; c.g *= n; c.b *= n; c.a *= n; return c; };
	inline TColor32F const & operator/(TColor32F & c, float n) { c.r /= n; c.g /= n; c.b /= n; c.a /= n; return c; };

	inline TColor32F const & operator+=(TColor32F & ca, TColor32F const & cb) { ca.r += cb.r; ca.g += cb.g; ca.b += cb.b; ca.a += cb.a; return ca; };
	inline TColor32F const & operator-=(TColor32F & ca, TColor32F const & cb) { ca.r -= cb.r; ca.g -= cb.g; ca.b -= cb.b; ca.a -= cb.a; return ca; };
	inline TColor32F const & operator*=(TColor32F & ca, TColor32F const & cb) { ca.r *= cb.r; ca.g *= cb.g; ca.b *= cb.b; ca.a *= cb.a; return ca; };
	inline TColor32F const & operator/=(TColor32F & ca, TColor32F const & cb) { ca.r /= cb.r; ca.g /= cb.g; ca.b /= cb.b; ca.a /= cb.a; return ca; };

    struct TColor8U
    {
        union 
        { 
            struct { uint8_t r, g, b, a; }; 
            uint32_t mWord; 
            uint8_t  mBuffer[4]; 
        };

        TColor8U()
            : mWord(255)
        {;}

        TColor8U (uint8_t aR, uint8_t aG, uint8_t aB, uint8_t aA = 255)
            : r(aR), g(aG), b(aB), a(aA)
        {;}

        TColor8U(TColor8U const & aOther)
        {
            mWord = aOther.mWord;
        }
    };

	inline TColor8U const operator+(TColor8U const & ca, TColor8U const & cb) { return TColor8U(ca.r + cb.r, ca.g + cb.g, ca.b + cb.b, ca.a + cb.a); };
	inline TColor8U const operator-(TColor8U const & ca, TColor8U const & cb) { return TColor8U(ca.r - cb.r, ca.g - cb.g, ca.b - cb.b, ca.a - cb.a); };
	inline TColor8U const & operator+=(TColor8U & ca, TColor8U const & cb) { ca.r += cb.r; ca.g += cb.g; ca.b += cb.b; ca.a += cb.a; return ca; };
	inline TColor8U const & operator-=(TColor8U & ca, TColor8U const & cb) { ca.r -= cb.r; ca.g -= cb.g; ca.b -= cb.b; ca.a -= cb.a; return ca; };
};


#endif // __SBX_COLOR_H__H__