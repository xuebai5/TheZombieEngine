#ifndef NC_SOUNDCLASS_H
#define NC_SOUNDCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSoundClass
    @ingroup Entities
    @author Juan Jose Luna Espinosa

    @brief Entity component class to keep track of played sounds on entities

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "nsoundeventtable/nsoundeventtable.h"

//------------------------------------------------------------------------------
class ncSoundClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncSoundClass,nComponentClass);

public:

    /// constructor
    ncSoundClass();
    /// destructor
    ~ncSoundClass();
    /// component class persistence
    bool SaveCmds(nPersistServer* ps);

    /// Get sounds table
    nSoundEventTable* GetSoundTable() const;

    // Get sound table and create it if not exists
    nSoundEventTable* SafeGetSoundTable();

    /// indicates the beginning of the sound table (persistence only)
    nSoundEventTable * BeginSoundTable();

    /// load resources of components
    bool LoadResources();

    /// load resources of components
    bool UnloadResources();

private:

    // Map of sounds
    nSoundEventTable* soundTable;

};

//------------------------------------------------------------------------------
#endif

