#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nlightnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nlightnode.h"
#include "nscene/ncscenelight.h"
#include "nscene/nscenegraph.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nLightNode, "nabstractshadernode");

//------------------------------------------------------------------------------
/**
*/
nLightNode::nLightNode() :
    lghtPassIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLightNode::~nLightNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    copy local parameters into entity shader overrides-
*/
void
nLightNode::EntityCreated(nEntityObject* entityObject)
{
    nShaderParams& shaderParams = entityObject->GetComponentSafe<ncScene>()->GetShaderOverrides();
    
    int i;
    int numValidParams = this->shaderParams.GetNumValidParams();
    for (i = 0; i < numValidParams; i++)
    {
        nShaderState::Param curParam = this->shaderParams.GetParamByIndex(i);
        if (!shaderParams.IsParameterValid(curParam))
        {
            shaderParams.SetArg(curParam, this->shaderParams.GetArgByIndex(i));
        }
    }

    //clear all shader overrides that can be overriden from local vars as well
    int numVarParams = this->varParamArray.Size();
    for (i = 0; i < numVarParams; i++)
    {
        nShaderState::Param curParam = this->varParamArray[i].param;
        if (shaderParams.IsParameterValid(curParam))
        {
            shaderParams.ClearArg(curParam);
        }
    }

    //for all selector types, get their priority from the scene server
    if (this->selectorType)
    {
        this->renderPri = nSceneServer::Instance()->GetPriorityBySelectorType( this->selectorType );
        ncSceneLight* sceneLight = entityObject->GetComponent<ncSceneLight>();
        if (sceneLight && sceneLight->GetLightPriority() < this->renderPri)
        {
            sceneLight->SetLightPriority(this->renderPri);
        }
    }

    nAbstractShaderNode::EntityCreated(entityObject);
}

//------------------------------------------------------------------------------
/**
    Attach to the scene graph.
*/
void
nLightNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (this->lghtPassIndex == -1)
    {
        this->lghtPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('lght'));
    }
    sceneGraph->AddGroup(this->lghtPassIndex, this, entityObject);

    // cache material selectors for fast shader resolution
    ncSceneLight* sceneLight = entityObject->GetComponent<ncSceneLight>();
    if (sceneLight)
    {
        sceneLight->AddLightType(this->GetSelectorType());
    }

    nSceneNode::Attach(sceneGraph, entityObject);
}

//------------------------------------------------------------------------------
/**
    This sets the light state which is constant across all shape
    instances which are lit by this light.
    TODO try to distinguish Apply() and Render() as in open source
*/
bool
nLightNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    
    // apply pre-instance shader parameters
    // OPTIMIZATION- copied into the entity at EntityCreated
    //nAbstractShaderNode::Apply(sceneGraph);

    // invoke animators
    //this->InvokeAnimators(entityObject);

    // get the current shader object from the gfx server
    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    nShader2* shader = gfxServer->GetShader();
    n_assert(shader);
    
    // apply per-instance, animated shader parameters
    if (nAbstractShaderNode::Render(sceneGraph, entityObject))
    {
        // depending on the type of light, compute the parameters from the light transform:
        const matrix44& lightTransform = sceneGraph->GetModelTransform();

        switch (this->GetType())
        {
        case nLight::Directional:
            {
                if (shader->IsParameterUsed(nShaderState::ModelLightPos))
                {
                    matrix44 modelLight = lightTransform * gfxServer->GetTransform(nGfxServer2::InvModel);
                    vector3 modelLightDir = modelLight.z_component();
                    modelLightDir.norm();
                    shader->SetVector3(nShaderState::ModelLightPos, modelLightDir);
                }
                if (shader->IsParameterUsed(nShaderState::LightPos))
                {
                    vector3 lightDir = lightTransform.z_component();
                    lightDir.norm();
                    shader->SetVector3(nShaderState::LightPos, lightDir);
                }
            }
            break;
            
        case nLight::Point:
        case nLight::Spot:
            {
                const vector3& lightPos = lightTransform.pos_component();
                if (shader->IsParameterUsed(nShaderState::LightPos))
                {
                    shader->SetVector3(nShaderState::LightPos, lightPos);
                }
                if (shader->IsParameterUsed(nShaderState::ModelLightPos))
                {
                    vector3 modelLightPos = gfxServer->GetTransform(nGfxServer2::InvModel) * lightPos;
                    shader->SetVector3(nShaderState::ModelLightPos, modelLightPos);
                }   
            }
            break;
        }

        // finally, apply render context overrides
        ncScene *renderContext = entityObject->GetComponent<ncScene>();
        shader->SetParams(renderContext->GetShaderOverrides());

        return true;
    }

    return false;
}
