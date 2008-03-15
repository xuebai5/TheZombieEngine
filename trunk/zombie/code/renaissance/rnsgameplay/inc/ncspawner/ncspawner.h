#ifndef NCSPAWNER_H
#define NCSPAWNER_H

//------------------------------------------------------------------------------
/**
    @class ncSpawner
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"

//------------------------------------------------------------------------------

class ncSpawner : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSpawner,nComponentObject);

public:
	// Constructor
	ncSpawner();

	// Destructor
	~ncSpawner();

	/// Init instance
	void InitInstance(nObject::InitInstanceMsg initType);

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

	/// Add a spawner with a probability
 void AddSpawner(nEntityObject*, float);
	/// Set probability
 void SetProbability(nEntityObject*, float);
	/// Remove spawner
 void RemoveSpawner(nEntityObject*);
	/// Choose a spawner
 nEntityObject* ChooseSpawner();
	/// Says if exist a spawner at the list
 bool ExistsSpawner(nEntityObject*) const;
	/// Order a new spawn
 nEntityObject* SpawnEntity(const nString&);
	/// Order a new spawn, but do not start the new entity behavior
 nEntityObject* SpawnEntityFrozen(const nString&);

    /// Get the number of spawners bound to this spawner
 int GetSpawnersNumber() const;
    /// Get a spawner by index
 nEntityObject* GetSpawnerByIndex(int) const;
    /// Get the probability of a spawner
 float GetSpawnerProbability(nEntityObject*) const;
    /// Add a spawner with a probability by entity id (used for persistence)
 void AddSpawnerById(nEntityObjectId, float);

															
									
private:
	struct nSpawnProb
	{
        // Store the entity id instead of a pointer because the entity
        // may not be available at load time or the pointer change due
        // to dynamic loading/unloading.
		//nEntityObject* spawner;
        nEntityObjectId spawnerId;
		float probability;
		float realProb;
	};

	/// Deletes the list of spawners
	void DeleteSpawners();
	/// Get a spawnProb from the list
	nSpawnProb* GetSpawnProb (nEntityObject* entity) const;
	/// Get sum of probabilities
	float GetSumProbabilities() const;
	/// Update real probabilities
	void CalculateRealProb();

	/// The list of spawn points/spawners
	nArray<nSpawnProb*> listSpawners;

};

#endif