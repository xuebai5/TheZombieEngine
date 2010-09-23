#ifndef _TEMPLATEAPP_H
#define _TEMPLATEAPP_H

#include "kernel/nref.h"
#include "util/narray.h"

#include "demos/demoapp.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

class _TemplateApp : public DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

private:

    nRef<nMesh2> refMesh;
    nRef<nTexture2> refTexture;

    nRef<nMesh2> refMeshFloor;
    nRef<nTexture2> refTextureFloor;

    nRef<nShader2> refShaderColor;
    nRef<nShader2> refShaderDiffuse;

    bool bWireframe;

    vector3 vecEye;//eye position
    vector3 vecRot;//euler rotation

    matrix44 matView;
    matrix44 matWorld;

    vector3 vecPosition;
    vector3 vecRotation;
    vector3 vecScale;

    vector3 vecLightPos;
};

#endif //_TEMPLATEAPP_H
