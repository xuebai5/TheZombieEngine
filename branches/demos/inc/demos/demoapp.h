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
    int BeginDraw( nShader2* pShader, nMesh2* pMesh );
    void BeginPass( nShader2* pShader, int passIndex );
    void Draw( const vector3& vPosition, const vector3& vScale );
    void Draw( const matrix44& matWorld );
    void EndPass( nShader2* );
    void EndDraw( nShader2* );
};

#endif //DEMOAPP_H
