#ifndef N_GFXUTILS_H
#define N_GFXUTILS_H
//-----------------------------------------------------------------------------
/**
    @class nGfxUtils
    @ingroup NebulaGraphicsSystem

    @brief conversions between relative and absolute coordinates
    Utils for converting between screensizes and/or widget sizes
*/

#include "gfx2/ngfxserver2.h"

//-----------------------------------------------------------------------------
class nGfxUtils
{
public:

    inline static uint absx(float rel, uint totalsize = 0) 
    {
	    if (totalsize)
        return (uint)(rel * totalsize);
        else
        return (uint)(rel * (nGfxServer2::Instance()->GetDisplayMode().GetWidth()));
    }

    inline static uint absy(float rel, uint totalsize = 0) 
    {
	    if (totalsize)
        return (uint)(rel * totalsize);
        else
	    return (uint)(rel * float(nGfxServer2::Instance()->GetDisplayMode().GetHeight()));
    }

    inline static float relx(uint abs, uint totalsize=0) 
    {
        uint x = totalsize;
        if (!x)
            x = (uint) nGfxServer2::Instance()->GetDisplayMode().GetWidth();
        float res = (float)abs / (float)x;
        return res;
    }

    inline static float rely(uint abs, uint totalsize=0) 
    {
        uint y = totalsize;
        if (!y)
            y = (uint) nGfxServer2::Instance()->GetDisplayMode().GetHeight();
        float res = (float)abs / (float)y;
        return res;
    }

    inline static vector4 color24(uint r, uint g, uint b)
    {
        return vector4(relx(r,255), relx(g,255), relx(b,255), 1.0f);
    }
};

//-----------------------------------------------------------------------------
#endif
