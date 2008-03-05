#ifndef NDEBUGTEXT_H
#define NDEBUGTEXT_H
//------------------------------------------------------------------------------
/**
    @class nDebugCross
    @ingroup NebulaDebugSystem
    
    Represents a graphical debug cross
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "mathlib/vector.h"

#include "ndebug/ndebuggraphicelement.h"

//------------------------------------------------------------------------------
class nGfxServer2;

//------------------------------------------------------------------------------
class nDebugText : public nDebugGraphicElement
{
public:
    /// constructor
    nDebugText();
    /// destructor
    ~nDebugText();
    /// set the position text from ( -1,-1) to (1,1)
    void SetScreenPos( float  x, float y );
    /// set text
    void SetText( const nString& string);
    /// set text
    void SetText( const char* string);
    /// format string printf-style
    void __cdecl Format(const char* fmtString, ...)
            __attribute__((format(printf,2,3)));
    /// draw the text
    virtual void Draw( nGfxServer2 * const gfxServer );
private:
    float x,y;
    nString text;
};

//------------------------------------------------------------------------------
#endif//NDEBUGCROSS_H
