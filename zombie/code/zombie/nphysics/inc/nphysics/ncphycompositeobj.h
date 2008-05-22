#ifndef NC_PHYCOMPOSITEOBJ_H
#define NC_PHYCOMPOSITEOBJ_H
//-----------------------------------------------------------------------------
/**
    @class ncPhyCompositeObj
    @ingroup NebulaPhysicsSystem
    @brief An abstract physics object where the geometries within are encapsulated 
        by transforms and share the same body

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Collision Composite
    
    @cppclass ncPhyCompositeObj
    
    @superclass ncPhysicsObj

    @classinfo An abstract physics object where the geometries within are encapsulated 
        by transforms and share the same body.
*/

#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphysicsgeom.h"
#include "kernel/ncmdargtypes.h"

//-----------------------------------------------------------------------------

class nPhyRigidBody;

//-----------------------------------------------------------------------------
class ncPhyCompositeObj : public ncPhysicsObj
{

    NCOMPONENT_DECLARE(ncPhyCompositeObj,ncPhysicsObj);

public:
    /// constructor
    ncPhyCompositeObj();

    /// destructor
    virtual ~ncPhyCompositeObj();

    /// sets the position of the rigid body
    virtual void SetPosition( const vector3& newposition );

    /// returns a geometry based on the index
    nPhysicsGeom* GetGeometry( int index ) const;

    /// returns the number of geometries
    int GetNumGeometries() const;

    /// sets the orientation of the rigid body
    virtual void SetRotation( const matrix33& neworientation );

    /// sets the euler orientation
    virtual void SetRotation( phyreal ax, phyreal ay, phyreal az );

    /// returns current position
    void GetPosition( vector3& position ) const;

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// object chunk persistency
    bool SaveChunk(nPersistServer* ps);

    /// sets the body of this object
    void SetBody( nPhyRigidBody* body );

    /// enables the physic object
    virtual void Enable();

    /// disables the physic object
    virtual void Disable();

    /// moves the physic object to another space
    virtual void MoveToSpace( nPhySpace* newspace );

    /// checks if collision with a geometry
    virtual int Collide( const nPhysicsGeom* geom,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// checks if collision with other physics objects
    virtual int Collide( const ncPhysicsObj* obj,
        int numContacts, nPhyCollide::nContact* contact ) const;

    /// sets the material for all the geometries contain in the object
    virtual void SetMaterial( nPhyMaterial::idmaterial material );

    /// returns the bounding box of this geometry
    virtual void GetAABB( nPhysicsAABB& boundingbox );

    /// creates the object
    virtual void Create( nPhysicsWorld* world );

    /// returns the orientation of the object
    void GetOrientation( matrix33& rotation );

    /// begin:scripting
    
    /// adds a geometry to the object
   virtual void Add(nPhysicsGeom*);
    /// removes a geometry from the object
   virtual void Remove(nPhysicsGeom*);
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

    /// sets if the creation of an object's sub-space
    void AllowCreateSubSpace( const bool is );

#ifndef NGAME

    /// removes a collide with
    virtual void RemovesCollidesWith( int category );

#endif

protected:
    
    // returns if it's allowed to use a sub space
    const bool IsSubSpaceAllowed() const;

    /// creates composite space
    void CreateSpace();

    /// type container geometries
    typedef nKeyArray< nPhysicsGeom* > tContainerGeometries;

    /// geometries of this object
    tContainerGeometries containerGeometries;

    /// adds the geomery to the container
    void AddContainer( nPhysicsGeom* geometry );

    /// composite space
    nPhySpace* objectSpace;

private:

    /// removes the geometry from the container
    void RemoveContainer( nPhysicsGeom* geometry );

    /// number of initial geometries space
    static const int NumInitialGeometries = 10;

    /// growth pace
    static const int NumGrowthGeometries = 10;

    /// resets the container
    void ResetsContainer();

    /// sotores if create a sub-space is allowed
    bool subSpaceAllowed;

};

//-----------------------------------------------------------------------------
/**
    Resets the container

    history:
        - 29-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inline
*/
inline
void ncPhyCompositeObj::ResetsContainer() 
{
    this->containerGeometries.Clear();
}

//-----------------------------------------------------------------------------
/**
    Returns a geometry based on the index

    @param index    index to the geometry

    history:
        - 29-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inline
*/
inline
nPhysicsGeom* ncPhyCompositeObj::GetGeometry( int index ) const
{
    return this->containerGeometries.GetElementAt( index );
}

//-----------------------------------------------------------------------------
/**
    Returns the number of geometries.

    @return number of the geometries in this object

    history:
        - 29-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inline
*/
inline
int ncPhyCompositeObj::GetNumGeometries() const
{
    return this->containerGeometries.Size();
}

//-----------------------------------------------------------------------------
/**
    Returns if it's allowed to use a sub space.

    @return true/false

    history:
        - 06-Sep-2004   Zombie         created
*/
inline
const bool ncPhyCompositeObj::IsSubSpaceAllowed() const
{
    return this->subSpaceAllowed;
}

//-----------------------------------------------------------------------------
/**
    Returns current position.

    @param position vector3 to be fill with the actual position of the object.

    history:
        - 19-Oct-2004   Zombie         created
        - 21-Feb-2004   Zombie         inlined
*/
inline
void ncPhyCompositeObj::GetPosition( vector3& position ) const
{
#ifndef NGAME
    if( !this->containerGeometries.Size() )
    {
        return;
    }
#endif
    this->containerGeometries.GetElementAt(0)->GetPosition(position);
}

#endif
