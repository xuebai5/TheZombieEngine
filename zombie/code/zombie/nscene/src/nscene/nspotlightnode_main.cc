#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nspotlightnode_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nspotlightnode.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "nscene/nscenegraph.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nlight.h"

nNebulaScriptClass(nSpotLightNode, "nlightnode");

//------------------------------------------------------------------------------
/**
*/
nSpotLightNode::nSpotLightNode() :
    camera(45.0f, 1.0f, 0.1f, 10.0f)
{
    this->SetType(nLight::Spot);
    // Use orthogonal camera because the shader only implements this type.
    // put a perfect cube, with arist dimension 1.0
    // If you need and diferente size then reescale the entity.
    this->camera.SetOrthogonal(1.f, 1.f, 0.f, 0.5f);
}

//------------------------------------------------------------------------------
/**
*/
nSpotLightNode::~nSpotLightNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Define the cone parameters. Since the spot light essentially defines
    a projection transformation, an nCamera2 object is used for the cone
    definition.

    @param  c   an nCamera2 object defining the cone
*/
void
nSpotLightNode::SetCamera(const nCamera2& c)
{
    this->camera = c;
}

//------------------------------------------------------------------------------
/**
    Returns the cone definition.

    @return     an nCamera2 object holding the cone definition
*/
const nCamera2&
nSpotLightNode::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
    Set perspective projection parameters.

    @param  aov     angle of view in degree
    @param  aspect  aspect ration
    @param  nearp   near plane distance
    @param  farp    far plane distance
*/
void
nSpotLightNode::SetPerspective(float aov, float aspect, float nearp, float farp)
{
    this->camera.SetPerspective(aov, aspect, nearp, farp);
}

//------------------------------------------------------------------------------
/**
    Set orthogonal projection parameters.

    @param  w       view volume width
    @param  h       view volume height
    @param  nearp   near plane distance
    @param  farp    far plane distance
*/
void
nSpotLightNode::SetOrthogonal(float w, float h, float nearp, float farp)
{
    this->camera.SetOrthogonal(w, h, nearp, farp);
}

//------------------------------------------------------------------------------
/**
    "Render" the spot light node.
    
    @param  sceneGraph          pointer to the scene server object rendering this node
    @param  renderContext       pointer to the render context for this node
*/
bool
nSpotLightNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
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
        //gfxServer->SetTransform(nGfxServer2::LightProjection, this->camera.GetProjection());
        //matrix44 modelLightProjection = gfxServer->GetTransform(nGfxServer2::ModelLightProjection);
        
        matrix44 invLightTransform = sceneGraph->GetModelTransform();
        invLightTransform.invert_simple();
        const matrix44& model = gfxServer->GetTransform(nGfxServer2::Model);
        const matrix44& projection = this->camera.GetProjection();
        matrix44 modelLight = model * invLightTransform;
        matrix44 modelLightProjection = modelLight * projection;

        /// @todo: remove this code and use always orthogonal camera
        /// if you would the other projection then create other nsceneNode because it need other shader.
        if ( this->camera.GetType() == nCamera2::Orthogonal)
        {
            shader->SetMatrix(nShaderState::ModelLightProjection, modelLightProjection );
        } else
        {
            //Perspective projection,
            shader->SetMatrix(nShaderState::ModelLightProjection, modelLightProjection * scaleAndBiasTextureProjection);
        }
    }
    
    return true;
}
