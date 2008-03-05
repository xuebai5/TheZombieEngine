#ifndef NC_SCENEDOTLASER_H
#define NC_SCENEDOTLASER_H
//------------------------------------------------------------------------------
/**
    @class ncSceneDotLaser
    @ingroup Scene
    @author Cristobal Castilllo

    @brief Graphics component object for dot laser entities.

    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/ncscenelight.h"

//------------------------------------------------------------------------------
class  ncSceneDotLaser: public ncSceneLight
{
    NCOMPONENT_DECLARE(ncSceneDotLaser , ncSceneLight);

public:
    /// constructor
    ncSceneDotLaser();
    /// destructor
    virtual ~ncSceneDotLaser();
    /// initialize component pointers from entity object
    void InitInstance(nObject::InitInstanceMsg initType);
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// render
    virtual void Render(nSceneGraph* sceneGraph);
    /// check if the entity is affected by this light
    virtual bool AffectsEntity(nEntityObject* entityObject);
    /// get the  light projection , tranform world to light projection
    const matrix44& GetLightProjection() const;
private:
    /// Get the contactPosition.
    void GetContactPosition( const matrix44& model, vector3& contactPosition , float& distance , nEntityObject *& receiver) const;
    /// Update the projection from entityclass , and update all component and scene
    void UpdateLaserProjection();
#ifndef NGAME
    /// Update the object when class is modified
    void TriggerClassModified(nObject* emitter);
#endif //!NGAME
    /// This entity receive the laser
    nEntityObject* entityReceiver;
    /// if all objects receive the red dot-laser;
    bool allAreCandidates;
    /// The collision point
    vector3 contactPosition;
};

//------------------------------------------------------------------------------
/**
*/

//------------------------------------------------------------------------------
#endif //!NC_SCENEDOTLASER_H
