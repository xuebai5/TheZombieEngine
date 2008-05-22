#ifndef N_PHYGEOMSPHERE_H
#define N_PHYGEOMSPHERE_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomSphere
    @ingroup NebulaPhysicsSystem
    @brief A sphere physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Sphere Geometry
    
    @cppclass nPhyGeomSphere
    
    @superclass nPhysicsGeom

    @classinfo A sphere physics geometry.
*/    

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------
class nPhyGeomSphere : public nPhysicsGeom 
{
public:
    static const phyreal DefaultRadius;

    /// constructor
    nPhyGeomSphere();

    /// creates the geometry
    void Create();

    /// destructor
    ~nPhyGeomSphere();

#ifndef NGAME
    /// draws the geometry
    void Draw( nGfxServer2 *server );
#endif

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// scales the geometry
    void Scale( const phyreal factor );

    /// begin:scripting
    
    /// sets the sphere radius
    void SetRadius(phyreal);

    /// returns the sphere radius
    phyreal GetRadius() const;

    /// end:scripting
};

//-----------------------------------------------------------------------------
/**
    Sets the sphere radius

    @param radius   real with the length of the radius

    history:
        - 23-Sep-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyGeomSphere::SetRadius( phyreal radius )
{

    n_assert2( this->Id() != NoValidID , "No valid geometry." );

    phySetRadiusSphere( this->Id(), radius );
}

//-----------------------------------------------------------------------------
/**
    Returns the sphere radius

    @return radius   real with the length of the radius

    history:
        - 23-Sep-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal nPhyGeomSphere::GetRadius() const
{

    n_assert2( this->Id() != NoValidID , "No valid geometry." );

    return phyGetRadiusSphere( this->Id() );
}

//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
        - 12-May-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyGeomSphere::Scale( const phyreal factor )
{
    this->SetRadius( this->GetRadius() * factor );
}

#endif 
