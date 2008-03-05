#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nsurfacenode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nsurfacenode.h"
#include "nscene/nsceneshader.h"
#include "nscene/nscenegraph.h"
#include "nscene/nshadertree.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nshader2.h"
#include "gfx2/ntexture2.h"
#include "kernel/ntimeserver.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "kernel/ndebug.h"
#include "nscene/nanimator.h"

nNebulaScriptClass(nSurfaceNode, "nabstractshadernode");

//------------------------------------------------------------------------------
/**
*/
nSurfaceNode::nSurfaceNode() :
    shaderArray(4, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSurfaceNode::~nSurfaceNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Unload all shaders.
*/
void
nSurfaceNode::UnloadShaders()
{
    int i;
    for (i = 0; i < this->shaderArray.Size(); i++)
    {
        if (this->shaderArray[i].IsShaderValid())
        {
            this->shaderArray[i].GetShader()->Release();
            this->shaderArray[i].Invalidate();
        }
    }

    for (i = 0; i < this->shaderTreeArray.Size(); ++i)
    {
        if (this->shaderTreeArray[i].isvalid())
        {
            this->shaderTreeArray[i]->Release();
            this->shaderTreeArray[i].invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Load shader resources.
*/
bool
nSurfaceNode::LoadShaders()
{
    // load shaders
    int i;
    for (i = 0; i < this->shaderArray.Size(); i++)
    {
        ShaderEntry& shaderEntry = this->shaderArray[i];
        if (!shaderEntry.IsShaderValid() && shaderEntry.GetName())
        {
            #if 0 // keep this commented while the shader database is not used
            // try to get shader by name from shader repository
            int shaderIndex = nSceneServer::Instance()->FindShader(shaderEntry.GetName());
            if (shaderIndex != -1)
            {
                nSceneShader& sceneShader = nSceneServer::Instance()->GetShaderAt(shaderIndex);
                if (!sceneShader.IsValid())
                {
                    sceneShader.Validate();
                    n_assert(sceneShader.IsValid());
                }
                sceneShader.GetShaderObject()->AddRef();
                shaderEntry.SetShaderIndex(shaderIndex);
                shaderEntry.SetShader(sceneShader.GetShaderObject());
            }
            else
            #endif
            {
                // create a new empty shader object
                nShader2* shd = nGfxServer2::Instance()->NewShader(shaderEntry.GetName());
                n_assert(shd);
                if (!shd->IsLoaded())
                {
                    // load shader resource file
                    shd->SetFilename(shaderEntry.GetName());
                }

                if (shd)
                {
                    // register shader object in scene shader database
                    nSceneShader sceneShader;
                    sceneShader.SetShader(shaderEntry.GetName());// filename
                    sceneShader.SetShaderName(shaderEntry.GetName());// shader name
                    int shaderIndex = nSceneServer::Instance()->FindShader(shaderEntry.GetName());
                    if (shaderIndex == -1)
                    {
                        shaderIndex = nSceneServer::Instance()->AddShader(sceneShader);
                        n_assert(shaderIndex != -1);
                    }

                    // set shader object and index for local entry
                    shaderEntry.SetShader(shd);
                    shaderEntry.shaderIndex = shaderIndex;

                    // create a degenerate decision tree for use in the render path
                    nShaderTree* shaderTree = static_cast<nShaderTree*>( kernelServer->New("nshadertree") );
                    n_assert( shaderTree );
                    shaderTree->BeginNode( nVariable::InvalidHandle, 0 );
                    shaderTree->SetShaderObject( shd );
                    shaderTree->SetShaderIndexAt( 0, shaderIndex );
                    shaderTree->EndNode();

                    this->shaderTreeArray.Set( shaderEntry.GetPassIndex(), shaderTree );
                }
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nSurfaceNode::LoadResources()
{
    if (this->LoadShaders())
    {
        if (nAbstractShaderNode::LoadResources())
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nSurfaceNode::UnloadResources()
{
    nAbstractShaderNode::UnloadResources();
    this->UnloadShaders();
}

//------------------------------------------------------------------------------
/**
    Find shader object associated with fourcc code.
*/
nSurfaceNode::ShaderEntry*
nSurfaceNode::FindShaderEntry(nFourCC fourcc) const
{
    int i;
    int numShaders = this->shaderArray.Size();
    for (i = 0; i < numShaders; i++)
    {
        ShaderEntry& shaderEntry = this->shaderArray[i];
        if (shaderEntry.GetFourCC() == fourcc)
        {
            return &shaderEntry;
        }
    }
    // fallthrough: no loaded shader matches this fourcc code
    return 0;
}

//------------------------------------------------------------------------------
/**
    Return number of levels.
*/
int
nSurfaceNode::GetNumLevels()
{
    return (this->shaderArray.Size() > 0) ? 1 : 0;
}

//------------------------------------------------------------------------------
/**
    Return number of passes for a level
*/
int
nSurfaceNode::GetNumLevelPasses(int /*level*/)
{
    return this->shaderArray.Size();
}

//------------------------------------------------------------------------------
/**
    Return number of passes for a level
*/
int
nSurfaceNode::GetLevelPassIndex(int level, int pass)
{
    n_assert_return((level == 0) && (pass < this->shaderArray.Size()), -1);
    return this->shaderArray[pass].GetPassIndex();
}

//------------------------------------------------------------------------------
/**
*/
nShaderTree*
nSurfaceNode::GetShaderTree(int /*level*/, int passIndex)
{
    n_assert(passIndex < this->shaderTreeArray.Size());
    return this->shaderTreeArray[passIndex];
}

//------------------------------------------------------------------------------
/**
*/
void
nSurfaceNode::SetShader(nFourCC fourcc, const char* name)
{
    n_assert(name);
    ShaderEntry* shaderEntry = this->FindShaderEntry(fourcc);
    if (shaderEntry)
    {
        shaderEntry->Invalidate();
        shaderEntry->SetName(name);
    }
    else
    {
        ShaderEntry newShaderEntry(fourcc, name);
        this->shaderArray.Append(newShaderEntry);
    }
}

//------------------------------------------------------------------------------
/**
*/
const char*
nSurfaceNode::GetShader(nFourCC fourcc) const
{
    ShaderEntry* shaderEntry = this->FindShaderEntry(fourcc);
    if (shaderEntry)
    {
        return shaderEntry->GetName();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nSurfaceNode::IsTextureUsed(nShaderState::Param /*param*/)
{
#if 0
    // check in all shaders if anywhere the texture specified by param is used
    int i;
    int numShaders = this->shaderArray.Size();
    for (i = 0; i < numShaders; i++)
    {
        const ShaderEntry& shaderEntry = this->shaderArray[i];
        
        // first be sure that the shader entry could be loaded
        if (shaderEntry.IsShaderValid())
        {
            nShader2* shader = shaderEntry.GetShader();
            if (shader->IsParameterUsed(param))
            {
                return true;
            }
        }
    }
    // fallthrough: texture not used by any shader
    return false;
#else
    return true;
#endif
}

//------------------------------------------------------------------------------
/**
    Setup shader attributes before rendering instances of this scene node.
    FIXME
*/
bool
nSurfaceNode::Apply(nSceneGraph* sceneGraph)
{
    n_assert(sceneGraph);
    int shaderIndex = sceneGraph->GetShaderIndex();
    if (shaderIndex != -1)
    {
        nSceneShader& sceneShader = nSceneServer::Instance()->GetShaderAt(shaderIndex);
        nGfxServer2::Instance()->SetShader(sceneShader.GetShaderObject());
        nAbstractShaderNode::Apply(sceneGraph);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Update shader and set as current shader in the gfx server.

    - 15-Jan-04     floh    AreResourcesValid()/LoadResources() moved to scene server
*/
bool
nSurfaceNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);
    
    //nShader2* shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();
    
    // invoke shader manipulators
    this->InvokeAnimators(entityObject);
/*    
    //nGfxServer2* gfxServer = nGfxServer2::Instance();
    // set texture transforms (that can be animated)
    //n_assert(nGfxServer2::MaxTextureStages >= 4);
    static matrix44 m;
    
    this->textureTransform[0].getmatrix44(m);
    gfxServer->SetTransform(nGfxServer2::Texture0, m);
    
    this->textureTransform[1].getmatrix44(m);
    gfxServer->SetTransform(nGfxServer2::Texture1, m);
    */
    // transfer the rest of per-instance (animated, overriden) parameters
    // per instance-set parameters are handled at Apply()
    // also, shader overrides are handled at nGeometryNode::Render()
    nAbstractShaderNode::Render(sceneGraph, entityObject);
    
    return true;
}
