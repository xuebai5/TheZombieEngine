#ifndef NCGPGRENADE_H
#define NCGPGRENADE_H
//------------------------------------------------------------------------------
/**
    @class ncGPGrenade
    @ingroup Entities

    Component Object that represent a Grenade gameplay.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsgameplay/ncgpthrowable.h"

//------------------------------------------------------------------------------
class ncPhyCompositeObj;
struct nLaunchSpecialFxData;

//------------------------------------------------------------------------------
class ncGPGrenade : public ncGPThrowable
{

    NCOMPONENT_DECLARE(ncGPGrenade,ncGPThrowable);

public:
    /// constructor
    ncGPGrenade();
    /// destructor
    virtual ~ncGPGrenade();

    /// runs the gameplay logic 
    virtual void Run ( const float time);

#ifndef NGAME
    /// debug draw of entity
    virtual void DebugDraw( nGfxServer2 * const gfxServer );
#endif//!NGAME

    /// set initial state of the grenade
    void SetInitial( const vector3 & dir, const vector3 & pos );

    /// create the grenade
    void Create();

    /// say if grenade is finaliced and need to explode
    bool IsDone();

private:
    ncPhyCompositeObj * object;

    vector3 direction;
    vector3 initialPos;
    float lifeTime;

};

//------------------------------------------------------------------------------
#endif//NCGPGRENADE_H
