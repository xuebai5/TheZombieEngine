#ifndef NPOPULATOR_H
#define NPOPULATOR_H

//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "ncpopulatorarea/ncpopulatorarea.h"

class nPopulator : public nObject
{
public:
	// Constructor
	nPopulator();

	// Destructor
	~nPopulator();

	/// Init instance
	void InitInstance(nObject::InitInstanceMsg initType);

	/// Create a random entity from the list
	nEntityObject* SpawnRandomEntity();
	/// Register a spawner
	void RegisterSpawner (nEntityObject*);
	/// Unregister a spawner
	void UnregisterSpawner (nEntityObject*);
	/// Register a populator area
	void RegisterPopulatorArea (nEntityObject*);
	/// Unregister a populator area
	void UnregisterPopulatorArea (nEntityObject*);
	/// Says if a given spawner is registered
	bool IsRegisteredSpawner (nEntityObject*);
	/// Says if a given populator area is registered
	bool IsRegisteredPopulatorArea (nEntityObject*);
	/// Check all population areas
	void CheckAllPopulatorAreas();
	/// Check a given population area
	void CheckPopulatorArea (nEntityObject*);

	/// Create a random entity from the list
/* nEntityObject* SpawnRandomEntity();
	/// Register a spawner
 void RegisterSpawner(nEntityObject*);
	/// Unregister a spawner
 void UnregisterSpawner(nEntityObject*);
	/// Register a populator area
 void RegisterPopulatorArea(nEntityObject*);
	/// Unregister a populator area
 void UnregisterPopulatorArea(nEntityObject*);
	/// Says if a given spawner is registered
 bool IsRegisteredSpawner(nEntityObject*);
	/// Says if a given populator area is registered
 bool IsRegisteredPopulatorArea(nEntityObject*);
	/// Check all population areas
 void CheckAllPopulatorAreas();
	/// Check a given population area
 void CheckPopulatorArea(nEntityObject*);
		
	NxCOMPONENT_INITCMDS_BEGIN()
																			NxCOMPONENT_INITCMDS_END()*/

	/// Get the instance of the populator object
	static nPopulator* Instance();

private:	
	/// Unregister all spawners
	void UnregisterAllSpawners();
	/// Unregister all populator areas
	void UnregisterAllPopulatorAreas();
	/// Choose breed
	ncPopulatorArea::nSpawnItem* ChooseBreed (const nArray<ncPopulatorArea::nSpawnItem*>& spawnItems) const;

	static nPopulator* Singleton;

	nArray<nEntityObject*> spawners;
	nArray<nEntityObject*> populatorAreas;
};

#endif