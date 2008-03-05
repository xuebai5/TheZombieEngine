//------------------------------------------------------------------------------
//  ncspawnpoint.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "ncspawnpoint/ncspawnpoint.h"
#include "zombieentity/nctransform.h"
#include "nworldinterface/nworldinterface.h"
#include "ncfsm/ncfsm.h"

#ifndef NGAME
#include "nspatial/ncspatial.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpawnPoint,nComponentObject);

//------------------------------------------------------------------------------
/**
	Constructor
*/
ncSpawnPoint::ncSpawnPoint() :
	transform(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
ncSpawnPoint::~ncSpawnPoint()
{
	// empty
}

//------------------------------------------------------------------------------
/**
	InitInstance
*/
void
ncSpawnPoint::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
	n_assert(this->entityObject);

	if ( this->entityObject )
	{
		this->transform = this->entityObject->GetComponent <ncTransform>();
	}

#ifndef NGAME
    ncSpatial* spatial = this->GetComponent<ncSpatial>();
    if ( spatial )
    {
        spatial->SetOriginalBBox( 0,0,0, 0.5f,0.5f,0.5f );
    }
#endif
}

//------------------------------------------------------------------------------
/**
	SpawnEntity
*/
nEntityObject*
ncSpawnPoint::SpawnEntity (const nString& entityName)
{
    nEntityObject* entity( this->SpawnEntityFrozen( entityName ) );
    if ( entity )
    {
        // If entity is an agent, make it begin running its behavior
        ncFSM* fsm = entity->GetComponent<ncFSM>();
        if ( fsm )
        {
            fsm->Init();
        }
    }
    return entity;
}

//------------------------------------------------------------------------------
/**
	SpawnEntityFrozen
*/
nEntityObject*
ncSpawnPoint::SpawnEntityFrozen (const nString& entityName)
{
    nWorldInterface* world = nWorldInterface::Instance();
    n_assert( world );
	nEntityObject* entity = 0;

	// Well, this spawn point is able to create this kind of entity
	if ( world )
	{
		// Now, we must place the entity in the right position,
		// if wasn't able to allocate in a position, we don't create it
        vector3 spawnPos;
        if ( this->CalcSpawnPosition( spawnPos ) )
        {
		    entity = world->NewServerEntity (entityName.Get(), spawnPos, NULL);
        }
    }

	return entity;
}

//------------------------------------------------------------------------------
/**
	CalcSpawnPosition

	@return the position where the next entity is going to be spawned
*/
bool
ncSpawnPoint::CalcSpawnPosition (vector3& spawnPoint) const
{
	n_assert(this->transform);

	bool able = false;

	if ( this->transform )
	{
        // The following commented block of code has been kept because we
        // still don't know which was its purpose
/*		vector3 position = this->transform->GetPosition();
		quaternion orientation = this->transform->GetQuat();		
		matrix44 transform(orientation);

		position += vector3 (0.f, 0.f, 5.f);
		transform.mult (position, spawnPoint);*/
        spawnPoint = this->transform->GetPosition();
        able = true;
    }

    return able;
}

//------------------------------------------------------------------------------
/**
	PlaceEntity

	@returns if was able to allocate the entity
*/
bool
ncSpawnPoint::PlaceEntity (nEntityObject* agent)
{
	n_assert(agent);
	n_assert(this->transform);

	bool able = false;

	if ( agent && this->transform )
	{
		vector3 position = this->transform->GetPosition();
		quaternion orientation = this->transform->GetQuat();		
		matrix44 transform(orientation);
		vector3 spawnPoint;

		position += vector3 (0.f, 0.f, 5.f);
		transform.mult (position, spawnPoint);

		// Temporal solution, it always puts the agent at the selected position
		ncTransform* agentTransform = agent->GetComponent <ncTransform>();
		n_assert(agentTransform);

		if ( agentTransform )
		{
			agentTransform->SetPosition (spawnPoint);
            able = true;
		}
	}

	return able;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
