#ifndef NCPOPULATORAREA_H
#define NCPOPULATORAREA_H

//------------------------------------------------------------------------------
/**
    @class ncPopulatorArea

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "util/ntag.h"

class polygon;

class ncPopulatorArea : public nComponentObject
{

    NCOMPONENT_DECLARE(ncPopulatorArea,nComponentObject);

public:

	struct nSpawnItem
	{
		nString name;
		int		needed;
		int		totals;
	};

	// Constructor
	ncPopulatorArea();

	// Destructor
	~ncPopulatorArea();

	/// Init instance
	void InitInstance(nObject::InitInstanceMsg initType);

	/// Set an area
	void SetArea (const polygon* polygon);

    /// Says if a point is inside the area
    bool IsInside(const vector3&) const;
    /// Set a spawner
    void SetSpawner(nEntityObject*);
    /// Get the spawner
    nEntityObject* GetSpawner() const;
    /// Sets the number of max inhabitants in the area
    void SetMaxInhabitants(int);
    /// Gets the max number of inhabitants of the area
    int GetMaxInhabitants() const;
    /// Add an entry at the spawn table
    bool AddSpawnItem(const nString&, int);
    /// Change the value for an entry at the spawn table
    bool ChangeSpawnItem(const nString&, int);
    /// Remove an entry from the spawn table
    bool RemoveSpawnItem(const nString&);
    /// Returns the number of a breed in the population area
    int GetNumberIndividuals(const nString&) const;
    /// Get the list of kind creatures available
    void GetBreeds(nArray<nSpawnItem*>&) const;
															
private:
	nArray<nSpawnItem*> spawnTable;

	/// Releases the current area
	void ReleaseArea();
	/// Releases the table of spawns
	void ReleaseTable();
	/// Return the entry regarding a kin
	nSpawnItem* FindSpawnItem (const nString& breed) const;
	/// Calculates the max radius for the sphere which spans the complete area
	void CalculateMaxRadius();

	polygon* area;
	nEntityObject* spawner;
	int maxInhabitants;
	float maxRadius;			// This is the max radius of the polygon that defines the area

};

//------------------------------------------------------------------------------
/**
	SetSpawner
*/
inline
void
ncPopulatorArea::SetSpawner (nEntityObject* spawner)
{
	this->spawner = spawner;
}

//------------------------------------------------------------------------------
/**
	GetSpawner
*/
inline
nEntityObject*
ncPopulatorArea::GetSpawner() const
{
	return this->spawner;
}

//------------------------------------------------------------------------------
/**
	SetMaxInhabitants
*/
inline
void 
ncPopulatorArea::SetMaxInhabitants (int max)
{
	this->maxInhabitants = max;
}

//------------------------------------------------------------------------------
/**
	GetMaxInhabitants
*/
inline
int
ncPopulatorArea::GetMaxInhabitants() const
{
	return this->maxInhabitants;
}

#endif