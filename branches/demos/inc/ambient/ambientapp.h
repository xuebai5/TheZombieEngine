#ifndef AMBIENTAPP_H
#define AMBIENTAPP_H

#include "kernel/nref.h"
#include "util/narray.h"

#include "demos/demoapp.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

class AmbientApp : public DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

private:

    struct Model
    {
        struct {
            nRef<nMesh2> refMesh;
            nRef<nShader2> refShader;
            nRef<nTexture2> refDiffMap;
            nRef<nTexture2> refSpecMap;
        } shapes[2];
    };

    Model model;

    nRef<nMesh2> refMesh;
    nRef<nTexture2> refTexture;
    nRef<nShader2> refShader;

    nRef<nMesh2> refSphereMesh;//to display the cube map
    nRef<nShader2> refCubeShader;//to display the cube map

    nRef<nTexture2> refCubeTexture;
    nRef<nTexture2> refDiffCubeTexture;

    bool bWireframe;
    bool bShowDiffMap;

    vector3 vecEye;//eye position
    vector3 vecRot;//euler rotation

    matrix44 matView;
    matrix44 matWorld;

};

#endif //_TEMPLATEAPP_H
