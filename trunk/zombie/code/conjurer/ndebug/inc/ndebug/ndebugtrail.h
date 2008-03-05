#ifndef NDEBUGTRAIL_H
#define NDEBUGTRAIL_H
//------------------------------------------------------------------------------
/**
    @class nDebugTrail
    @ingroup NebulaDebugSystem
    
    Represents a graphical debug trail
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "mathlib/vector.h"
#include "util/narray.h"

#include "ndebug/ndebuggraphicelement.h"

//------------------------------------------------------------------------------
class nGfxServer2;

//------------------------------------------------------------------------------
class nDebugTrail : public nDebugGraphicElement
{
public:
    /// constructor
    nDebugTrail();
    /// destructor
    ~nDebugTrail();
    /// insert a new point in the trail
    void InsertPoint( const vector3 & point );
    /// draw the trail
    virtual void Draw( nGfxServer2 * const gfxServer );
private:
    nArray<vector3> points;
};

//------------------------------------------------------------------------------
#endif//NDEBUGTRAIL_H
