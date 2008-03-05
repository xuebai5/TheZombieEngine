#ifndef NDEBUGCAPSULE_H
#define NDEBUGCAPSULE_H
//------------------------------------------------------------------------------
/**
    @class nDebugCapsule
    @ingroup NebulaDebugSystem

    Represents a graphical debug capsule

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "mathlib/vector.h"

#include "ndebug/ndebuggraphicelement.h"

//------------------------------------------------------------------------------
class nGfxServer2;

//------------------------------------------------------------------------------
class nDebugCapsule : public nDebugGraphicElement
{
public:
    /// constructor
    nDebugCapsule();
    /// destructor
    ~nDebugCapsule();
    /// set the center of the capsule
    void SetCenter( const vector3 & point );
    /// set the radius of the capsule
    void SetRadius( const float value );
    /// set the length of the capsule
    void SetLength( const float value );
    /// set the orientation of the capsule
    void SetOrientation( const quaternion & value );
    /// draw the sphere
    virtual void Draw( nGfxServer2 * const gfxServer );
private:
    quaternion orientation;
    vector3 center;
    float radius;
    float length;
};

//------------------------------------------------------------------------------
#endif//NDEBUGCAPSULE_H
