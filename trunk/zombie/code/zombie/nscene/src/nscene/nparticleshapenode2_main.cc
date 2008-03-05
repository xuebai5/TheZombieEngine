#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nparticleshapenode2_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nparticleshapenode2.h"
#include "nscene/ncscene.h"
#include "nscene/nscenegraph.h"
#include "variable/nvariableserver.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nParticleShapeNode2, "nshapenode");

//------------------------------------------------------------------------------
/**
*/
nParticleShapeNode2::nParticleShapeNode2() :
    refParticleServer("/sys/servers/particle2"),
    emissionDuration(10.0),
    loop(true),
    activityDistance(10.0f),
    startRotationMin(0.0f),
    startRotationMax(0.0f),
    gravity(-9.81f),
    emitterVarIndex(-1),
    renderOldestFirst(true),
    curvesValid(false),
    invisible(false),
    startDelay(0),
    trfmPassIndex(-1),
    stretchUsingCurve(false)
{
    // obtain variable handles
    this->timeHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
    this->windHandle = nVariableServer::Instance()->GetVariableHandleByName("wind");
    this->offsetHandle = nVariableServer::Instance()->GetVariableHandleByName("timeOffset");
}

//------------------------------------------------------------------------------
/**
*/
nParticleShapeNode2::~nParticleShapeNode2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.
*/
bool
nParticleShapeNode2::RenderTransform(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    this->InvokeAnimators(entityObject);

    // get emitter from render context
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
    nParticle2Emitter* emitter = (nParticle2Emitter*)varEmitter.GetObj();

//    ncDictionary *varContext = entityObject->GetComponentSafe<ncDictionary>();
//    nVariable* windVar = varContext->GetVariable(this->windHandle);
//    n_assert2(windVar, "No 'wind' variable provided by application!");
    //emitter->SetTransform(this->tform.getmatrix() * parentMatrix);
    emitter->SetTransform(sceneGraph->GetModelTransform()); 
//    emitter->SetWind(windVar->GetFloat4());

    // setup emitter
    emitter->SetMeshGroupIndex(this->groupIndex);
    emitter->SetEmitterMesh(this->refMesh.get());
    emitter->SetGravity(this->gravity);
    emitter->SetTileTexture(this->tileTexture);
    emitter->SetStretchToStart(this->stretchToStart);
    emitter->SetStretchUsingCurve(this->stretchUsingCurve);
    emitter->SetParticleStretch(this->particleStretch);
    emitter->SetStartRotationMin(this->startRotationMin);
    emitter->SetStartRotationMax(this->startRotationMax);
    emitter->SetParticleVelocityRandomize(this->particleVelocityRandomize);
    emitter->SetParticleRotationRandomize(this->particleRotationRandomize);
    emitter->SetParticleSizeRandomize(this->particleSizeRandomize);
    emitter->SetRandomRotDir(this->randomRotDir);
    emitter->SetPrecalcTime(this->precalcTime);
    emitter->SetViewAngleFade(this->viewAngleFade);
    emitter->SetStretchDetail(this->stretchDetail);
    emitter->SetStartDelay(this->startDelay);

    // set emitter settings
    emitter->SetEmissionDuration(this->emissionDuration);
    emitter->SetLoop(this->loop);
    emitter->SetActivityDistance(this->activityDistance);
    emitter->SetRenderOldestFirst(this->renderOldestFirst);
    emitter->SetIsSetUp(true);

    // reset start time for non-looped emitter
    if (!this->loop)
    {
        ncDictionary *varContext = entityObject->GetComponentSafe<ncDictionary>();
        nVariable* var = varContext->FindLocalVar(this->offsetHandle);
        if (var)
        {
            emitter->SetStartTime(var->GetFloat());
        }
    }

    sceneGraph->SetModelTransform(matrix44());

    return true;
}

//------------------------------------------------------------------------------
/**
    history:
        - 30-Mar-2006   ma.garcias  static curves are assigned at creation
                                    for emitters after the first one
*/
void
nParticleShapeNode2::EntityCreated(nEntityObject* entityObject)
{
    nShapeNode::EntityCreated(entityObject);
    ncScene *renderContext = entityObject->GetComponent<ncScene>();

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    nParticle2Emitter* emitter = this->refParticleServer.get()->NewParticleEmitter();
    n_assert(0 != emitter);

    // sample curves to speed up calculating
    if (!this->curvesValid) 
    {
        int c,s;
        for (c = 0; c < nParticle2Emitter::CurveTypeCount; ++c)
        {
            for (s = 0; s < PARTICLE_TIME_DETAIL; ++s)
            {
                this->staticCurve[s][c] = curves[c].GetValue((float)s / (float)PARTICLE_TIME_DETAIL);
            }
        }

        // encode colors
        for (s = 0; s < PARTICLE_TIME_DETAIL; ++s)
        {
            vector3 col = rgbCurve.GetValue((float)s / (float)PARTICLE_TIME_DETAIL);
            this->staticCurve[s][nParticle2Emitter::StaticRGBCurve] = (float)((((uint)(col.x*255.0f)) << 16) |
                                                                              (((uint)(col.y*255.0f)) << 8) |
                                                                              (((uint)(col.z*255.0f)) ));
        }

        // encode alpha values from [0,1] to [0,255] 
        for (s = 0; s < PARTICLE_TIME_DETAIL; ++s)
        {
            this->staticCurve[s][nParticle2Emitter::ParticleAlpha] = (float)(((int)(staticCurve[s][nParticle2Emitter::ParticleAlpha] * 255.0f)));
        }

        this->curvesValid = true;
    }

    if (this->curvesValid)
    {
        emitter->SetStaticCurvePtr((float*)&this->staticCurve);
        emitter->CurvesChanged();
    }

    this->emitterVarIndex = renderContext->AddLocalVar(nVariable(0, emitter));
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode2::EntityDestroyed(nEntityObject* entityObject)
{
    nShapeNode::EntityDestroyed(entityObject);

    // get emitter from render context
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
    nParticle2Emitter* emitter = (nParticle2Emitter*) varEmitter.GetObj();

    n_assert(0 != emitter);
    nParticleServer2::Instance()->DeleteParticleEmitter(emitter);
}

//------------------------------------------------------------------------------
/**
    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
    - 28-Jan-04     daniel  emitter setup moved to RenderTransform()
*/
void
nParticleShapeNode2::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
//    nShapeNode::Attach(sceneGraph, entityObject);
    if (this->trfmPassIndex == -1)
    {
        this->trfmPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('trfm'));
    }

    sceneGraph->BeginGroup(this->trfmPassIndex, this, entityObject);
    nShapeNode::Attach(sceneGraph, entityObject);
    sceneGraph->EndGroup();
}

