#include "precompiled/pchrnsscene.h"
//------------------------------------------------------------------------------
//  ndotlasernode_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ndotlaser/ndotlasernode.h"
#include "nscene/nscenegraph.h"
#include "nscene/ntransformnode.h"
#include "nphysics/nphycollide.h"
#include "nphysics/nphysicsserver.h"
#include "entity/nentityobject.h"
#include "nphysics/ncphysicsobj.h"
#include "nspatial/ncspatial.h"
#include "nphysics/nphysicsgeom.h"
#include "nscene/ncscene.h"
#include "gfx2/ncamera2.h"
#include "ndotlaser/ncscenedotlaser.h"

nNebulaScriptClass(nDotLaserNode , "nlightnode");


//------------------------------------------------------------------------------
/**
*/
nDotLaserNode::nDotLaserNode()
{
    this->SetType( nLight::Omni );
}

//------------------------------------------------------------------------------
/**
*/
nDotLaserNode::~nDotLaserNode()
{
}

//------------------------------------------------------------------------------
/**
    attach this to transform pass
*/
void
nDotLaserNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    nLightNode::Attach(sceneGraph, entityObject );
}


//------------------------------------------------------------------------------
/*
*/
bool
nDotLaserNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (!nLightNode::Render(sceneGraph, entityObject))
    {
        return false;
    }
    
    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    nShader2* shader = gfxServer->GetShader();
    n_assert(shader);
    
    if (shader->IsParameterUsed(nShaderState::ModelLightProjection))
    {
        matrix44 invLightTransform = sceneGraph->GetModelTransform();
        invLightTransform.invert_simple();
        const matrix44& model = gfxServer->GetTransform(nGfxServer2::Model);
        const matrix44& lightProjection = entityObject->GetComponentSafe<ncSceneDotLaser>()->GetLightProjection();
        shader->SetMatrix(nShaderState::ModelLightProjection,  model* lightProjection * scaleAndBiasTextureProjection);
    }
    return true;

}

//------------------------------------------------------------------------------
/*
*/
bool
nDotLaserNode::Apply(nSceneGraph* sceneGraph)
{
    return nLightNode::Apply(sceneGraph);
}
