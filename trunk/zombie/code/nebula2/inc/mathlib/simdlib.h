#ifndef _SIMDLIB_H
#define _SIMDLIB_H
//------------------------------------------------------------------------------
/**
    @namespace SIMDLib
    @ingroup NebulaMathDataTypes
    @autor cristobal.castillo

    @brief A library to Single Instruction Multiple Data.

    (C) 2006 Conjurer Services, S.A.
*/

namespace SIMDLib
{
    /// Add 16Bits unsigned
    void Add16u(ushort* dst, const ushort *src, const ushort op, int numElements);
    /// Add 16Bits unsigned , optimize for triad
    void Add16u_tuple3(ushort* dst, const ushort *src, const ushort op, int numTriads);
};
//------------------------------------------------------------------------------
#endif //!_SIMDLIB_H

