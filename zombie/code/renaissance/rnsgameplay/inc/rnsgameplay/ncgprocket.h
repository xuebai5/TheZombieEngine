#ifndef NCGPROCKET_H
#define NCGPROCKET_H
//------------------------------------------------------------------------------
/**
    @class ncGPRocket
    @ingroup Entities

    Component Object that represent a Rocket Propelled Grenade gameplay.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsgameplay/ncgpthrowable.h"

//------------------------------------------------------------------------------
class ncPhyCompositeObj;
class ncTransform;
class nPhyGeomRay;
struct nLaunchSpecialFxData;
#ifndef NGAME
class nDebugTrail;
class nGfxServer2;
#endif//!NGAME

//------------------------------------------------------------------------------
class ncGPRocket : public ncGPThrowable
{

    NCOMPONENT_DECLARE(ncGPRocket,ncGPThrowable);

public:
    /// constructor
    ncGPRocket();
    /// destructor
    virtual ~ncGPRocket();

    /// runs the gameplay logic 
    virtual void Run ( const float deltaTime);
#ifndef NGAME
    /// debug draw of entity
    virtual void DebugDraw( nGfxServer2 * const gfxServer );
#endif//!NGAME

    /// set initial state of the rocket
    void SetInitial( const vector3 & dir, const vector3 & pos );

    /// create the rocket
    void Create();

    /// say if rocket is finaliced
    bool IsDone();

private:
    // rotate the object to follow the direction
    void UpdateObjectDirection();

    ncPhyCompositeObj * object;
    ncTransform * transform;
    nPhyGeomRay * ray;

    vector3 lastPos;
    vector3 actualPos;
    vector3 direction;
    vector3 initialPos;
    vector3 finalPos;
    float power;

    float meters;
    float lifeTime;

#ifndef NGAME
    nDebugTrail * trail;
#endif//!NGAME

};

//------------------------------------------------------------------------------
#endif//NCGPROCKET_H
