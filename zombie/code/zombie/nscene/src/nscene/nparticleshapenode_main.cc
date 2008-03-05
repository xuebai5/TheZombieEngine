#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nparticleshapenode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nparticleshapenode.h"
#include "variable/nvariableserver.h"
#include "nscene/ncscene.h"
#include "nscene/nscenegraph.h"
#include "kernel/ntimeserver.h"
#include "nscene/nanimator.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nParticleShapeNode, "nshapenode");

//------------------------------------------------------------------------------
/**
*/
nParticleShapeNode::nParticleShapeNode() :
    refParticleServer("/sys/servers/particle"),
    emissionDuration(10.0),
    loop(true),
    activityDistance(10.0f),
    spreadAngle(0.0f),
    birthDelay(0.0f),
    emitterVarIndex(-1),
    renderOldestFirst(true),
    globalScale(1.0f),
    particlesFollowNode(false),
    doReset(false),
    active(true),
    trfmPassIndex(-1)
{
    int i;
    for (i=0; i<4; i++)
    {
        this->curves[nParticleEmitter::ParticleVelocityFactor].keyFrameValues[i] = 1.0;
    }

    // obtain variable handles
    this->timeHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
    this->windHandle = nVariableServer::Instance()->GetVariableHandleByName("wind");
}

//------------------------------------------------------------------------------
/**
*/
nParticleShapeNode::~nParticleShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
    - 28-Jan-04     daniel  emitter setup moved to RenderTransform()
*/
void
nParticleShapeNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
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
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.

    *** FIXME FIXME FIXME ***
    * why is emitter setup in RenderTransform() and not in RenderGeometry()???
    * generally do some cleanup on the particle subsystem

    - 28-Jan-04     daniel  emitter setup moved here from RenderTransform()
