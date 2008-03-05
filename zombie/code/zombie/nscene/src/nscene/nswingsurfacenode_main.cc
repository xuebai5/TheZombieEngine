#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nswingsurfacenode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nswingsurfacenode.h"
#include "variable/nvariableserver.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "zombieentity/ncdictionary.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nSwingSurfaceNode, "nsurfacenode");

//------------------------------------------------------------------------------
/**
*/
nSwingSurfaceNode::nSwingSurfaceNode() :
    timeVarHandle(nVariable::InvalidHandle),
    windVarHandle(nVariable::InvalidHandle),
    swingAngle(45.0f),
    swingTime(5.0f)
{
//    SetMeshUsage( nMesh2::WriteOnce | nMesh2::NeedsVertexShader );
}

//------------------------------------------------------------------------------
/**
*/
nSwingSurfaceNode::~nSwingSurfaceNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This validates the variable handles for time and wind.
*/
bool
nSwingSurfaceNode::LoadResources()
{
    if (nSurfaceNode::LoadResources())
    {
        this->timeVarHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
        this->windVarHandle = nVariableServer::Instance()->GetVariableHandleByName("wind");
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    This invalidates the variable handles for time and wind.
*/
void
nSwingSurfaceNode::UnloadResources()
{
    this->timeVarHandle = nVariable::InvalidHandle;
    this->windVarHandle = nVariable::InvalidHandle;
    nSurfaceNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Permute the provided static angle by time and world space position. Lots
    of constants here, but the result is quite reasonable for a swinging tree.
    The swinging geometry should not move around in world space, as that
    would break the position offset and lead to stuttering in the
    geometry.

    @param  pos             position in world space
    @param  time            current time
*/
float
nSwingSurfaceNode::ComputeAngle(const vector3& pos, nTime time) const
{
    // add position offset to time to prevent that all trees swing in sync
    time += pos.x + pos.y + pos.z;

    // sinus wave swing value (between +1 and -1)
    float swing = (float) n_sin((time * n_deg2rad(360.0f)) / this->swingTime);

    // get a wind strength "swinging" angle, we want no swinging at
    // min and max wind strength, and max swinging at 0.5 wind strength
    return this->swingAngle * 0.3f + (this->swingAngle * swing * 0.7f);
}

//------------------------------------------------------------------------------
/**
    This computes the Swing rotation matrix and bounding box parameters needed by
    the shaders which implement swinging geometry.
*/
bool
nSwingSurfaceNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    // ... render this at the surface stage, or else render the geometry one as usual
    if (nSurfaceNode::Render(sceneGraph, entityObject))
    //if( this->refSurfaceNode->Render(sceneGraph, renderContext) )
    {
        ncScene *renderContext = entityObject->GetComponent<ncScene>();
        ncDictionary *varContext = entityObject->GetComponent<ncDictionary>();

        // get current wind dir and wind strength
        //nVariable* timeVar = renderContext.GetVariable(this->timeVarHandle);
        //nVariable* windVar = renderContext.GetVariable(this->windVarHandle);
        //n_assert(timeVar && windVar);
        //nTime time = (nTime) timeVar->GetFloat();

        nTime time = renderContext->GetTime();
        nVariable* windVar = varContext->GetVariable(this->windVarHandle);
        n_assert(windVar);
        const nFloat4& wind = windVar->GetFloat4();

        // build horizontal wind vector
        vector3 windVec(wind.x, wind.y, wind.z);

        // get current position in world space
        const matrix44& model = nGfxServer2::Instance()->GetTransform(nGfxServer2::Model);

        // implement swinging by permuting angle by time and position
        float permutedAngle = this->ComputeAngle(model.pos_component(), time);

        // build a rotation matrix from the permuted angle
        static const vector3 up(0.0f, 1.0f, 0.0f);
        matrix44 rotMatrix;
        rotMatrix.rotate(windVec * up, permutedAngle);

        // set shader parameter
        nShader2* shader = nGfxServer2::Instance()->GetShader();
        n_assert(shader);
        if (shader->IsParameterUsed(nShaderState::Swing))
        {
            shader->SetMatrix(nShaderState::Swing, rotMatrix);
        }

        // set wind shader parameter
        if (shader->IsParameterUsed(nShaderState::Wind))
        {
            shader->SetFloat4(nShaderState::Wind, windVar->GetFloat4());
        }

        return true;
    }
    return false;
}
