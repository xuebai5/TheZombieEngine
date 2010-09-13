#ifndef SHOOTEMAPP_H
#define SHOOTEMAPP_H
//------------------------------------------------------------------------------
/**
    @class ShootemApp

    A minimal demo framework for Zombie apps
*/
#include "util/nstring.h"
#include "mathlib/matrix.h"

#include "kernel/nref.h"
#include "util/narray.h"
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

class ShootemApp
{
public:
    
    virtual void Init();
    
    virtual bool Open();
    virtual void Close();
    virtual void Tick( float );

    virtual void Render();

protected:

    //options
    bool bWireframe;
    bool bCameraOrtho;

    vector3 vecEye;
    vector3 vecRot;

    vector3 vecPlayerPos;
    vector3 vecCameraOffset;

    float fCameraThreshold;

    matrix44 matView;
    matrix44 matProj;

    float fPlayerSpeed;
    float fPlayerSize;

    //fire
    struct Projectile
    {
        float fTimeElapsed;
        vector3 vecPos;//position
        vector3 vecDir;//direction
        vector3 vecSize;//size for collision(x,y,z)
    };

    nArray<Projectile> projectiles;//current projectiles
    float fProjectileMaxTime;
    float fProjectileSpeed;

    void AddProjectile();
    void TickProjectiles(float fTimeElapsed);
    void DrawProjectiles();

    //tiles
    struct Tile
    {
        vector3 vecPos;
        vector3 vecScale;
        vector4 color;
    };

    nArray<Tile> tiles;

    //props
    struct Prop
    {
        vector3 vecPos;
        vector3 vecScale;
        vector4 color;
        bool highlight;
    };

    nArray<Prop> props;
    void DrawProps();
    Prop* CheckProps(const vector3& pos, float fDistance);

    //resources
    nRef<nMesh2> refMeshGround;
    nRef<nTexture2> refTextureGround;

    nRef<nMesh2> refMeshCylinder;
    nRef<nMesh2> refMeshSphere;
    nRef<nMesh2> refMeshCone;

    nRef<nShader2> refShaderColor; //diffusecolor-only shader
    nRef<nShader2> refShaderDiffuse; //diffusemap-only shader

    //---copied from DemoApp---
    bool LoadResource( nResource* pResource, const nString& strFilename );

    //draw helpers
    int BeginDraw( nShader2* pShader, nMesh2* pMesh );
    int BeginDraw( nShader2* );
    void BeginDraw( nMesh2* );
    void BeginPass( nShader2* pShader, int passIndex );
    void Draw( const vector3& vPosition, const vector3& vScale );
    void Draw( const matrix44& matWorld );
    void EndPass( nShader2* );
    void EndDraw( nShader2* );
};

#endif //SHOOTEMAPP_H
