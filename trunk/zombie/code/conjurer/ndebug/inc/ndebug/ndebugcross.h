#ifndef NDEBUGCROSS_H
#define NDEBUGCROSS_H
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
class nDebugCross : public nDebugGraphicElement
{
public:
    /// constructor
    nDebugCross();
    /// destructor
    ~nDebugCross();
    /// set the center of the cross
    void SetCenter( const vector3 & point );
    /// set the size of cross
    void SetSize( const float value );
    /// draw the cross
    virtual void Draw( nGfxServer2 * const gfxServer );
private:
    vector3 center;
    float size;
};

//------------------------------------------------------------------------------
/**
    @param value the new size
*/
inline
void
nDebugCross::SetSize( const float value )
{
    this->size = value;
}

//------------------------------------------------------------------------------
#endif//NDEBUGCROSS_H
