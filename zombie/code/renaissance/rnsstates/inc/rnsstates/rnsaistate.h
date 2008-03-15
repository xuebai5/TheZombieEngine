#ifndef RNSAISTATE_H
#define RNSAISTATE_H

//------------------------------------------------------------------------------
/**
    @class RnsAiState
    @ingroup RnsStates
    @author Luis Jose Cabellos Gomez
   
    (C) 2005 Conjurer Services, S.A.
*/

#include "napplication/nappstate.h"

#include "mathlib/vector.h"
#include "mathlib/polar.h"
#include "mathlib/matrix.h"

#include "nphysics/nphysicsgeom.h"

#undef  NUM_JP 
#define NUM_JP 1

class nTransformNode;
class nRenderContext;
class nSceneGraph;

class nPhyGeomRay;
class nGfxServer2;

class nGPActionManager;
class nNavMesh;
class nPathFinder;

class nAppViewport;

//------------------------------------------------------------------------------
/**
    @class RnsAiState
    @ingroup RnsStates

    @brief
*/
class RnsAiState : public nAppState
{
public:
    /// Constructor
    RnsAiState();
    /// Destructor
    virtual ~RnsAiState();

    /// Called when state is created
    virtual void OnCreate( nApplication* application );

    /// Called when state is becoming active
    virtual void OnStateEnter( const nString & prevState );
    /// Called when state is becoming inactive
    virtual void OnStateLeave( const nString & nextState );
    /// Called on state to perform state logic 
    virtual void OnFrame();
    /// Called on state to perform 3d rendering
    virtual void OnRender3D();
    /// Called on state to perform 2d rendering
    virtual void OnRender2D();

private:
    /// Handle general input
    void HandleInput(const float frameTime);
    vector3 ConvertScreenToViewSpace( const vector2& screenCoord );

    /// Navigation mesh
    void CreatePathFinder();
    void CreateNavigationMesh();

    /// Initializes AI System
    void InitAISystem();
    void InitAIEntities();
    void DestroyAIEntities();
    void ShutDownAISystem();

    /// Draw cross at the center of the screen
    void DrawCross (nGfxServer2* server);

    /// Test path finder
    void SetPathPoint();

    /// Test stream class
    void TestStream();

    /// Test entity movement and facing
    void MoveEntity();
	void FaceEntity();
    void MoveToMouse();
	void JumpEntity();

    nNavMesh*           navMesh;
    bool                bStart;
    vector3             start, goal;
    nPathFinder*		pathFinder;

    // AI requirement
    bool                loaded;
    nGPActionManager*   actionManager;

    // Entities test
    nEntityObject*      entity;
    nEntityObject*      jp[NUM_JP];

    nRef<nAppViewport>  refViewport;

    matrix44            viewMatrix;
    polar2              viewerAngles;
    vector3             viewerPos;

    nRef<nPhyGeomRay>   shot;
    geomid              shotObj;
    vector3             shotDirection;
    vector3             shotPosition;

    bool                showPhysics;
    bool                showPathInfo;
    bool                showPathNodes;
    bool                showNavMesh;
    bool                showEntityDebugInfo;
};

#endif//RNSAISTATE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
