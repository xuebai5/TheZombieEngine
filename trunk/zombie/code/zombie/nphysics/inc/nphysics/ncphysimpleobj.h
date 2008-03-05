#ifndef NC_PHYSIMPLEOBJ_H
#define NC_PHYSIMPLEOBJ_H
//-----------------------------------------------------------------------------
/**
    @class ncPhySimpleObj
    @ingroup NebulaPhysicsSystem
    @brief An abstract simple object

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Simple Object.
    
    @cppclass ncPhySimpleObj
    
    @superclass ncPhysicsObj

    @classinfo An abstract simple object.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------

class nPhysicsGeom;
class nPhyRigidBody;

//-----------------------------------------------------------------------------
class ncPhySimpleObj : public ncPhysicsObj
{

    NCOMPONENT_DECLARE(ncPhySimpleObj,ncPhysicsObj);

public:
    /// constructor
    ncPhySimpleObj();

    /// destructor
    virtual ~ncPhySimpleObj();

    /// returns the geometry
    nPhysicsGeom* GetGeometry() const;

    /// enables the physic object
    virtual void Enable();

    /// disables the physic object
    virtual void Disable();

    /// picks an object trough a given geometry
    geomid ObjectPicking( const nPhysicsGeom* geom, nPhyCollide::nContact& contact );

    /// moves the physic object to another space
    void MoveToSpace( nPhySpace* newspace );

    /// checks if collision with other physics object
    int Collide( const ncPhysicsObj* obj,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// checks if collision with a geometry
    int Collide( const nPhysicsGeom* geom,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// sets the position of this physic object
    virtual void SetPosition( const vector3& newposition );

    /// sets the rotation of this physic object
    virtual void SetRotation( const matrix33& newrotation );

    /// returns current position
    void GetPosition( vector3& position ) const;

    /// gets the rotation of this physic object
    void GetOrientation( matrix33& rotation );

    /// sets the material for all the geometries contain in the object
    void SetMaterial( nPhyMaterial::idmaterial material );

    /// returns the number of geometries
    int GetNumGeometries() const;

    /// returns a geometry based on the index
    nPhysicsGeom* GetGeometry( int index ) const;

    /// returns the bounding box of this geometry
    virtual void GetAABB( nPhysicsAABB& boundingbox );

    /// creates the object
    virtual void Create( nPhysicsWorld* world );

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// object chunk persistency
    bool SaveChunk(nPersistServer *ps);

    /// begin:scripting

    /// sets the geometry
    void SetGeometry(nPhysicsGeom*);
    /// creates an object composited
    nPhysicsGeom* CreateGeometryObj(const nString&);

    /// end:scripting

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// scales the object
    virtual void Scale( const phyreal factor );

    /// checks if the object collides with anything in his own world.
    virtual int Collide( int numContacts, nPhyCollide::nContact* contact ) const;

    /// sets the categories where the object belongs
    virtual void SetCategories( int categories );
    
    /// sets the categories wich the object will collide with
    virtual void SetCollidesWith( int categories );

#ifndef NGAME

    /// adds a collide with
    virtual void RemovesCollidesWith( int category );

#endif

private:
    /// stores the geometry
    nPhysicsGeom* geometry;

};

//-----------------------------------------------------------------------------
/**
    Returns the geometry.

    @return object geometry

    history:
        - 29-Nov-2004   David Reyes    created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
nPhysicsGeom* ncPhySimpleObj::GetGeometry() const
{
    return this->geometry;
}

//-----------------------------------------------------------------------------
/**
    Returns the geometry.

    @param index    index to the geometry (it's ignored)
    @return object geometry

    history:
        - 29-Nov-2004   David Reyes    created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
nPhysicsGeom* ncPhySimpleObj::GetGeometry(int /*index*/) const
{
    return this->GetGeometry();
}

//-----------------------------------------------------------------------------
/**
    Returns the number of geometries.

    @return number of geometries

    history:
        - 29-Nov-2004   David Reyes    created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
int ncPhySimpleObj::GetNumGeometries() const
{
    if( this->geometry )
    {
        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Returns the bounding box of this geometry.

    @param boundingBox  bounding box

    history:
        - 30-Nov-2004   David Reyes    created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
void ncPhySimpleObj::GetAABB( nPhysicsAABB& boundingbox )
{
    n_assert2( this->geometry, "Not valid object" );

    this->geometry->GetAABB( boundingbox );
}

//-----------------------------------------------------------------------------
/**
    Sets the position of this physic object.

    @param newposition      world position

    history:
        - 29-Nov-2004   David Reyes    created
        - 27-Jan-2005   David Reyes    inline
*/
inline
void ncPhySimpleObj::SetPosition( const vector3& newposition )
{
    n_assert2( this->GetGeometry(), "Not valid object" );
    
    this->GetGeometry()->SetPosition( newposition );

    ncPhysicsObj::SetPosition( newposition );
}

//-----------------------------------------------------------------------------
/**
    Sets the rotation of this physic object.

    @param newrotation  orientation of the object

    history:
        - 29-Nov-2004   David Reyes    created
        - 27-Jan-2005   David Reyes    inline
*/
inline
void ncPhySimpleObj::SetRotation( const matrix33& newrotation )
{
    n_assert2( this->GetGeometry(), "Not valid object" );

    this->GetGeometry()->SetOrientation( newrotation );

    ncPhysicsObj::SetRotation( newrotation );
}

//-----------------------------------------------------------------------------
/**
    Returns current position.

    @param position     object position

    history:
        - 29-Nov-2004   David Reyes    created
        - 27-Jan-2005   David Reyes    inline
*/
inline
void ncPhySimpleObj::GetPosition( vector3& position ) const
{
    n_assert2( this->GetGeometry(), "Not valid object" );

    this->GetGeometry()->GetPosition( position );
}

//-----------------------------------------------------------------------------
/**
    Gets the rotation of this physic object.

    @param rotation matrix33 object orientation

    history:
        - 29-Nov-2004   David Reyes    created
*/
inline
void ncPhySimpleObj::GetOrientation( matrix33& rotation )
{
    this->GetGeometry()->GetOrientation( rotation );
}

//-----------------------------------------------------------------------------
/**
    Sets the material for all the geometries contain in the object.

    @param material     material

    history:
        - 29-Nov-2004   David Reyes    created
        - 27-Jan-2005   David Reyes    inline
*/
inline
void ncPhySimpleObj::SetMaterial( nPhyMaterial::idmaterial material )
{
    n_assert2( this->GetGeometry(), "Not valid object" );

    this->GetGeometry()->SetMaterial( material );
}

#endif

