#ifndef MODELSAPP_H
#define MODELSAPP_H

#include "demos/demoapp.h"
#include "models/model.h"

#include "kernel/nref.h"
#include "util/narray.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;
class nGfxServer2;

class ModelsApp : public DemoApp
{
public:

    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

private:

    nArray<Model> models;

    bool LoadModel(Model* model, const char* path);
    void ReleaseModel(Model*);
    void DrawModels();

    bool LoadScene(Model* model, const char* filename);
    bool LoadMaterial(Shape* shape, const char* filename);

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
