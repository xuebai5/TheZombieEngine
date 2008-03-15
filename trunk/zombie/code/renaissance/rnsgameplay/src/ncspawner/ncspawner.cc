#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncspawner.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------
#include "ncspawner/ncspawner.h"
#include "util/nrandomlogic.h"
#include "ncspawnpoint/ncspawnpoint.h"
#include "entity/nentityobjectserver.h"

#ifndef NGAME
#include "nspatial/ncspatial.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpawner,nComponentObject);

//------------------------------------------------------------------------------
/**
	Constructor
*/
ncSpawner::ncSpawner()
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
ncSpawner::~ncSpawner()
{
    this->DeleteSpawners();
}

//------------------------------------------------------------------------------
/**
	InitInstance
*/
void
ncSpawner::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
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
	DeleteSpawners
*/
void
ncSpawner::DeleteSpawners()
{
	for ( int i(0); i < this->listSpawners.Size(); ++i )
    {
        n_delete( this->listSpawners[i] );
    }
}

//------------------------------------------------------------------------------
/**
	AddSpawner	
*/
void
ncSpawner::AddSpawner (nEntityObject* spawner, float probability)
{
    n_assert( spawner );

	if ( spawner && !this->ExistsSpawner (spawner) )
	{
		nSpawnProb* spawnProb = n_new (nSpawnProb);
	
        spawnProb->spawnerId = spawner->GetId();
		spawnProb->probability = probability;
		spawnProb->realProb = 0.f;

		this->listSpawners.Append (spawnProb);
	}
}

//------------------------------------------------------------------------------
/**
	AddSpawnerById	
*/
void
ncSpawner::AddSpawnerById (nEntityObjectId spawnerId, float probability)
{
    nEntityObject* spawner = nEntityObjectServer::Instance()->GetEntityObject( spawnerId );
    n_assert( spawner );

    if ( spawner )
    {
        this->AddSpawner( spawner, probability );
    }
}

//------------------------------------------------------------------------------
/**
	SetProbability
*/
void
ncSpawner::SetProbability (nEntityObject* spawner, float probability)
{
	nSpawnProb* spawnProb = this->GetSpawnProb (spawner);

	if ( spawnProb )
	{
		spawnProb->probability = probability;
	}
}

//------------------------------------------------------------------------------
/**
	RemoveSpawner
*/
void
ncSpawner::RemoveSpawner (nEntityObject* spawner)
{
    n_assert( spawner );

    if ( spawner )
    {
    	bool done = false;

	    for ( int i=0; i<this->listSpawners.Size() && !done; i++ )
	    {
		    nSpawnProb* elem = this->listSpawners[i];

            if ( elem->spawnerId == spawner->GetId() )
		    {
			    this->listSpawners.Erase(i);
    			
                // Do not release the spawner here, who has created it should release it by itself
                // (this is specially desired in editor mode, where the user may want to not delete the spawner,
                // but only to unbind it from this spawner)
			    n_delete (elem);
			    done = true;
		    }
	    }
    }
}

//------------------------------------------------------------------------------
/**
	CalculateRealProb
*/
void
ncSpawner::CalculateRealProb()
{
	float maxProb = this->GetSumProbabilities();

	if ( maxProb > 0 )
	{
		for ( int i=0; i<this->listSpawners.Size(); i++ )
		{
            nSpawnProb* prob = this->listSpawners[i];
			prob->realProb = prob->probability*100.f / maxProb;
		}
	}
}

//------------------------------------------------------------------------------
/**
	ChooseSpawner
*/
nEntityObject*
ncSpawner::ChooseSpawner()
{
	nEntityObject* spawner = 0;

	this->CalculateRealProb();

	// Get a random value to choosing the spawner
	float random = n_rand_real(100.f);
	float current = 0.f;

	for ( int i=0; i<this->listSpawners.Size() && !spawner; i++ )
	{
		nSpawnProb* spawnProb = this->listSpawners[i];

		current += spawnProb->realProb;

		if ( random <= current )
		{
            spawner = nEntityObjectServer::Instance()->GetEntityObject( spawnProb->spawnerId );
            n_assert( spawner );
		}
	}

	return spawner;
}

