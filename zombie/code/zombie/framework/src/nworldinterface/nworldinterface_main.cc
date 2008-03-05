//-----------------------------------------------------------------------------
//  nworldinterface_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchframework.h"
#include "nworldinterface/nworldinterface.h"
#include "entity/nentityobjectserver.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"

#include "nphysics/ncphysicsobj.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncsubentity.h"
#include "zombieentity/ncsuperentity.h"
#include "zombieentity/nloaderserver.h"
#include "nspatial/ncspatialspace.h"

#ifndef __ZOMBIE_EXPORTER__
#include "ntrigger/ncareaevent.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/ntriggerserver.h"

#include "rnsgameplay/ncgameplay.h"
#endif

#ifndef NGAME
#include "ndebug/nceditor.h"
#include "ndebug/nceditorclass.h"
#include "nlayermanager/nlayermanager.h"
#include "nspatial/ncspatial.h"
#endif

//-----------------------------------------------------------------------------
nNebulaScriptClass(nWorldInterface, "nroot");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nWorldInterface, EntityCreated);

//------------------------------------------------------------------------------
nWorldInterface* nWorldInterface::instance = 0;

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nWorldInterface::nWorldInterface()
{
    if ( !nWorldInterface::instance )
    {
        // Initialize instance pointer
        nWorldInterface::instance = this;
    }
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nWorldInterface::~nWorldInterface()
{
    if ( nWorldInterface::instance == this )
    {
        nWorldInterface::instance = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Get an entity by its name as assigned by the level
*/
nEntityObject*
nWorldInterface::GetLevelEntity( const char* name ) const
{
#ifndef NGAME
    // Verify that a name has been given
    if ( !name )
    {
        n_error( "carles.ros: No entity name has been given to the 'world:getlevelentity' command" );
        return NULL;
    }
#endif

    // Get the entity for the given name
    nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
#ifndef NGAME
    if ( !level )
    {
        n_error( "carles.ros: Current level not found.\nPlease, contact with a programmer if you see this error." );
        return NULL;
    }
#endif
    nEntityObjectId entityId = level->FindEntity( name );

#ifndef NGAME
    // Verify that there's an entity with the given name
    if ( entityId == 0 )
    {
        nString error( "carles.ros: Couldn't find any entity with the name '" );
        error += name;
        error += "'";
        n_error( error.Get() );
        return NULL;
    }
#endif

    // Get the entity
    nEntityObject* entity = nEntityObjectServer::Instance()->GetEntityObject( entityId );
#ifndef NGAME
    if ( !entity )
    {
        nString error( "carles.ros: The current level has an id for the entity named '" );
        error += name;
        error += "', but there isn't any entity with this id.\nPlease, contact with a programmer if you see this error.";
        n_error( error.Get() );
        return NULL;
    }
#endif
    return entity;
}

//-----------------------------------------------------------------------------
/**
    Get an entity by its name as assigned by the gameplay

    @todo Use a look up table instead of iterating all the entities
*/
nEntityObject*
nWorldInterface::GetGameEntity( const char* name ) const
{
#ifndef __ZOMBIE_EXPORTER__
    nEntityObjectServer* objServer = nEntityObjectServer::Instance();
    for ( nEntityObject* entity = objServer->GetFirstEntityObject(); entity; entity = objServer->GetNextEntityObject() )
    {
        ncGameplay* gameplay = entity->GetComponent<ncGameplay>();
        if ( gameplay )
        {
            if ( gameplay->GetName() == name )
            {
                return entity;
            }
        }
    }
#endif // __ZOMBIE_EXPORTER__
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Create a new entity
*/
nEntityObject*
nWorldInterface::NewEntity(const char* className, vector3 position) const
{
    return this->CreateEntity( className, position, nEntityObjectServer::Normal, NULL );
}

//-----------------------------------------------------------------------------
/**
    Create a new entity
*/
nEntityObject*
nWorldInterface::NewLocalEntity (const char* className, vector3 position, bool /*obsoleteParam*/, nEntityObject* superEntity) const
{
    return this->CreateEntity( className, position, nEntityObjectServer::Local, superEntity );
}

//-----------------------------------------------------------------------------
/**
    Create a new entity
*/
nEntityObject*
nWorldInterface::NewServerEntity (const char* className, vector3 position, nEntityObject* superEntity) const
{
    return this->CreateEntity( className, position, nEntityObjectServer::Server, superEntity );
}

//-----------------------------------------------------------------------------
/**
    Create a new entity and place it in the world ready to be used

    The difference from this method and the nEntityObjectServer::NewEntityObject
    is that this method does other necessary stuff required to have the entity
    available and usable by the several servers.

    @todo Review it to match the placer tool implementation
    @todo Make the placer tool use this function instead of having repeated code
*/
nEntityObject*
nWorldInterface::CreateEntity( const char* className, vector3 position, nEntityObjectServer::nEntityObjectType entityScope, nEntityObject* superEntity ) const
{
    // The position is inside a cell?
    nSpatialServer* spatSrv = nSpatialServer::Instance();
    n_assert( spatSrv );
    if ( spatSrv->SearchCell( position ) == 0 )
    {
        return false;
    }

    // Instantiate an entity from the specified class
    nEntityObject* entityInstance = 0;
    switch ( entityScope )
    {
        case nEntityObjectServer::Normal:
            {
#ifndef NGAME
                bool mustBeLocal(false);

                // check the class it isn't required to be create local
                nEntityClass* eclass( static_cast<nEntityClass*>(nKernelServer::Instance()->FindClass( className )) );

                if( eclass )
                {
                    ncEditorClass* editorclass(eclass->GetComponent<ncEditorClass>());

                    if( editorclass )
                    {
                        if( editorclass->GetAssetKeyInt( "LocalEntity" ) )
                        {
                            mustBeLocal = true;
                        }
                    }
                }

                if( mustBeLocal )
                {
                    entityInstance = static_cast<nEntityObject*>( nEntityObjectServer::Instance()->NewLocalEntityObject( className ) );
                }
                else
#endif // !!!!!ATENTION!!!!!: If you add more lines after the endif be careful, with the code check it does work
                {
                    entityInstance = static_cast<nEntityObject*>( nEntityObjectServer::Instance()->NewEntityObject( className ) );
                }
            }
            break;
        case nEntityObjectServer::Local:
            entityInstance = static_cast<nEntityObject*>( nEntityObjectServer::Instance()->NewLocalEntityObject( className ) );
            break;
        case nEntityObjectServer::Server:
            entityInstance = static_cast<nEntityObject*>( nEntityObjectServer::Instance()->NewServerEntityObject( className ) );
            break;
    }
    if ( ! entityInstance )
    {
        return NULL;
    }

    // check if the entity is an indoor
    ncSpatialSpace *spaceComp = entityInstance->GetComponent<ncSpatialSpace>();

    bool isIndoor = spaceComp && ( spaceComp->GetType() == ncSpatialSpace::N_INDOOR_SPACE );
    if (! isIndoor )
    {
        // if it's a subentity, link it to the superentity
        if ( entityScope != nEntityObjectServer::Normal && superEntity )
        {
            ncSuperentity* superEntComp = superEntity->GetComponent<ncSuperentity>();
            n_assert( superEntComp );
            superEntComp->AddSubentity( entityInstance );
        }
    }

    // Set current layer to the object
    nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert( level );

#ifndef NGAME
    // Insert the entity in the current layer
    nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );
    n_assert( layerManager );
    ncEditor* editorComp = entityInstance->GetComponent<ncEditor>();
    if ( editorComp )
    {
        int selectedLayerId = layerManager->GetSelectedLayerId();
        editorComp->SetLayerId( selectedLayerId );
        if (!layerManager->SearchLayer( selectedLayerId )->IsActive())
        {
            ncSpatial* spatialComp = entityInstance->GetComponent<ncSpatial>();
            if (spatialComp)
            {
                spatialComp->RemoveTemporary();
            }
        }
    }
#endif

    entityInstance->GetComponentSafe<ncTransform>()->SetPosition( position );
    this->AddToWorld(entityInstance);

    // Notify entity creation
    this->SignalEntityCreated( const_cast<nWorldInterface*>(this), entityInstance );

    return entityInstance;
}

//-----------------------------------------------------------------------------
/**
    Remove an entity from the world and delete it

    This method hasn't been exposed to scripting because the script side may
    keep a reference to the entity that's going to be deleted, which could
    cause the entity server to complain about someone still not having
    released its reference.
*/
void
nWorldInterface::DeleteEntity( nEntityObject* entity ) const
{
    if ( entity )
    {
        this->RemoveFromWorld(entity);
        nEntityObjectServer::Instance()->RemoveEntityObject( entity );
    }
}

//-----------------------------------------------------------------------------
/**
    Remove an entity from the world and delete it
*/
void
nWorldInterface::DeleteEntityById( nEntityObjectId entityId ) const
{
    this->DeleteEntity( nEntityObjectServer::Instance()->GetEntityObject( entityId ) );
}

//-----------------------------------------------------------------------------
/**
    Add an entity to the world
*/
void
nWorldInterface::AddToWorld(nEntityObject * entityInstance) const
{
    n_assert(entityInstance);

    // Reinsert entity in spatial server
    ncTransform* trComp = entityInstance->GetComponent<ncTransform>();
    n_assert( trComp );
    trComp->UpdateComponent(ncTransform::cSpatial);

    // Immediate loading
    nLoaderServer::Instance()->EntityNeedsLoading(entityInstance);

    // Add entity to physic space
    ncPhysicsObj* phyComp = entityInstance->GetComponent<ncPhysicsObj>();
    if ( phyComp )
    {
        phyComp->AutoInsertInSpace();
    }

#ifndef NGAME
    // Update subentity relative transform
    ncSubentity* se = entityInstance->GetComponent<ncSubentity>();
    if ( se )
    {
        se->UpdateRelativePosition();
    }
#endif

    // Update entities
    ncSuperentity* supEnt = entityInstance->GetComponent<ncSuperentity>();
    if ( supEnt )
    {
        supEnt->UpdateSubentities();
    }

    // Connect indoors if placed an indoor
    ncSpatialSpace *spaceComp = entityInstance->GetComponent<ncSpatialSpace>();
    bool isIndoor = spaceComp && ( spaceComp->GetType() == ncSpatialSpace::N_INDOOR_SPACE );
    if ( isIndoor )
    {
        nSpatialServer::Instance()->ConnectIndoors();
    }
}

//-----------------------------------------------------------------------------
/**
    Remove an entity from the world
*/
void
nWorldInterface::RemoveFromWorld(nEntityObject * entity) const
{
    n_assert(entity);

#ifndef __ZOMBIE_EXPORTER__
    // stop emitting events
    ncAreaEvent* events = entity->GetComponent<ncAreaEvent>();
    if ( events )
    {
        events->DeleteAllEvents();
    }

    // stop receiving events
    ncTrigger* trigger = entity->GetComponent<ncTrigger>();
    if ( trigger )
    {
        nTriggerServer::Instance()->RemoveTrigger( entity );
    }
#endif // __ZOMBIE_EXPORTER__

#ifndef NGAME
    entity->SendToLimbo( entity );
#endif
}

//-----------------------------------------------------------------------------
