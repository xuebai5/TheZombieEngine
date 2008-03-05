/**
   @file RnsWanderState.h
   @brief This state belongs to the renaissance project and its main purpose is
    to allow a character to wonder around the scenerios.
   @brief $Id$ 
   
   (C) 2005 Conjurer Services, S.A.
*/

#ifndef RnsWanderState_H
#define RnsWanderState_H

//------------------------------------------------------------------------------

#include "kernel/ntypes.h"
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/nroot.h"
#include "kernel/nclass.h"
#include "kernel/nobject.h"
#include "kernel/nref.h"

#include "napplication/nappviewport.h"

#include "napplication/nappstate.h"

//------------------------------------------------------------------------------

class nInputServer;
class nEntityObject;
class nPhyMaterial;
class ncGameCamera;
class ncTransform;

//------------------------------------------------------------------------------
class RnsWanderState : public nAppState
{
public:
    /// default contructor
    RnsWanderState();

    /// destructor
    ~RnsWanderState();

    // begin: Inheret functionality from nAppState

    /// called when state is created
    void OnCreate(nApplication* application);
    /// called when state is becoming active
    void OnStateEnter(const nString& prevState);
    /// called when state is becoming inactive
    void OnStateLeave(const nString& nextState);
    /// called on state to perform state logic 
    void OnFrame();
    /// called on state to perform 3d rendering
    void OnRender3D();
    /// called on state to perform 2d rendering
    void OnRender2D();
    /// called before nSceneServer::RenderScene()
    void OnFrameBefore();
    /// called after nSceneServer::RenderScene()
    void OnFrameRendered();

    // end: Inheret functionality from nAppState

    /// sets the previous viewport
    void SetPreviousViewport( nAppViewport* prevViewport );

private:

    /// prepares viewport
    void PrepareViewPort();

    /// updates input
    void UpdateInput();

    /// updates camera
    void UpdateCamera();

    /// stores viewport info
    nRef<nAppViewport> viewport;      ///< game viewport

    /// stores a pointer to the input server
    nInputServer* input;

    /// stores the last state
    nString previousState;

    /// creates the player information
    void CreatePlayer();

    /// destroys info
    void Destroy();

    /// manage player movement
    void UpdatePlayerMovement();

    /// physics character
    nEntityObject* physicCharacter;

    /// transform player
    ncTransform* transformCharacter;

    /// physics component object
    ncPhysicsObj* physicsObject;

    /// store the previous viewport
    nAppViewport* oldViewport;

    /// stores the proportion of time elapsed between frames
    float frameProportion;

    /// stores the orientation
    vector3 orientation;

    /// store the physics material
    nPhyMaterial* phyMaterial;

    /// creates a graphic dummy representing the player
    void CreateGraphicDummy();

    /// stores the graphic dummy transform
    ncTransform* transformDummy;

    /// stores if some input has hapened
    bool inputInputed; // :)

    /// dummy character
    nEntityObject* dummyCharacter;

    /// stores the game camera
    ncGameCamera* gCamera;
};

#endif