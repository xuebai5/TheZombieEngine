#ifndef N_SHOOTEMSTATE_H
#define N_SHOOTEMSTATE_H
//------------------------------------------------------------------------------
/**
    @class nShootemState
    @ingroup NebulaConjurerEditor

    (C) 2010 M.A. Garcias <ma@magarcias.com>
*/
#include "conjurer/nsummonerstate.h"

#include "entity/nentityobject.h"
#include "entity/nrefentityobject.h"

class DemoApp;
//------------------------------------------------------------------------------
class nShootemState : public nCommonState
{
public:
    /// constructor
    nShootemState();
    /// destructor
    virtual ~nShootemState();

    /// Called when state is becoming active
    void OnStateEnter( const nString& prevState );
    /// Called when state is becoming inactive
    virtual void OnStateLeave( const nString& nextState );
    /// Called on state to perform state logic 
    virtual void OnFrame();
    /// Called on state to perform 3d rendering
    virtual void OnRender3D();
    /// Called on state to perform 2d rendering
    //virtual void OnRender2D();

    bool HandleInput(nTime frameTime);

protected:
    /// handle input
    //virtual bool HandleInput(nTime frameTime);
    nRef<nAppViewport> refViewport;
    nRefEntityObject refPlayerEntity;

    //player
    vector3 playerPos;
    vector3 playerRot;
    float playerSpeed;
    float turnSpeed;

    //camera
    vector3 cameraOffset;
    vector3 cameraPos;
    polar2 cameraAngles;
    float cameraThreshold;
};

//------------------------------------------------------------------------------
#endif    
