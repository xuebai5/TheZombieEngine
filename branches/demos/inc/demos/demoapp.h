#ifndef DEMOAPP_H
#define DEMOAPP_H
//------------------------------------------------------------------------------
/**
    @class DemoApp

    A minimal demo framework for Zombie apps
*/

#include "util/nstring.h"
#include "mathlib/matrix.h"
//------------------------------------------------------------------------------

class nMesh2;
class nTexture2;
class nShader2;
class nResource;

#define N_REF_LOAD_MESH(a,name,file) a = gfxServer->NewMesh(name);\
    if (!this->LoadResource(a, file)) return false;

#define N_REF_LOAD_TEXTURE(a,name,file) a = gfxServer->NewTexture(name);\
    if (!this->LoadResource(a, file)) return false;

#define N_REF_LOAD_SHADER(a,name,file) a = gfxServer->NewShader(name);\
    if (!this->LoadResource(a, file)) return false;

#define N_REF_RELEASE(a) if (a.isvalid()) {a->Release(); a.invalidate();}

class DemoApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

protected:

    bool LoadResource( nResource* pResource, const nString& strFilename );

    //draw helpers
    int BeginDraw( nShader2* pShader, nMesh2* pMesh, const char* technique=0 );
    void BeginPass( nShader2* pShader, int passIndex );
    void Draw( const vector3& vPosition, const vector3& vScale );
    void Draw( const matrix44& matWorld );
    void EndPass( nShader2* );
    void EndDraw( nShader2* );
};

#endif //DEMOAPP_H
