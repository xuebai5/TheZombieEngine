#ifndef NCSPAWNPOINT_H
#define NCSPAWNPOINT_H

//------------------------------------------------------------------------------
/**
    @class ncSpawnPoint
    
    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
#include "entity/nentity.h"

class nEntityObject;
class ncTransform;

class ncSpawnPoint : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSpawnPoint,nComponentObject);

public:
	// Constructor
	ncSpawnPoint();

	// Destructor
	~ncSpawnPoint();

	/// Init instance
	void InitInstance(nObject::InitInstanceMsg initType);

	/// Create a new entity by its class name, and start its behavior
 nEntityObject* SpawnEntity(const nString&);
	/// Create a new entity by its class name, without starting its behavior
 nEntityObject* SpawnEntityFrozen(const nString&);
	
						
private:	
	/// Set a position to a new entity
	bool PlaceEntity (nEntityObject* agent);
    /// Return the position where the next entity is going to be spawned
    bool CalcSpawnPosition (vector3& spawnPoint) const;

	ncTransform*	transform;	// Transform component

};

#endif