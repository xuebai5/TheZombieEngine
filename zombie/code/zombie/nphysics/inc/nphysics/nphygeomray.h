#ifndef N_PHYGEOMRAY_H
#define N_PHYGEOMRAY_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomRay
    @ingroup NebulaPhysicsSystem
    @brief A ray physics geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Ray Geometry
    
    @cppclass nPhyGeomRay
    
    @superclass nPhysicsGeom

    @classinfo A ray physics geometry.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------
class nPhyGeomRay : public nPhysicsGeom 
{
public:
    static const phyreal DefaultLength;

    /// constructor
    nPhyGeomRay();

    /// creates the geometry
    void Create();

    /// destructor
    ~nPhyGeomRay();

    /// sets the position of this geometry
    void SetPosition( const vector3& newposition );

    /// returns current position
    void GetPosition( vector3& position ) const;

    /// returns the end position of the ray
    void GetEndPosition( vector3& position ) const;

#ifndef NGAME
    /// draws the ray
    void Draw( nGfxServer2* server );
#endif

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// scales the geometry
    void Scale( const phyreal factor );

    /// begin:scripting

    /// sets the length of the ray
    void SetLength(phyreal);

    /// returns the length of the ray
    phyreal GetLength()const;

    /// sets the direction of the ray
    void SetDirection(const vector3&);

    /// gets the direction of the ray
    void GetDirection(vector3&) const;

    /// end:scripting

private:
    /// storage of the length of the ray
    phyreal lengthRay;
};

//-----------------------------------------------------------------------------
/**
    Returns the length of the ray

    @return length of the ray

    history:
        - 24-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
phyreal nPhyGeomRay::GetLength() const
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    return this->lengthRay;
}

//-----------------------------------------------------------------------------
/**
    Sets the direction of the ray

    @param direction    new direction of the ray

    history:
        - 24-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomRay::SetDirection( const vector3& direction )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
    
    phySetRayDirection( this->Id(), direction );
}

//-----------------------------------------------------------------------------
/**
    Gets the direction of the ray

    @param direction    direction of the ray

    history:
        - 24-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomRay::GetDirection( vector3& direction ) const
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );
    
    phyGetRayDirection( this->Id(), direction );
}

//-----------------------------------------------------------------------------
/**
    Sets the position of this geometry

    @param newposition  new start position of the ray

    history:
        - 24-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomRay::SetPosition( const vector3& newposition ) 
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
 
    phySetRayStartPosition( this->Id(), newposition );
}

//-----------------------------------------------------------------------------
/**
    Gets the position of this geometry

    @param nweposition  start position of the ray

    history:
        - 24-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomRay::GetPosition( vector3& position ) const
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );
 
    phyGetRayStartPosition( this->Id(), position );
}

//-----------------------------------------------------------------------------
/**
    Sets the length of the ray

    @param length     new length of the ray

    history:
        - 24-Sep-2004   David Reyes    created
        - 11-May-2005   David Reyes    inlined
*/
inline
void nPhyGeomRay::SetLength( phyreal length )
{
    n_assert2( this->Id() != NoValidID, "No valid geometry" );

    n_assert2( length > 0, "Length of a ray has to be bigger than zero" );

    phySetRayLength( this->Id(), length );

    this->lengthRay = length;
}

#endif 
