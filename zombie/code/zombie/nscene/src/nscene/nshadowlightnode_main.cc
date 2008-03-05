#include "precompiled/pchrnsscene.h"
//------------------------------------------------------------------------------
//  nshadowlightnode_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nshadowlightnode.h"

nNebulaScriptClass(nShadowLightNode, "nabstractshadernode");

//------------------------------------------------------------------------------
/**
*/
nShadowLightNode::nShadowLightNode() :
    camera(45.0f, 1.0f, 0.1f, 10.0f),
    dirtySProj(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowLightNode::~nShadowLightNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowLightNode::SetCamera(const nCamera2& c)
{
    this->camera = c;
}

//------------------------------------------------------------------------------
/**
*/
const nCamera2&
nShadowLightNode::GetCamera() const
{
    return this->camera;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowLightNode::SetOrthogonal(float w, float h, float nearp, float farp)
{
    this->orthogonal = vector4(w,h,nearp,farp);
    this->camera.SetOrthogonal(w, h, nearp, farp);
    this->dirtySProj = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowLightNode::SetEuler( const vector3& eulerAngles )
{
    this->euler = eulerAngles;
    this->dirtySProj = true;
}

//------------------------------------------------------------------------------
/**
*/
const vector3 
nShadowLightNode::GetEuler() const
{
    return this->euler;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowLightNode::SetPosition( const vector3& pos )
{
    this->position = pos;
    this->dirtySProj = true;
}

//------------------------------------------------------------------------------
/**
*/
const vector3 
nShadowLightNode::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowLightNode::SetDeformation( const vector4& deformation )
{
    this->deformation = deformation;
    this->dirtySProj = true;
}

//------------------------------------------------------------------------------
/**
*/
const vector4
nShadowLightNode::GetDeformation() const
{
    return this->deformation;
}

//------------------------------------------------------------------------------
/**
*/
void
nShadowLightNode::ComputeShadowProjection()
{
    this->shadowProj.ident();
    this->shadowProj.rotate_x(this->euler.x);    
    this->shadowProj.rotate_y(this->euler.y);
    this->shadowProj.rotate_z(this->euler.z);
    this->shadowProj.set_translation( this->position );
    this->shadowProj.invert_simple();

    matrix44 deformationMatrix( this->deformation.x, this->deformation.y, 0.f, 0.f,
                                this->deformation.z, this->deformation.w, 0.f, 0.f,
                                                0.f,                 0.f, 1.f, 0.f,
                                                0.f,                 0.f, 0.f, 1.f );

    this->shadowProj = this->shadowProj * deformationMatrix* this->camera.GetProjection();
    this->scaledShadowProj = this->shadowProj;//* scaleAndBias;

    this->dirtySProj = false;
}

//------------------------------------------------------------------------------
/**
*/
const matrix44 &
nShadowLightNode::GetShadowProjection()
{
    if( this->dirtySProj )
    {
        this->ComputeShadowProjection();
    }

    return this->scaledShadowProj;
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowLightNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    //@TODO: use the deformationMatrix

    //set node own parameters
    if (!nAbstractShaderNode::Apply(sceneGraph))
    {
        return false;
    }

    //set node overrides (textures passed from the lightenv)
    if (!nAbstractShaderNode::Render(sceneGraph, entityObject))
    {
        return false;
    }

    nGfxServer2 *gfxServer = nGfxServer2::Instance();
    nShader2* shader = gfxServer->GetShader();
    n_assert(shader);
    
    if (shader->IsParameterUsed(nShaderState::ModelShadowProjection))
    {
        //matrix44 invLightTransform = sceneGraph->GetModelTransform();
        //invLightTransform.invert_simple();

        const matrix44& model = gfxServer->GetTransform(nGfxServer2::Model);
        matrix44 modelLight = model /** invLightTransform*/;
        matrix44 modelShadowProjection = modelLight * this->shadowProj;
        shader->SetMatrix(nShaderState::ModelShadowProjection, modelShadowProjection * scaleAndBiasTextureProjection);
    }
    
    return true;
}
