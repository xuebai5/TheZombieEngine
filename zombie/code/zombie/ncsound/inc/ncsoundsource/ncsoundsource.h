#ifndef NC_SOUNDSOURCE_H
#define NC_SOUNDSOURCE_H
//------------------------------------------------------------------------------
/**
    @class ncSoundSource
    @ingroup Entities
    @author Juan Jose Luna Espinosa

    @brief Entity component for sound sources data and linkage to a trigger

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "nsoundeventtable/nsoundeventtable.h"
#include "nsoundscheduler/nsoundscheduler.h"

class ncTransform;
struct nGameEvent;

//------------------------------------------------------------------------------
class ncSoundSource : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSoundSource,nComponentObject);

public:

    /// constructor
    ncSoundSource();
    /// destructor
    virtual ~ncSoundSource();
    /// component persistency
    bool SaveCmds(nPersistServer* ps);

    /// Set trigger that controls the sound source
    void SetTriggerId(nEntityObjectId);

    /// Get id of trigger that controls the sound source
    nEntityObjectId GetTriggerId();

    /// Set fade border distance of sound
    void SetFadeDistance(float);

    /// Get fade border distance of sound
    float GetFadeDistance();

    /// Process enter area event
    void EnterArea(nGameEvent*);

    /// Process exit area event
    void ExitArea(nGameEvent*);

    /// initialize component pointers from entity object
    void InitInstance( nObject::InitInstanceMsg /*initType*/ );

    /// Set trigger that controls the sound source
    void SetTrigger(nEntityObject *trigger);

    /// Change trigger parameters that depend on the sound
    void SynchronizeTriggerShape( nEntityObject * trigger );

    /// Called when entering area trigger
    void OnEnterArea();

    /// Called when exiting area trigger
    void OnExitArea();

private:

    /// fade border distance. Sound is attenuated from border of trigger shape until this distance
    float fadeDistance;

    /// trigger linked to this sound source
    nEntityObjectId triggerEntityId;

};

//------------------------------------------------------------------------------
#endif
