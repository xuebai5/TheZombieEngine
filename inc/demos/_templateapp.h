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
    nRef<nShader2> refShader;
};

#endif //_TEMPLATEAPP_H
