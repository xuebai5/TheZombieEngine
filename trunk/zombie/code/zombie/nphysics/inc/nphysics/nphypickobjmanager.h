#ifndef N_PHYPICKOBJMANAGER_H
#define N_PHYPICKOBJMANAGER_H

//-----------------------------------------------------------------------------
/**
    @class nPhyPickObjManager
    @ingroup NebulaPhysicsSystem
    @brief Manager for the picking object functionality.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Picking Object Manager
    
    @cppclass nPhyPickObjManager
    
    @superclass nObject

    @classinfo Manager for the picking object functionality.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nobject.h"
#include "nphysics/nphysicsconfig.h"

#ifndef NGAME
//-----------------------------------------------------------------------------

class nPhysicsWorld;
class ncPhysicsObj;
class nPhyGeomRay;

//-----------------------------------------------------------------------------
class nPhyPickObjManager : public nObject
{
public:
    /// default constructor
    nPhyPickObjManager();

    /// destructor
    ~nPhyPickObjManager();

    /// creates the parallel world for picking no physic objects
    void CreateWorld();

    /// returns the world for a no physic objects
    nPhysicsWorld* GetWorld();

    /// returns the entity picked if any
    ncPhysicsObj* GetPickedObj( const vector3& startposition, const vector3& direction );

    /// set the filter to pick by categories
    void SetCategoriesToCollideWith( int categories );

    /// begin:scripting

    /// end:scripting

private:

    /// stores the max distance where a object can be picked
    static const phyreal maxRayDistance;

    /// store the world created for non physic objects
    nPhysicsWorld* world;

    /// stores the geometry to be used for picking objects
    nPhyGeomRay* ray;

    /// stores the categories pickeable
    int pickeableCategories;
};

//-----------------------------------------------------------------------------
/**
    Returns the world for a no physic objects.

    @return world

    history:
        - 07-Apr-2005   Zombie         created
*/
inline
nPhysicsWorld* nPhyPickObjManager::GetWorld()
{
    return this->world;
}

//-----------------------------------------------------------------------------
/**
    Set the filter to pick by categories.

    @param categories new set of categories flags

    history:
        - 08-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyPickObjManager::SetCategoriesToCollideWith( int categories )
{
    this->pickeableCategories = categories;
}

#else

class nPhyPickObjManager : public nObject 
{
public:
    nPhyPickObjManager() {}
};

#endif // !NGAME

#endif 
