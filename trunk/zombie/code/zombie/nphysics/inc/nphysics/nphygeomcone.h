#ifndef N_PHYGEOMCONE_H
#define N_PHYGEOMCONE_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomCone
    @ingroup NebulaPhysicsSystem
    @brief A Cone physics geometry

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Geometric Cone
    
    @cppclass nPhyGeomCone
    
    @superclass nPhysicsGeom

    @classinfo A Cone physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------

class nPhyGeomCone : public nPhysicsGeom 
{
public:
    static const phyreal DefaultLength;

    static const phyreal DefaultRadius;

    /// constructor
    nPhyGeomCone();

    /// destructor
    ~nPhyGeomCone();

    /// gets the length of the cone
    phyreal GetLength() const;

    /// gets the radius of the cone
    phyreal GetRadius() const;

    /// sets the length of a cone
    void SetLength( phyreal length );

    /// sets the radius of a cone
    void SetRadius( phyreal radius );

#ifndef NGAME
    /// draws the geometry
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
    Gets the length of the cone

    @return length of the cone

    history:
        - 15-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal nPhyGeomCone::GetLength() const
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    return phyGetLengthCone( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Gets the radius of the cone

    @return radius of the cone

    history:
        - 15-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal nPhyGeomCone::GetRadius() const
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    return phyGetRadiusCone( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Sets the length of a cone

    @param length new length of the cone

    history:
        - 15-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyGeomCone::SetLength( phyreal length )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
    
    phySetLengthCone( this->Id(), length );
}

//-----------------------------------------------------------------------------
/**
    Sets the radius of a cone

    @param radius new radius of the cone

    history:
        - 15-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyGeomCone::SetRadius( phyreal radius )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
    
    phySetRadiusCone( this->Id(), radius );
}

#endif 