//------------------------------------------------------------------------------
/**
    This method must return the mesh usage flag combination required by
    this shape node class. Subclasses should override this method
    based on their requirements.

    @return     a combination of nMesh2::Usage flags
*/
int
nParticleShapeNode2::GetMeshUsage() const
{
    return nMesh2::ReadOnly | nMesh2::PointSprite | nMesh2::NeedsVertexShader;
}

//------------------------------------------------------------------------------
/**
    Perform pre-instance-rendering of particle system.
    FIXME: check if this is the optimal setup for the new instance 
    rendering!
*/
bool
nParticleShapeNode2::Apply(nSceneGraph* sceneGraph)
{
    return nGeometryNode::Apply(sceneGraph);
}

//------------------------------------------------------------------------------
/**
    Render
*/
bool
nParticleShapeNode2::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (sceneGraph->GetCurrentPassIndex() == this->trfmPassIndex)
    {
        return this->RenderTransform(sceneGraph, entityObject);
    }
    else
    {
        return this->RenderGeometry(sceneGraph, entityObject);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nParticleShapeNode2::RenderGeometry(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    #ifndef NGAME
    if (sceneGraph->GetShaderIndex() == this->wireframeShaderIndex)
    {
        nShader2* curShader = nGfxServer2::Instance()->GetShader();
        if (curShader->IsParameterUsed(nShaderState::isSkinned))
        {
            curShader->SetInt(nShaderState::isSkinned, 4);//particle
        }
    }
    #endif

    if (nGeometryNode::Render(sceneGraph, entityObject)) 
    {
		ncScene *renderContext = entityObject->GetComponentSafe<ncScene>();
        const nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
        nTime curTime = this->GetTime(entityObject);
        n_assert(curTime >= 0.0f);
       
        nParticle2Emitter* emitter = (nParticle2Emitter*)varEmitter.GetObj();
        n_assert(0 != emitter);
        if(!this->invisible)
        {
            emitter->Render(curTime);
        }

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Returns the current emitter
*/
nParticle2Emitter*
nParticleShapeNode2::GetEmitter(nEntityObject* entityObject)
{
    ncScene* renderContext = entityObject->GetComponent<ncScene>();

    // get emitter from render context
    nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
    return (nParticle2Emitter*)varEmitter.GetObj();
}

//------------------------------------------------------------------------------
/**
*/
float
nParticleShapeNode2::GetTime(nEntityObject* entityObject) const 
{
    // get the sample time from the render context
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    float curTime = (float) renderContext->GetTime();

    if (this->offsetHandle != nVariable::InvalidHandle)
    {
        ncDictionary *varContext = entityObject->GetComponentSafe<ncDictionary>();
        nVariable* var = varContext->FindLocalVar(this->offsetHandle);
        if (var)
        {
            curTime = curTime - var->GetFloat();
        }
        else
        {
            varContext->AddLocalVar(nVariable(this->offsetHandle, curTime));
        }
    }

    return curTime;
}

#ifndef NGAME

//------------------------------------------------------------------------------
/**
*/
nMesh2*
nParticleShapeNode2::GetDebugMesh(nSceneGraph* sceneGraph, nEntityObject* entityObject, const char *name)
{
    n_assert(name);

    if (strcmp(name, "emitter") == 0)
    {
        nString meshName;
        meshName.Format("%s_%s", this->GetMesh(), name);

        nMesh2* mesh = 0;
        mesh = static_cast<nMesh2*>(nResourceServer::Instance()->FindResource(meshName.Get(), nResource::Mesh));
        if (mesh)
        {
            return mesh;
        }
        else
        {
            mesh = nGfxServer2::Instance()->NewMesh(meshName.Get());
            if (mesh)
            {
                mesh->SetFilename(this->GetMesh());
                if (mesh->Load())
                {
                    return mesh;
                }
            }
        }
    }
    else if (strcmp(name, "normal") == 0)
    {
        return nShapeNode::GetDebugMesh(sceneGraph, entityObject, name);
    }

    return 0;
}

#endif
