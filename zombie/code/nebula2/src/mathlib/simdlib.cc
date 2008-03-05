#include "precompiled/pchnkernel.h"
#include "mathlib/simdlib.h"
#include <emmintrin.h>

namespace SIMDLib
{

//------------------------------------------------------------------------------
/**
    @brief SIMD add 16 bit unsigned
    @param dst Destination
    @param src Source
    @param op The value to add
    @param numElements The number of elements
*/
void Add16u(ushort* dst, const ushort *src, const ushort op, int numElements)
{
    const int loopCount = numElements >> 2; // there are 4 ushort(16) in __m64

    const __m64* in  = (const __m64*) src;
    __m64* out = (__m64*) dst;

    __m64 tmp; 
    _mm_empty(); //Empty MMX state (prevents collisions with floating-point operations)
    __m64 val = _mm_set_pi16(op, op, op, op);

    for ( int i = 0;  i < loopCount; ++i )
    {
        tmp  = _mm_add_pi16(  val, *in);
        *out = tmp;
        ++in;
        ++out;
    }
    _mm_empty(); //Empty MMX state (prevents collisions with floating-point operations)

    src = (const ushort*)in;
    dst = (ushort*)out;

    const int remainder = numElements & 3;
    switch( remainder)
    {
        case 3:
            dst[2] = src[2]+op;
            // not break
        case 2:
            dst[1] = src[1]+op;
            // not break
        case 1:
            dst[0] = src[0]+op;
            // not break
        default:
            ;
            //empty
    }

}

//------------------------------------------------------------------------------
/**
    @brief SIMD add 16 bit unsigned oftimize for a triads
    @param dst Destination
    @param src Source
    @param op The value to add
    @param numTriads The number of triads
*/
void Add16u_tuple3(ushort* dst, const ushort *src, const ushort op, int numVertex)
{
    const int loopCount = numVertex >> 2; // there are 4 ushort(16) in __m64


    const __m64* in  = (const __m64*) src;
    __m64* out = (__m64*) dst;

    _mm_empty(); //Empty MMX state (prevents collisions with floating-point operations)
    __m64 val = _mm_set_pi16(op, op, op, op);

    for ( int i = 0;  i < loopCount; ++i )
    {
        // Copy 12 indices
        out[0] = _mm_add_pi16( val, in[0]);
        out[1] = _mm_add_pi16( val, in[1]);
        out[2] = _mm_add_pi16( val, in[2]);

        in+=3;
        out+=3;
    }


    src = (const ushort*)in;
    dst = (ushort*)out;

    const int remainder = numVertex & 3;
    switch( remainder)
    {
        case 3:
            out[0] = _mm_add_pi16( val, in[0]);
            out[1] = _mm_add_pi16( val, in[1]);
            dst[8] = src[8]+op;
            break;
        case 2:
            out[0] = _mm_add_pi16( val, in[0]);
            dst[4] = src[4]+op;
            dst[5] = src[5]+op;
            break;
        case 1:
            dst[0] = src[0]+op;
            dst[1] = src[1]+op;
            dst[2] = src[2]+op;
            // not break
        default:
            ;
            //empty
    }
   _mm_empty(); //Empty MMX state (prevents collisions with floating-point operations)

}

}; //namespace SIMDLib
