#ifndef NDEBUGVECTOR_H
#define NDEBUGVECTOR_H
//------------------------------------------------------------------------------
/**
    @class nDebugVector
    @ingroup NebulaDebugSystem
    
    Represents a graphical debug vector
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "mathlib/vector.h"

#include "ndebug/ndebuggraphicelement.h"

//------------------------------------------------------------------------------
class nGfxServer2;

//------------------------------------------------------------------------------
class nDebugVector : public nDebugGraphicElement
{
public:
    /// constructor
    nDebugVector();
    /// destructor
    ~nDebugVector();
    /// set the center of the cross
    void SetCenter( const vector3 & point );
    /// set the size of cross
    void SetDirection( const vector3 & dir );
    /// set the size of cross
    void SetSize( float size );
    /// draw the vector
    virtual void Draw( nGfxServer2 * const gfxServer );
private:
    vector3 center;
    vector3 direction;
    float arrowSize;
};

//------------------------------------------------------------------------------
#endif//NDEBUGVECTOR_H
