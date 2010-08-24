#ifndef SHADERSAPP_H
#define SHADERSAPP_H

#include "kernel/nref.h"
#include "util/narray.h"

#include "demos/demoapp.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

class ShadersApp : public DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

private:

    bool bWireframe;

    vector3 vecEye;//eye position
    vector3 vecRot;//euler rotation

    matrix44 matView;
    matrix44 matWorld;

    vector3 vecPosition;
    vector3 vecRotation;
    vector3 vecScale;

    vector3 vecLightPos;
    //vector4 vecLightColor;
    //vector4 vecLightAmbient;

    nRef<nMesh2> refMesh;
    nRef<nTexture2> refTexture;
    nRef<nTexture2> refBumpTexture;
    nRef<nTexture2> refCubeTexture;
    nRef<nShader2> refShader;

    nRef<nShader2> refColorShader;
    nRef<nMesh2> refSphereMesh;

    nRef<nMesh2> refFloorMesh;
    nRef<nTexture2> refFloorTexture;
    nRef<nShader2> refFloorShader;
};

#endif //ShadersApp_H