//------------------------------------------------------------------------------
/**
	ExistsSpawner
*/
bool
ncSpawner::ExistsSpawner (nEntityObject* spawner) const
{
	return this->GetSpawnProb (spawner) != 0;
}

//------------------------------------------------------------------------------
/**
	GetSpawnProb
*/
ncSpawner::nSpawnProb*
ncSpawner::GetSpawnProb (nEntityObject* spawner) const
{
	nSpawnProb* spawnProb = 0;

	for ( int i=0; i<this->listSpawners.Size() && !spawnProb; i++ )
	{
		nSpawnProb* elem = this->listSpawners[i];

        if ( elem->spawnerId == spawner->GetId() )
		{
			spawnProb = elem;
		}
	}

	return spawnProb;
}

//------------------------------------------------------------------------------
/**
	GetSumProbabilities
*/
float
ncSpawner::GetSumProbabilities() const
{
	float sum = 0.f;

	for ( int i=0; i<this->listSpawners.Size(); i++ )
	{
		nSpawnProb* elem = this->listSpawners[i];
		sum += elem->probability;
	}

	return sum;
}

//------------------------------------------------------------------------------
/**
	SpawnEntity
*/
nEntityObject*
ncSpawner::SpawnEntity (const nString& entity)
{
	nEntityObject* spawner = this->ChooseSpawner();
	nEntityObject* spawn = 0;

	
	if ( spawner )
	{
		// Check if there is a spawn point for spawn of wether is other spawner
		ncSpawnPoint* spawnPoint = spawner->GetComponent <ncSpawnPoint>();
	
		if ( spawnPoint )
		{
			spawn = spawnPoint->SpawnEntity (entity);
		}
		else
		{
			ncSpawner* spawnerComponent = spawner->GetComponent <ncSpawner>();

			if ( spawnerComponent )
			{
				spawn = spawnerComponent->SpawnEntity (entity);
			}
			else
			{
				n_assert2_always ("There is not components for spawn entities");
			}
		}

		if ( !spawn )
		{
			// ISMA TODO: Try choose other spawner
		}
	}

	return spawn;
}

//------------------------------------------------------------------------------
/**
	SpawnEntityFrozen
*/
nEntityObject*
ncSpawner::SpawnEntityFrozen (const nString& entity)
{
	nEntityObject* spawner = this->ChooseSpawner();
	nEntityObject* spawn = 0;

	
	if ( spawner )
	{
		// Check if there is a spawn point for spawn of wether is other spawner
		ncSpawnPoint* spawnPoint = spawner->GetComponent <ncSpawnPoint>();
	
		if ( spawnPoint )
		{
			spawn = spawnPoint->SpawnEntityFrozen (entity);
		}
		else
		{
			ncSpawner* spawnerComponent = spawner->GetComponent <ncSpawner>();

			if ( spawnerComponent )
			{
				spawn = spawnerComponent->SpawnEntityFrozen (entity);
			}
			else
			{
				n_assert2_always ("There is not components for spawn entities");
			}
		}

		if ( !spawn )
		{
			// ISMA TODO: Try choose other spawner
		}
	}

	return spawn;
}

//------------------------------------------------------------------------------
/**
	GetSpawnersNumber
*/
int
ncSpawner::GetSpawnersNumber() const
{
	return this->listSpawners.Size();
}

//------------------------------------------------------------------------------
/**
	GetSpawnerByIndex
*/
nEntityObject*
ncSpawner::GetSpawnerByIndex (int index) const
{
    nEntityObject* spawner (0);

    if ( index >= 0 && index < this->listSpawners.Size() )
    {
        nEntityObjectId spawnerId = this->listSpawners[index]->spawnerId;
        spawner = nEntityObjectServer::Instance()->GetEntityObject( spawnerId );
        n_assert( spawner );
    }

    return spawner;
}

//------------------------------------------------------------------------------
/**
	GetSpawnerProbability
*/
float
ncSpawner::GetSpawnerProbability (nEntityObject* spawner) const
{
	nSpawnProb* spawnProb = this->GetSpawnProb (spawner);

	if ( !spawnProb )
	{
        return 0;
	}
	return spawnProb->probability;
}
