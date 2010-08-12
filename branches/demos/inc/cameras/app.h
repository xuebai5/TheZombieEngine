#ifndef CAMERASAPP_H
#define CAMERASAPP_H

#include "kernel/nref.h"
#include "util/narray.h"
#include "mathlib/matrix.h"

#include "demos/demoapp.h"

//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

class CamerasApp : public DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

    enum CameraMode
    {
        FreeCam = 0,
        LookAt,
        FirstPerson,

        Max_CameraModes,
    };

    CameraMode cameraMode;

private:

    bool bWireframe;

    vector3 vecEye;//eye position
    vector3 vecRot;//euler rotation

    vector3 vecAt; //look-at target
    vector3 vecUp; //up vector

    matrix44 matView;

    nRef<nMesh2> refMesh;
    nRef<nMesh2> refFloorMesh;
    nRef<nTexture2> refTexture;
    nRef<nTexture2> refFloorTexture;
    nRef<nShader2> refShader;
};

#endif //CAMERASAPP_H
