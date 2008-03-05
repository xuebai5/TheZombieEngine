#ifndef N_SHADOWLIGHTNODE_H
#define N_SHADOWLIGHTNODE_H
//------------------------------------------------------------------------------
/**
    @class nShadowLightNode
    @ingroup Scene
    @brief Scene node which provides terrain lighting information.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/nlightnode.h"

//------------------------------------------------------------------------------
class nShadowLightNode : public nAbstractShaderNode
{
public:
    /// constructor
    nShadowLightNode();
    /// destructor
    virtual ~nShadowLightNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// set the volume projection matrix in the current shader
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// set the cone definition directly
    void SetCamera(const nCamera2& c);
    /// get the cone definition
    const nCamera2& GetCamera() const;

    /// set the cone definition directly
    void ComputeShadowProjection();
    /// get shadow projection
    const matrix44& GetShadowProjection();

    /// set orthogonal parameters
    void SetOrthogonal(float, float, float, float);
    /// set light euler
    void SetEuler(const vector3&);
    /// get euler
    const vector3 GetEuler() const;
    /// set light position
    void SetPosition(const vector3&);
    /// get position
    const vector3 GetPosition() const;
    /// set deformation matrix 2d
    void SetDeformation(const vector4&);
    /// get deformation matrix 2d
    const vector4 GetDeformation() const;

private:
    vector3 position;
    vector3 euler;
    vector4 orthogonal;
    vector4 deformation;

    nCamera2 camera;

    matrix44 shadowProj;
    matrix44 scaledShadowProj;
    bool dirtySProj;
};

//------------------------------------------------------------------------------
#endif
