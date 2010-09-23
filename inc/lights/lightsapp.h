#ifndef LIGHTSAPP_H
#define LIGHTSAPP_H

#include "kernel/nref.h"
#include "util/narray.h"
#include "gfx2/ngfxserver2.h"

#include "demos/demoapp.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

class LightsApp : public DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

private:

    //resources
    nRef<nMesh2> refMesh;
    nRef<nTexture2> refDiffMap;
    nRef<nTexture2> refBumpMap;
    nRef<nTexture2> refCubeMap;

    nRef<nMesh2> refMeshFloor;
    nRef<nTexture2> refTextureFloor;

    nRef<nMesh2> refMeshSphere;

    nRef<nShader2> refShaderColor;
    nRef<nShader2> refShaderDiffuse;

    struct Material
    {
        nRef<nShader2> refShader;
        nShaderParams shaderParams;
        nString technique;
    };

    nArray<Material> materials;
    int curMaterialIndex;

    //common stuff
    bool bWireframe;

    vector3 vecEye;//eye position
    vector3 vecRot;//euler rotation

    matrix44 matView;
    matrix44 matWorld;

    vector3 vecPosition;
    vector3 vecRotation;
    vector3 vecScale;

    vector3 vecLightPos;
    vector4 vecLightAmbient;
    vector4 vecLightDiffuse;
};

#endif //LIGHTSAPP_H
