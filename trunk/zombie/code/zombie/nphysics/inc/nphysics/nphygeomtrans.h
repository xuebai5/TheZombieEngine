#ifndef N_PHYGEOMTRANS_H
#define N_PHYGEOMTRANS_H

//-----------------------------------------------------------------------------
/**
    @class nPhyGeomTrans
    @ingroup NebulaPhysicsSystem
    @brief A Geometry Transform is a geometry that encapsulates another geom.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Transform Geometry
    
    @cppclass nPhyGeomTrans
    
    @superclass nPhysicsGeom

    @classinfo A geometry that encapsulates another geom.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------
class nPhyGeomTrans : public nPhysicsGeom 
{
public:
    /// constructor
    nPhyGeomTrans();

    /// destructor
    ~nPhyGeomTrans();

    /// get the encapsulated geometry
    nPhysicsGeom* GetGeometry() const;

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// returns the geometry material
    virtual const nPhyMaterial::idmaterial GetMaterial() const;

    /// gets the categories where the geometry belongs
    virtual int GetCategories() const;

    /// gets the categories wich the geom will collide with
    virtual int GetCollidesWith() const;

    /// sets the categories where the geometry belongs
    virtual void SetCategories( int categories );
   
    /// sets the categories wich the geom will collide with
    virtual void SetCollidesWith( int categories );

    /// returns the owner if any
    ncPhysicsObj* GetOwner() const;

#ifndef NGAME
    /// draw
    void Draw( nGfxServer2 *server );
#endif

    /// begin:scripting
    
    /// adds a geometry to be encapsulated
    void AddGeometry(nPhysicsGeom*);

    /// end:scripting

    /// scales the geometry
    void Scale( const phyreal factor );

private:

    /// creates the geometry
    void Create();

    /// stores the geometry added to this one
    nPhysicsGeom* encapsulatedGeom;

};

//-----------------------------------------------------------------------------
/**
    Get the encapsulated geometry

    @return encapsulated geometry

    history:
        - 18-Oct-2004   David Reyes    created
        - 12-May-2005   David Reyes    inline
*/
inline
nPhysicsGeom* nPhyGeomTrans::GetGeometry() const
{
    return this->encapsulatedGeom;
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry material.

    @return material id

    history:
        - 16-May-2004   David Reyes    created
*/
inline
const nPhyMaterial::idmaterial nPhyGeomTrans::GetMaterial() const
{
    n_assert2( this->GetGeometry(), "Doesn't have a geometry." );

    return this->GetGeometry()->GetMaterial();
}

//-----------------------------------------------------------------------------
/**
    Gets the categories where the geometry belongs.

    @return categories flags

    history:
        - 16-May-2004   David Reyes    created
*/
inline
int nPhyGeomTrans::GetCategories() const
{
    n_assert2( this->GetGeometry(), "Doesn't have a geometry." );

    return this->GetGeometry()->GetCategories();
}

//-----------------------------------------------------------------------------
/**
    Gets the categories wich the geom will collide with.

    @return categories flags

    history:
        - 16-May-2004   David Reyes    created
*/
inline
int nPhyGeomTrans::GetCollidesWith() const
{
    n_assert2( this->GetGeometry(), "Doesn't have a geometry." );

    return this->GetGeometry()->GetCollidesWith();
}

//-----------------------------------------------------------------------------
/**
    Returns the owner if any.

    @return a physic object

    history:
        - 16-May-2004   David Reyes    created
*/
inline
ncPhysicsObj* nPhyGeomTrans::GetOwner() const
{
    n_assert2( this->GetGeometry(), "Doesn't have a geometry." );

    return this->GetGeometry()->GetOwner();
}

//-----------------------------------------------------------------------------
/**
    Sets the categories where the geometry belongs.

    @param categories new categories flags

    history:
        - 22-Jul-2005   David Reyes    created
*/
inline
void nPhyGeomTrans::SetCategories( int categories )
{
    if( this->GetGeometry() )
        this->GetGeometry()->SetCategories( categories );

    this->nPhysicsGeom::SetCategories( categories );
}

//-----------------------------------------------------------------------------
/**
    Sets the categories wich the geom will collide with.

    @param categories new categories flags

    history:
        - 22-Jul-2005   David Reyes    created
*/
inline
void nPhyGeomTrans::SetCollidesWith( int categories )
{
    if( this->GetGeometry() )
        this->GetGeometry()->SetCollidesWith( categories );

    this->nPhysicsGeom::SetCollidesWith( categories );
}

#endif 
