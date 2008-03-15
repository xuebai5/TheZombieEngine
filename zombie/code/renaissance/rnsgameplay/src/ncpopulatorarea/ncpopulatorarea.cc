#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncpopulatorarea.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------
#include "ncpopulatorarea/ncpopulatorarea.h"
#include "mathlib/polygon.h"
#include "nspatial/nspatialserver.h"
#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncPopulatorArea,nComponentObject);

//------------------------------------------------------------------------------
/**
	Constructor
*/
ncPopulatorArea::ncPopulatorArea() : 
	area(0),
	spawner(0),
	maxInhabitants(10),
	maxRadius(0.f)
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
ncPopulatorArea::~ncPopulatorArea()
{
	this->ReleaseArea();
	this->ReleaseTable();
}

//------------------------------------------------------------------------------
/**
	InitInstance
*/
void
ncPopulatorArea::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
	this->CalculateMaxRadius();
}

//------------------------------------------------------------------------------
/**
	SetArea
*/
void
ncPopulatorArea::SetArea (const polygon* poly)
{
	this->ReleaseArea();

	this->area = n_new (polygon);
	this->area->Copy (*poly);
	this->CalculateMaxRadius();
}

//------------------------------------------------------------------------------
/**
	ReleaseArea
*/
void
ncPopulatorArea::ReleaseArea()
{
	n_delete (this->area);
	this->area = 0;
}

//------------------------------------------------------------------------------
/**
	ReleaseTable
*/
void
ncPopulatorArea::ReleaseTable()
{
	for ( int i=0; i<this->spawnTable.Size(); i++ )
	{
		nSpawnItem* item = this->spawnTable[i];

		n_delete (item);
		item = 0;
	}

	this->spawnTable.Clear();
}

//------------------------------------------------------------------------------
/**
	IsInside
*/
bool
ncPopulatorArea::IsInside (const vector3& point) const
{
	n_assert(this->area);

	vector3 aux(point);
	aux.y = 0.f;

	return this->area && this->area->IsPointInside (aux);
}

//------------------------------------------------------------------------------
/**
	CalculateMaxRadius	
*/
void
ncPopulatorArea::CalculateMaxRadius()
{
	if ( this->area )
	{
		// First, get the max distance from the center to every single vertex
		vector3 center = this->area->Midpoint();
		float max = 0.f;

		for ( int i=0; i<this->area->GetNumVertices(); i++ )
		{
			vector3 vertex = this->area->GetVertex(i);
			float distance = (vertex - center).len();

			if ( distance > max )
			{
				max = distance;
			}
		}

		this->maxRadius = max;
	}
}

//------------------------------------------------------------------------------
/**
	AddSpawnItem
*/
bool
ncPopulatorArea::AddSpawnItem (const nString& breed, int needed)
{
	nSpawnItem* item = this->FindSpawnItem (breed);
	bool added = item != 0;

	if ( item != 0 )
	{
		item = n_new (nSpawnItem);

		item->name = breed;
		item->needed = needed;
		item->totals = needed;
		this->spawnTable.Append (item);
	}

	return added;
}

//------------------------------------------------------------------------------
/**
	ChangeSpawnItem
*/
bool
ncPopulatorArea::ChangeSpawnItem (const nString& breed, int needed)
{
	nSpawnItem* item = this->FindSpawnItem (breed);
	bool exists = item != 0;

	if ( exists )
	{
		item->needed = needed;
		item->totals = needed;
	}

	return exists;
}

//------------------------------------------------------------------------------
/**
	RemoveSpawnItem
*/
bool 
ncPopulatorArea::RemoveSpawnItem (const nString& breed)
{
	bool done = false;

	for ( int i=0; i<this->spawnTable.Size() && !done; i++ )
	{
		nSpawnItem* item = this->spawnTable[i];

		if ( item->name == breed )
		{
			this->spawnTable.Erase(i);
			n_delete (item);
		}
	}

	return done;
}

//------------------------------------------------------------------------------
/**
	FindSpawnItem
*/
ncPopulatorArea::nSpawnItem*
ncPopulatorArea::FindSpawnItem (const nString& breed) const
{
	nSpawnItem* item = 0;

	for ( int i=0; i<this->spawnTable.Size() && !item; i++ )
	{
		nSpawnItem* elem = this->spawnTable[i];

		if ( elem->name == breed )
		{
			item = elem;
		}
	}

	return item;
}

//------------------------------------------------------------------------------
/**
	GetNumberIndividuals
*/
int
ncPopulatorArea::GetNumberIndividuals (const nString& breed) const
{
	n_assert(this->area);

	int numEntities = 0;

	if ( this->area )
	{
		// Search all entities inside the area
		nArray<nEntityObject*> entities;
		vector3 pos = this->area->Midpoint();
		sphere sph (pos, this->maxRadius);

		nSpatialServer::Instance()->GetEntitiesByPos (sph, &entities);

		for ( int i=0; i<entities.Size(); i++ )
		{
			nEntityObject* entity = entities[i];

			if ( entity && entity->IsA (breed.Get()) )
			{
				ncTransform* transform = entity->GetComponentSafe <ncTransform>();
				n_assert(transform);

				if ( transform )
				{
					vector3 pos = transform->GetPosition();

					pos.y = 0.f;
					
					if ( this->area->IsPointInside(pos) )
					{
						numEntities++;
					}
				} // if ( transform )
			}
		} // for ( int i...)
	}

	return numEntities;
}

//------------------------------------------------------------------------------
/**
	GetBreeds
*/
void
ncPopulatorArea::GetBreeds (nArray<nSpawnItem*>& breeds) const
{
	breeds.Reset();

	for ( int i=0; i<this->spawnTable.Size(); i++ )
	{
		nSpawnItem* spawnItem = this->spawnTable[i];
		breeds.Append (spawnItem);
	}
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------