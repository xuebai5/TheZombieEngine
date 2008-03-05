//-----------------------------------------------------------------------------
//  nphypickobjmanager_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"


#include "nphysics/nphypickobjmanager.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyPickObjManager, "nobject");

#ifndef NGAME
//-----------------------------------------------------------------------------

const phyreal nPhyPickObjManager::maxRayDistance( phyreal(1000000) );


//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 07-Apr-2005   David Reyes    created
*/
nPhyPickObjManager::nPhyPickObjManager() :
    world(0),
    ray(0),
    pickeableCategories(-1) // all categories by default
{
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 07-Apr-2005   David Reyes    created
*/
nPhyPickObjManager::~nPhyPickObjManager()
{
    if( this->world )
    {
        this->world->Release();
        this->world = 0;
    }

    if( this->ray )
    {
        this->ray->Release();
        this->ray = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the parallel world for picking no physic objects.

    history:
        - 07-Apr-2005   David Reyes    created
*/
void nPhyPickObjManager::CreateWorld()
{
    n_assert2( this->world == 0, "The world of picking it's already in place." );

    /// Instanciating the world
    this->world = static_cast<nPhysicsWorld*>(nKernelServer::Instance()->New( "nphysicsworld" ));

    n_assert2( this->world, "Failed to create the picking world." );

#ifndef NGAME
    /// So it's not included in the simulation
    this->world->SetHasBodies( false );
#endif

    /// creating the ray to collide
    this->ray = static_cast<nPhyGeomRay*>(nKernelServer::Instance()->New( "nphygeomray" ));

    n_assert2( this->ray, "Failed to create the picking geometry." );

    /// ollides with everything
    this->ray->SetCollidesWith( -1 );

    /// doesn't belong to any category
    this->ray->SetCategories(0);

    /// Initting ray length
    this->ray->SetLength( maxRayDistance );

    this->world->SetSpace( static_cast<nPhyGeomSpace*>(nKernelServer::Instance()->New( "nphygeomhashspace" )) );
}

//-----------------------------------------------------------------------------
/**
    Returns a picked object.

    @param startposition world position
    @param direction direction vector

    @return physics object

    history:
        - 07-Apr-2005   David Reyes    created
*/
ncPhysicsObj* nPhyPickObjManager::GetPickedObj( const vector3& startposition, const vector3& direction )
{
    n_assert2( direction.len(), "Incorrect direction." );

    nPhyCollide::nContact contact;

    /// setting up the ray
    this->ray->SetPosition( startposition );

    this->ray->SetDirection( direction );

    this->ray->SetCollidesWith( this->pickeableCategories );

    /// checking collision
    if( !nPhysicsServer::Instance()->Collide( this->ray, 1, &contact ) )
        return 0; // No collision detected

    return contact.GetPhysicsObjB(); // TODO: It will be an entity
}

#endif // !NGAME

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
