#ifndef RNSPHYSICSSTATE_H
#define RNSPHYSICSSTATE_H
//------------------------------------------------------------------------------
/**
    @class RnsPhysicsState
    @author Luis Jose Cabellos Gomez
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "napplication/nappstate.h"

#include "mathlib/vector.h"
#include "mathlib/polar.h"
#include "mathlib/matrix.h"

#include "nphysics/nphysicsgeom.h"

class nTransformNode;
class nRenderContext;
class nSceneGraph;
class nPhyGeomRay;
//------------------------------------------------------------------------------
/**
    @class RnsPhysicsState
    @ingroup RnsStates

    @brief
*/
class RnsPhysicsState : public nAppState
{
public:
    /// Constructor
    RnsPhysicsState();
    /// Destructor
    virtual ~RnsPhysicsState();

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

    /// Set the render context
    void SetRenderContext( nEntityObject * rc );
    /// Set the light context
    void SetLightContext( nEntityObject * rc );
    /// Set the view context
    void SetViewContext( nEntityObject * rc );

private:
    /// Handle general input
    void HandleInput(const float frameTime);
    vector3 ConvertScreenToViewSpace( const vector2& screenCoord );

    nRef<nTransformNode> refRootNode;

    nRef<nSceneGraph>  refSceneGraph;
    nEntityObject * renderContext;
    nEntityObject * lightContext;
    nEntityObject * viewContext;

    matrix44 viewMatrix;
    polar2 viewerAngles;
    vector3 viewerPos;

    nRef<nPhyGeomRay> shot;
    geomid shotObj;
    vector3 shotDirection;
    vector3 shotPosition;
};

//------------------------------------------------------------------------------
/**
    @param rc render context
*/
inline
void 
RnsPhysicsState::SetRenderContext( nEntityObject * rc )
{
    this->renderContext = rc;
}

//------------------------------------------------------------------------------
/**
    @param rc render context
*/
inline
void 
RnsPhysicsState::SetLightContext( nEntityObject * rc )
{
    this->lightContext = rc;
}

//------------------------------------------------------------------------------
/**
    @param rc render context
*/
inline
void 
RnsPhysicsState::SetViewContext( nEntityObject * rc )
{
    this->viewContext = rc;
}

#endif//RNSPHYSICSSTATE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
