#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncameranode_main.cc
//  (C) 2004 RadonLabs GmbH
//  author: matthias
//------------------------------------------------------------------------------
#include "nscene/ncameranode.h"
#include "mathlib/polar.h"

nNebulaClass(nCameraNode, "nabstractcameranode");

//------------------------------------------------------------------------------
/**
*/
nCameraNode::nCameraNode() :
    viewPassIndex(-1)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nCameraNode::~nCameraNode()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
matrix44 
nCameraNode::ComputeViewMatrix(const vector3& cameraPosition, const vector3& cameraDirection)
{      
    matrix44 viewMatrix;
    
    // initialize theta and rho
    polar2 viewerAngles;
    
    vector3 _cameraDirection = cameraDirection;
    _cameraDirection.norm();
    
    viewerAngles.set(_cameraDirection);
    viewerAngles.theta -= N_PI*0.5f;

    viewMatrix.ident();
    viewMatrix.rotate_x(viewerAngles.theta);
    viewMatrix.rotate_y(viewerAngles.rho);

    // initialize point from camera
    vector3 _cameraPosition;
    viewMatrix.mult(cameraPosition, _cameraPosition);
    viewMatrix.pos_component() = _cameraPosition;

    return viewMatrix;
}


//------------------------------------------------------------------------------
/**
*/
bool
nCameraNode::RenderCamera(const matrix44& modelWorldMatrix, const matrix44& /*viewMatrix*/, const matrix44& projectionMatrix)
{
    // projectionmatrix
    this->projMatrix = projectionMatrix;

    // viewmatrix
    //this->viewMatrix = this->ComputeViewMatrix(modelWorldMatrix.pos_component(), modelWorldMatrix.z_component());

    // @todo can't see why a nCameraNode::ComputeViewMatrix() is required
    this->viewMatrix = modelWorldMatrix;
    this->viewMatrix.invert_simple();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nCameraNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    nAbstractCameraNode::Attach(sceneGraph, entityObject);
}
