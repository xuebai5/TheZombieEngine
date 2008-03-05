#ifndef N_PHYGEOMPLANE_H
#define N_PHYGEOMPLANE_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomPlane
    @ingroup NebulaPhysicsSystem
    @brief A plane physics geometry

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Geomtry Plane
    
    @cppclass nPhyGeomPlane
    
    @superclass nPhysicsGeom

    @classinfo A plane physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------
class nPhyGeomPlane : public nPhysicsGeom 
{
public:
    /// constructor
    nPhyGeomPlane();

    /// creates the geometry
    void Create();

    /// destructor
    ~nPhyGeomPlane();

    /// sets the position of this geometry (doesn't affect)
    void SetPosition( const vector3& );

    /// returns current position (doesn't affect)
    void GetPosition( vector3& ) const;

    /// sets the orientation of this geometry (doesn't affect)
    void SetOrientation( const matrix33& );

    /// returns current orientation (doesn't affect)
    void GetOrientation( matrix33& ) const;

#ifndef NGAME
    /// draws the plane
    void Draw( nGfxServer2* server );
#endif

    /// sets plane equation
    void SetEquation( const vector4& equation );

    /// gets the plane equation
    const vector4& GetEquation() const;

private:
    /// normal and offset of the plane
    vector4 planeequation; // x,y,z,d
};

//-----------------------------------------------------------------------------
/**
    Gets the plane equation.

    @return plane equation coeficients

    history:
        - 12-Jan-2005   David Reyes    created
        - 27-Jan-2005   David Reyes    inlined
*/
inline
const vector4& nPhyGeomPlane::GetEquation() const
{
    n_assert2( this->Id() != NoValidID, "Error the plane hasn't been created" );

    return this->planeequation;
}

#endif 
