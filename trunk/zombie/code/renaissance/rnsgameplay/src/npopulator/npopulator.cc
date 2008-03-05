#include "precompiled/pchrnsgameplay.h"

#include "npopulator/npopulator.h"
#include "util/nrandomlogic.h"
#include "ncspawner/ncspawner.h"

nNebulaClass(nPopulator, "nobject");

nPopulator* nPopulator::Singleton = 0;

//------------------------------------------------------------------------------
/**
	Constructor
*/
nPopulator::nPopulator()
{
	n_assert(Singleton==0);
	Singleton = this;
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
nPopulator::~nPopulator()
{
	this->UnregisterAllSpawners();
	this->UnregisterAllPopulatorAreas();

	Singleton = 0;
}

//------------------------------------------------------------------------------
/**
	InitInstance
*/
void
nPopulator::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
	// empty
}

//------------------------------------------------------------------------------
/**
	SpawnRandomEntity
*/
nEntityObject*
nPopulator::SpawnRandomEntity()
{
	return 0;
}

//------------------------------------------------------------------------------
/**
	RegisterSpawner
*/
void
nPopulator::RegisterSpawner (nEntityObject* spawner)
{
	if ( !this->IsRegisteredPopulatorArea (spawner) )
	{
		this->populatorAreas.Append (spawner);
	}
}

//------------------------------------------------------------------------------
/**
	UnregisterSpawner
*/
void
nPopulator::UnregisterSpawner (nEntityObject* spawner)
{
	bool found = false;

	for ( int i=0; i<this->spawners.Size() && !found; )
	{
		nEntityObject* element = this->spawners[i];

		if ( element == spawner )
		{
			this->spawners.EraseQuick (i);
			found = true;
		}
		else
		{
			i++;
		}
	}
}

//------------------------------------------------------------------------------
/**
	RegisterPopulatorArea
*/
void
nPopulator::RegisterPopulatorArea (nEntityObject* populatorArea)
{
	if ( !this->IsRegisteredPopulatorArea (populatorArea) )
	{
		this->populatorAreas.Append (populatorArea);
	}
}

//------------------------------------------------------------------------------
/**
	UnregisterPopulatorArea
*/
void
nPopulator::UnregisterPopulatorArea (nEntityObject* populatorArea)
{
	bool found = false;

	for ( int i=0; i<this->populatorAreas.Size() && !found; )
	{
		nEntityObject* element = this->populatorAreas[i];

		if ( element == populatorArea )
		{
			this->populatorAreas.EraseQuick (i);
			found = true;
		}
		else
		{
			i++;
		}
	}
}

//------------------------------------------------------------------------------
/**
	UnregisterAllSpawners
*/
void
nPopulator::UnregisterAllSpawners()
{
	for ( int i=0; i<this->spawners.Size(); i++ )
	{
		this->spawners.EraseQuick(i);
	}
}

//------------------------------------------------------------------------------
/**
	UnregisterAllPopulatorAreas
*/
void
nPopulator::UnregisterAllPopulatorAreas()
{
	for ( int i=0; i<this->populatorAreas.Size(); i++ )
	{
		this->populatorAreas.EraseQuick(i);
	}
}

//------------------------------------------------------------------------------
/**
	IsRegisteredSpawner
*/
bool
nPopulator::IsRegisteredSpawner (nEntityObject* spawner)
{
	bool found = false;

	for ( int i=0; i<this->spawners.Size() && !found; i++ )
	{
		found = bool (this->spawners[i] == spawner);
	}

	return found;
}

//------------------------------------------------------------------------------
/**
	IsRegisteredPopulatorArea
*/
bool
nPopulator::IsRegisteredPopulatorArea (nEntityObject* populatorArea)
{
	bool found = false;

	for ( int i=0; i<this->populatorAreas.Size() && !found; i++ )
	{
		found = bool (this->populatorAreas[i] == populatorArea);
	}

	return found;
}

//------------------------------------------------------------------------------
/**
	CheckAllPopulatorAreas
*/
void
nPopulator::CheckAllPopulatorAreas()
{
	// Note: You must just check the active areas, not all. Do this depending the 
	// LOD system chosen.
	for ( int i=0; i<this->populatorAreas.Size(); i++ )
	{
		nEntityObject* pa = this->populatorAreas[i];
		this->CheckPopulatorArea (pa);
	}
}

//------------------------------------------------------------------------------
/**
	CheckPopulatorArea
*/
void
nPopulator::CheckPopulatorArea (nEntityObject* populatorArea)
{
	n_assert(populatorArea);

	if ( populatorArea )
	{
		nArray<ncPopulatorArea::nSpawnItem*> breeds;
		ncPopulatorArea* pa = populatorArea->GetComponentSafe <ncPopulatorArea>();
		n_assert(pa);

		if ( pa )
		{
			pa->GetBreeds (breeds);

			// Now, say how many creatures are needed for each breed
			for ( int i=0; i<breeds.Size(); i++ )
			{
				ncPopulatorArea::nSpawnItem* spawnItem = breeds[i];
				int current = pa->GetNumberIndividuals (spawnItem->name);

				spawnItem->needed = spawnItem->totals - current;
			}

			// Now, we know the needed elements to put in the area, choose one kind
			ncPopulatorArea::nSpawnItem* spawnItem = this->ChooseBreed (breeds);

			if ( spawnItem )
			{
				nEntityObject* spawner = pa->GetSpawner();
				n_assert(spawner);

				if ( spawner )
				{
					ncSpawner* sp = spawner->GetComponentSafe <ncSpawner>();
					n_assert(sp);

					// Finally, we spawn the selected entity
					if ( sp )
					{
						//#ifndef NGAME
						sp->SpawnEntity (spawnItem->name);
						//#endif
					}
				}
			}
		} // if ( pa )
	}
}

//------------------------------------------------------------------------------
/**
	ChooseBreed
*/
ncPopulatorArea::nSpawnItem*
nPopulator::ChooseBreed (const nArray<ncPopulatorArea::nSpawnItem*>& spawnItems) const
{
	ncPopulatorArea::nSpawnItem* selected = 0;

	// We'll use a token ring algorithm to select the given spawn
	// First, we choose a random entry and check if is a valid spawn
	// after this, we run the list looking for a valid one.
	int maxItems = spawnItems.Size();
	int index = n_rand_int (maxItems);
	int counter = 0;

	do {
		ncPopulatorArea::nSpawnItem* candidate = spawnItems[index];

		if ( candidate->needed > 0 )
		{
			selected = candidate;
		}
		else
		{
			if ( ++index >= maxItems )
			{
				index = 0;
			}
		}

	} while ( !selected && ++counter < maxItems );

	return selected;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------