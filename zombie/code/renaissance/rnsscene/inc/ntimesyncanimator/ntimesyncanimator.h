#ifndef N_TIMESYNCANIMATOR_H
#define N_TIMESYNCANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nTimeSyncAnimator
    @ingroup SceneNodes
    @author Cristobal Castillo

    @brief Reset the entity's time's ofsset  Animates the UV transform parameters on an nAbstractShaderNode.

    Reset the time ofsset of 

    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/nanimator.h"
#include "util/nanimkeyarray.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
class nTimeSyncAnimator : public nAnimator
{
public:
    /// constructor
    nTimeSyncAnimator();
    /// destructor
    virtual ~nTimeSyncAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// return the type of this animator object (SHADER)
    virtual Type GetAnimatorType() const;
    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);
    /// Set the cycle time
    void SetCycleTime(float time);
    /// Get the cycle time
    float GetCycleTime() const;
    ///  Set the offset time
    void SetOffsetTime(float time);
    /// get the offset time
    float GetOffsetTime() const;
private:
    float cycleTime;
    float offsetTime;
};


//------------------------------------------------------------------------------
#endif
