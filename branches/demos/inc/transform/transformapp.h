#ifndef TRANSFORMAPP_H
#define TRANSFORMAPP_H

#include "kernel/nref.h"
#include "util/narray.h"
#include "mathlib/matrix.h"
#include "mathlib/bbox.h"

#include "demos/demoapp.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

class TransformApp : public DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

private:

    enum TransformMode
    {
        Translate = 0,
        Rotate,
        Scale,

        Max_TransformModes
    };

    bool bWireframe;

    int transformMode;

    vector3 vecEye;//eye position
    vector3 vecRot;//euler rotation

    matrix44 matView;
    matrix44 matWorld;

    bbox3 bbox;
    vector3 vecPosition;
    vector3 vecRotation;
    vector3 vecScale;

    nRef<nMesh2> refMesh;
    nRef<nMesh2> refFloorMesh;
    nRef<nTexture2> refTexture;
    nRef<nTexture2> refFloorTexture;
    nRef<nShader2> refShader;
};

#endif //TRANSFORMAPP_H
