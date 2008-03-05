#ifndef NDEBUGSPHERE_H
#define NDEBUGSPHERE_H
//------------------------------------------------------------------------------
/**
    @class nDebugSphere
    @ingroup NebulaDebugSystem

    Represents a graphical debug sphere

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "mathlib/vector.h"

#include "ndebug/ndebuggraphicelement.h"

//------------------------------------------------------------------------------
class nGfxServer2;

//------------------------------------------------------------------------------
class nDebugSphere : public nDebugGraphicElement
{
public:
    /// constructor
    nDebugSphere();
    /// destructor
    ~nDebugSphere();
    /// set the center of the sphere
    void SetCenter( const vector3 & point );
    /// set the radius of the sphere
    void SetRadius( const float value );
    /// draw the sphere
    virtual void Draw( nGfxServer2 * const gfxServer );
private:
    vector3 center;
    float radius;
};

//------------------------------------------------------------------------------
#endif//NDEBUGSPHERE_H
