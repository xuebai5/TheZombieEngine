#ifndef N_CAMERANODE_H
#define N_CAMERANODE_H
//------------------------------------------------------------------------------
/**
    @class nCameraNode
    @ingroup Camera
    @brief Extends nAbstractCameraNode. Is a fixed camera with no intelligence.

    author: matthias
    (C) 2004 RadonLabs GmbH
*/

#include "nscene/nabstractcameranode.h"

//------------------------------------------------------------------------------
class nCameraNode : public nAbstractCameraNode
{
public:
    /// constructor
    nCameraNode();
    /// destructor
    virtual ~nCameraNode();

    /// function wich updates the camera 
    virtual bool RenderCamera(const matrix44& modelWorldMatrix, const matrix44& viewMatrix, const matrix44& projectionMatrix);    

    /// called by nSceneGraph when object is attached to scene
    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);

protected:
    /// compute the viewmatrix
    matrix44 ComputeViewMatrix(const vector3& cameraPosition, const vector3& cameraDirection);    

    int viewPassIndex;
};

//------------------------------------------------------------------------------
#endif
