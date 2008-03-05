#ifndef N_PARTICLESHAPENODE2_H
#define N_PARTICLESHAPENODE2_H
//------------------------------------------------------------------------------
/**
    @class nParticleShapeNode2
    @ingroup Scene

    A shape node representing a particle system.

    (C) 2004 RadonLabs GmbH
*/
#include "nscene/nshapenode.h"
#include "gfx2/ndynamicmesh.h"
#include "particle/nparticle2emitter.h"
#include "particle/nparticleserver2.h"

class nRenderContext;
//------------------------------------------------------------------------------
class nParticleShapeNode2 : public nShapeNode
{
public:
    /// constructor
    nParticleShapeNode2();
    /// destructor
    virtual ~nParticleShapeNode2();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);

    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by app when render context has been destroyed
    virtual void EntityDestroyed(nEntityObject* entityObject);
    /// called by scenegraph when object is attached to scene
    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// update transform and render into scene server
    virtual bool RenderTransform(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// render geometry
    virtual bool RenderGeometry(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// perform pre-instancing rending of geometry
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// called by scenegraph when object is rendered
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// get the mesh usage flags required by this shape node
    virtual int GetMeshUsage() const;

    void SetInvisible(bool);
    bool GetInvisible();
    void SetEmissionDuration(nTime);
    nTime GetEmissionDuration() const;
    void SetLoop(bool);
    bool GetLoop() const;
    void SetActivityDistance(float);
    float GetActivityDistance() const;
    void SetRenderOldestFirst(bool);
    bool GetRenderOldestFirst() const;
    void SetStartRotationMin(float);
    float GetStartRotationMin();
    void SetStartRotationMax(float);
    float GetStartRotationMax();
    void SetGravity(float);
    float GetGravity();
    void SetParticleStretch(float);
    float GetParticleStretch();
    void SetTileTexture(int);
    int GetTileTexture();
    void SetStretchToStart(bool);
    bool GetStretchToStart();
    void SetStretchUsingCurve(bool);
    bool GetStretchUsingCurve();
    void SetPrecalcTime(float);
    float GetPrecalcTime();
    void SetStretchDetail(int);
    int GetStretchDetail();
    void SetViewAngleFade(bool);
    bool GetViewAngleFade();
    void SetStartDelay(float);
    float GetStartDelay();

    void SetEmissionFrequency(float, float, float, float, float, float, float, float, int);
    void GetEmissionFrequency(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleLifeTime(float, float, float, float, float, float, float, float, int);
    void GetParticleLifeTime(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleRGB(const vector3&, const vector3&, const vector3&, const vector3&, float, float);
    void GetParticleRGB(vector3&, vector3&, vector3&, vector3&, float&, float&);
    void SetParticleSpreadMin(float, float, float, float, float, float, float, float, int);
    void GetParticleSpreadMin(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleSpreadMax(float, float, float, float, float, float, float, float, int);
    void GetParticleSpreadMax(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleStartVelocity(float, float, float, float, float, float, float, float, int);
    void GetParticleStartVelocity(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleRotationVelocity(float, float, float, float, float, float, float, float, int);
    void GetParticleRotationVelocity(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleSize(float, float, float, float, float, float, float, float, int);
    void GetParticleSize(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleMass(float, float, float, float, float, float, float, float, int);
    void GetParticleMass(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetTimeManipulator(float, float, float, float, float, float, float, float, int);
    void GetTimeManipulator(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleAlpha(float, float, float, float, float, float, float, float, int);
    void GetParticleAlpha(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleVelocityFactor(float, float, float, float, float, float, float, float, int);
    void GetParticleVelocityFactor(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleAirResistance(float, float, float, float, float, float, float, float, int);
    void GetParticleAirResistance(float&, float&, float&, float&, float&, float&, float&, float&, int&);
    void SetParticleStretchVelocity(float, float, float, float, float, float, float, float, int);
    void GetParticleStretchVelocity(float&, float&, float&, float&, float&, float&, float&, float&, int&);

    void SetParticleVelocityRandomize(float);
    float GetParticleVelocityRandomize() const;
    void SetParticleRotationRandomize(float);
    float GetParticleRotationRandomize() const;
    void SetParticleSizeRandomize(float);
    float GetParticleSizeRandomize() const;
    void SetRandomRotDir(bool);
    bool GetRandomRotDir() const;

    #ifndef NGAME
    void SetFuncSrcBlend(int);
    int GetFuncSrcBlend();
    void SetFuncDestBlend(int);
    int GetFuncDestBlend();
    void SetParticleTexture(const char *);
    nString GetParticleTexture();
    void SetParticleMesh(const char *);
    nString GetParticleMesh();
    #endif

    /// set one of the envelope curves (not the color)
    void SetCurve(nParticle2Emitter::CurveType curveType, const nEnvelopeCurve& curve);
    /// get one of the envelope curves
    const nEnvelopeCurve& GetCurve(nParticle2Emitter::CurveType curveType) const;

    /// set the particle rgb curve
    void SetRGBCurve(const nVector3EnvelopeCurve& curve);
    /// get the particle rgb curve
    const nVector3EnvelopeCurve& GetRGBCurve() const;
    /// Returns the current emitter
    nParticle2Emitter* GetEmitter(nEntityObject* entityObject);

    /// get time using time offset
    float GetTime(nEntityObject* entityObject) const;

    #ifndef NGAME
    /// resolve debug mesh for current instance
    virtual nMesh2* GetDebugMesh(nSceneGraph* sceneGraph, nEntityObject* entityObject, const char *name);
    #endif

protected:
    nAutoRef<nParticleServer2> refParticleServer;

    int emitterVarIndex;            // index of the emitter in the rendercontext
    nTime emissionDuration;         // how long shall be emitted ?
    bool loop;                      // loop emitter ?

    float activityDistance;         // distance between viewer and emitter on witch emitter is active
    float startRotationMin;         // minimum angle of rotation at birth
    float startRotationMax;         // maximum angle of rotation at birth
    float gravity;                  // gravity
    float particleStretch;          // amount of stretching
    float particleVelocityRandomize;
    float particleRotationRandomize;
    float particleSizeRandomize;
    float precalcTime;
    bool  randomRotDir;
    int   tileTexture;              // texture tiling
    bool  stretchUsingCurve;        // stretch using velocity curve
    bool  stretchToStart;           // stretch to point of emission
    bool  renderOldestFirst;        // wether to render the oldest particles first or the youngest
    bool  invisible;
    bool  viewAngleFade;
    int   stretchDetail;
    float startDelay;

    nEnvelopeCurve curves[nParticle2Emitter::CurveTypeCount];
    nVector3EnvelopeCurve rgbCurve;

    nVariable::Handle timeHandle;
    nVariable::Handle windHandle;
    nVariable::Handle offsetHandle;

    bool curvesValid;
    float staticCurve[PARTICLE_TIME_DETAIL][nParticle2Emitter::CurveTypeCount];

    int trfmPassIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetEmissionDuration(nTime time)
{
    this->emissionDuration = time;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nParticleShapeNode2::GetEmissionDuration() const
{
    return this->emissionDuration;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetLoop(bool b)
{
    this->loop = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleShapeNode2::GetLoop() const
{
    return this->loop;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetActivityDistance(float f)
{
    this->activityDistance = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetActivityDistance() const
{
    return this->activityDistance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetStartRotationMin(float f)
{
    this->startRotationMin = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetStartRotationMin()
{
    return this->startRotationMin;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetStartRotationMax(float f)
{
    this->startRotationMax = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetStartRotationMax()
{
    return this->startRotationMax;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetGravity(float f)
{
    this->gravity = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetGravity()
{
    return this->gravity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetRenderOldestFirst(bool b)
{
    this->renderOldestFirst = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleShapeNode2::GetRenderOldestFirst() const
{
    return this->renderOldestFirst;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetParticleStretch(float value)
{
    this->particleStretch = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetParticleStretch()
{
    return this->particleStretch;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetPrecalcTime(float value)
{
    this->precalcTime = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetPrecalcTime()
{
    return this->precalcTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetParticleVelocityRandomize(float value)
{
    this->particleVelocityRandomize = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetParticleVelocityRandomize() const
{
    return this->particleVelocityRandomize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetParticleRotationRandomize(float value)
{
    this->particleRotationRandomize = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetParticleRotationRandomize() const
{
    return this->particleRotationRandomize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetParticleSizeRandomize(float value)
{
    this->particleSizeRandomize = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetParticleSizeRandomize() const
{
    return this->particleSizeRandomize;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetRandomRotDir(bool value)
{
    this->randomRotDir = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleShapeNode2::GetRandomRotDir() const
{
    return this->randomRotDir;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetTileTexture(int value)
{
    this->tileTexture = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nParticleShapeNode2::GetTileTexture()
{
    return this->tileTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetStretchToStart(bool value)
{
    this->stretchToStart = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleShapeNode2::GetStretchToStart()
{
    return this->stretchToStart;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetStretchUsingCurve(bool value)
{
    this->stretchUsingCurve = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleShapeNode2::GetStretchUsingCurve()
{
    return this->stretchUsingCurve;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetCurve(nParticle2Emitter::CurveType curveType, const nEnvelopeCurve& curve)
{
    n_assert(curveType < nParticle2Emitter::CurveTypeCount);
    n_assert(curveType >= 0);
    this->curvesValid = false;
    this->curves[curveType].SetParameters(curve);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nEnvelopeCurve&
nParticleShapeNode2::GetCurve(nParticle2Emitter::CurveType curveType) const
{
    n_assert(curveType < nParticle2Emitter::CurveTypeCount);
    n_assert(curveType >= 0);
    return this->curves[curveType];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetRGBCurve(const nVector3EnvelopeCurve& curve)
{
    this->curvesValid = false;
    this->rgbCurve.SetParameters(curve);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nVector3EnvelopeCurve&
nParticleShapeNode2::GetRGBCurve() const
{
    return this->rgbCurve;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetInvisible(bool value)
{
    this->invisible = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleShapeNode2::GetInvisible()
{
    return this->invisible;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetStretchDetail(int value)
{
    this->stretchDetail = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nParticleShapeNode2::GetStretchDetail()
{
    return this->stretchDetail;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetViewAngleFade(bool value)
{
    this->viewAngleFade = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nParticleShapeNode2::GetViewAngleFade()
{
    return this->viewAngleFade;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nParticleShapeNode2::SetStartDelay(float value)
{
    this->startDelay = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nParticleShapeNode2::GetStartDelay()
{
    return this->startDelay;
}

//------------------------------------------------------------------------------
#endif
