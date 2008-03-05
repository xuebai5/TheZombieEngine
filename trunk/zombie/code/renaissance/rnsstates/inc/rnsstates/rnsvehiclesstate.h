/**
   @file rnsvehiclesstate.h
   @author Luis Jose Cabellos Gomez
   @brief 
   @brief $Id$ 
   
   (C) 2005 Conjurer Services, S.A.
*/
#ifndef RNSVEHICLESSTATE_H
#define RNSVEHICLESSTATE_H
//------------------------------------------------------------------------------
#include "napplication/nappstate.h"

#include "mathlib/vector.h"
#include "mathlib/polar.h"
#include "mathlib/matrix.h"

class nTransformNode;
class nRenderContext;
class nSceneGraph;
//------------------------------------------------------------------------------
/**
    @class RnsVehiclesState
    @ingroup RnsStates

    @brief
*/
class RnsVehiclesState : public nAppState
{
public:
    /// Constructor
    RnsVehiclesState();
    /// Destructor
    virtual ~RnsVehiclesState();

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

    nRef<nTransformNode> refRootNode;

    nRef<nSceneGraph>  refSceneGraph;
    nEntityObject * renderContext;
    nEntityObject * lightContext;
    nEntityObject * viewContext;

    polar2 viewerAngles;
    vector3 viewerPos;
};

//------------------------------------------------------------------------------
/**
    @param rc render context
*/
inline
void 
RnsVehiclesState::SetRenderContext( nEntityObject * rc )
{
    this->renderContext = rc;
}

//------------------------------------------------------------------------------
/**
    @param rc render context
*/
inline
void 
RnsVehiclesState::SetLightContext( nEntityObject * rc )
{
    this->lightContext = rc;
}

//------------------------------------------------------------------------------
/**
    @param rc render context
*/
inline
void 
RnsVehiclesState::SetViewContext( nEntityObject * rc )
{
    this->viewContext = rc;
}

#endif//RNSVEHICLESSTATE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
