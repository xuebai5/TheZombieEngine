#ifndef N_PHYGEOMCYLINDER_H
#define N_PHYGEOMCYLINDER_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomCylinder
    @ingroup NebulaPhysicsSystem
    @brief A Box physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Geometric Cylinder
    
    @cppclass nPhyGeomCylinder
    
    @superclass nPhysicsGeom

    @classinfo A Cylinder physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------

class nPhyGeomCylinder : public nPhysicsGeom 
{
public:
    static const phyreal DefaultLength;

    static const phyreal DefaultRadius;

    /// constructor
    nPhyGeomCylinder();

    /// destructor
    ~nPhyGeomCylinder();

    /// gets the length of the cylinder
    phyreal GetLength() const;

    /// gets the radius of the cylinder
    phyreal GetRadius() const;

    /// sets the length of a cylinder
    void SetLength( phyreal length );

    /// sets the radius of a cylinder
    void SetRadius( phyreal radius );

#ifndef NGAME
    /// draws the geomtery
    void Draw( nGfxServer2 *server );
#endif

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// scales the geometry
    void Scale( const phyreal factor );

private:
    /// creates the geometry
    void Create();

};

//-----------------------------------------------------------------------------
/**
    Gets the length of the cylinder

    @return length of the cylinder

    history:
        - 28-Sep-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
phyreal nPhyGeomCylinder::GetLength() const
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    return phyGetLengthCylinder( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Gets the radius of the cylinder

    @return radius of the cylinder

    history:
        - 28-Sep-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
phyreal nPhyGeomCylinder::GetRadius() const
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    return phyGetRadiusCylinder( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Sets the length of a cylinder

    @param length new length of the cylinder

    history:
        - 28-Sep-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomCylinder::SetLength( phyreal length )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
    
    phySetLengthCylinder( this->Id(), length );
}

//-----------------------------------------------------------------------------
/**
    Sets the radius of a cylinder

    @param radius new radius of the cylinder

    history:
        - 28-Sep-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomCylinder::SetRadius( phyreal radius )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
    
    phySetRadiusCylinder( this->Id(), radius );
}

//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
     - 11-May-2005   David Reyes    created
     - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomCylinder::Scale( const phyreal factor )
{
    this->SetRadius( this->GetRadius() * factor );

    this->SetLength( this->GetLength() * factor );
}

#endif 