*/
bool
nParticleShapeNode::RenderTransform(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    this->InvokeAnimators(entityObject);

    // get emitter from render context
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    //ncDictionary *varContext = entityObject->GetComponent<ncDictionary>();
    nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
    int emitterKey = varEmitter.GetInt();
    nParticleEmitter* emitter = nParticleServer::Instance()->GetParticleEmitter(emitterKey);
    if(this->IsResetting())
    {
        emitter = 0;
        this->doReset = false;
    }

    // keep emitter alive
    if (0 == emitter)
    {
        // need new emitter
        emitter = nParticleServer::Instance()->NewParticleEmitter();
        n_assert(0 != emitter);

        varEmitter.SetInt(emitter->GetKey());
    }

    // setup emitter
    emitter->SetMeshGroupIndex(this->groupIndex);
    emitter->SetEmitterMesh(this->refMesh.get());
    emitter->SetTransform(sceneGraph->GetModelTransform()); 
    /// @todo use a windWar
    //nVariable* windVar = varContext->GetVariable(this->windHandle);
    //n_assert2(windVar, "No 'wind' variable provided by application!");    
    //emitter->SetWind(windVar->GetFloat4());

    nFloat4 www= { 1.0f, 0.0f, 0.0f, 0.5f };
    emitter->SetWind(www);

    // set emitter settings
    emitter->SetEmissionDuration(this->emissionDuration);
    emitter->SetLoop(this->loop);
    emitter->SetActivityDistance(this->activityDistance);
    emitter->SetSpreadAngle(this->spreadAngle);
    emitter->SetBirthDelay(this->birthDelay);
    emitter->SetStartRotation(this->startRotation);
    emitter->SetRenderOldestFirst(this->renderOldestFirst);
    emitter->SetScale(this->globalScale);
    emitter->SetParticlesFollowEmitter(this->particlesFollowNode);
    emitter->SetActive(this->IsEffectActive());
    int curveType;
    for (curveType = 0; curveType < nParticleEmitter::CurveTypeCount; curveType++)
    {
        emitter->SetCurve((nParticleEmitter::CurveType) curveType, this->curves[curveType]);
    }
    emitter->SetRGBCurve(this->rgbCurve);

    sceneGraph->SetModelTransform(matrix44());

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nParticleShapeNode::EntityCreated(nEntityObject* entityObject)
{
    nShapeNode::EntityCreated(entityObject);
    ncScene *renderContext = entityObject->GetComponent<ncScene>();

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    nParticleEmitter* emitter = nParticleServer::Instance()->NewParticleEmitter();
    n_assert(0 != emitter);

    // put emitter key in render context
    this->emitterVarIndex = renderContext->AddLocalVar(nVariable(0, emitter->GetKey()));
}

//------------------------------------------------------------------------------
/**
    This method must return the mesh usage flag combination required by
    this shape node class. Subclasses should override this method
    based on their requirements.

    @return     a combination of nMesh2::Usage flags
*/
int
nParticleShapeNode::GetMeshUsage() const
{
    return nMesh2::ReadOnly | nMesh2::PointSprite | nMesh2::NeedsVertexShader;
}

//------------------------------------------------------------------------------
/**
    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
bool
nParticleShapeNode::RenderGeometry(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    #ifndef NGAME
    if (sceneGraph->GetShaderIndex() == this->wireframeShaderIndex)
    {
        nShader2* curShader = nGfxServer2::Instance()->GetShader();
        if (curShader->IsParameterUsed(nShaderState::isSkinned))
        {
            curShader->SetInt(nShaderState::isSkinned, 5);//particle1
            curShader->SetInt(nShaderState::CullMode, nShaderState::NoCull);
        }
    }
    #endif

    if (nGeometryNode::Render(sceneGraph, entityObject)) 
    {
        ncScene *renderContext = entityObject->GetComponent<ncScene>();

        const nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
        int emitterKey = varEmitter.GetInt();
        float curTime = (float) renderContext->GetTime();
        n_assert(curTime >= 0.0f);
        
        nParticleEmitter* emitter = nParticleServer::Instance()->GetParticleEmitter(emitterKey);
        n_assert(0 != emitter);
        emitter->Trigger(curTime);
        emitter->Render(curTime);

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nParticleShapeNode::Apply(nSceneGraph* sceneGraph)
{
    return nGeometryNode::Apply(sceneGraph);
}

//------------------------------------------------------------------------------
/**
    Render
*/
bool
nParticleShapeNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if( sceneGraph->GetCurrentPassIndex() == trfmPassIndex )
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
    Returns the current emitter
*/
nParticleEmitter*
nParticleShapeNode::GetEmitter(nEntityObject* entityObject)
{
    // get emitter from render context
    ncScene* renderContext = entityObject->GetComponentSafe<ncScene>();
    nVariable& varEmitter = renderContext->GetLocalVar(this->emitterVarIndex);
    int emitterKey = varEmitter.GetInt();
    return this->refParticleServer->GetParticleEmitter(emitterKey);
}

#ifndef NGAME

//------------------------------------------------------------------------------
/**
    Returns the path of diffMap, facility property grid
*/
nString
nParticleShapeNode::GetParticleTexture()
{
    return nFileServer2::Instance()->ManglePath( this->GetTexture(nShaderState::diffMap) );  
}

//------------------------------------------------------------------------------
/**
    set the path of diffMap, facility property grid
*/
void 
nParticleShapeNode::SetParticleTexture(const char* path)
{
    nString newPath(path);
    if (!nFileServer2::Instance()->UnManglePath( newPath , "home") )
    {
        nFileServer2::Instance()->UnManglePath( newPath , "wctextures");
    }

    this->SetTexture( nShaderState::diffMap, newPath.Get() );
}

//------------------------------------------------------------------------------
/**
    Returns the path of mesh
*/
nString
nParticleShapeNode::GetParticleMesh()
{
    return nFileServer2::Instance()->ManglePath( this->GetMesh() );  
}

//------------------------------------------------------------------------------
/**
    set the path of mesh
    Each vertex of mesh is a spawn point
*/
void 
nParticleShapeNode::SetParticleMesh(const char* path)
{
    nString newPath(path);
    if ( newPath.CheckExtension( "nvx2") || newPath.CheckExtension("n3d2") )
    {
        if (!nFileServer2::Instance()->UnManglePath( newPath , "home") )
        {
            nFileServer2::Instance()->UnManglePath( newPath , "wc");
        }
        this->SetMesh( newPath.Get() );
    }
}

//------------------------------------------------------------------------------
/**
*/
int 
nParticleShapeNode::GetFuncSrcBlend()
{
    return this->GetInt( nShaderState::funcSrcBlend);
}

//------------------------------------------------------------------------------
/**
*/
void 
nParticleShapeNode::SetFuncSrcBlend( int function)
{
    this->SetInt( nShaderState::funcSrcBlend , function);
}

//------------------------------------------------------------------------------
/**
*/
int
nParticleShapeNode::GetFuncDestBlend()
{
    return this->GetInt( nShaderState::funcDestBlend);
}

//------------------------------------------------------------------------------
/**
*/
void 
nParticleShapeNode:: SetFuncDestBlend( int function )
{
    this->SetInt( nShaderState::funcDestBlend , function );
}

#endif
