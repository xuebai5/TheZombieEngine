#include "precompiled/pchnmaterial.h"
//------------------------------------------------------------------------------
//  nmaterialnode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nmaterial/nmaterialnode.h"
#include "nmaterial/nmaterialserver.h"
#include "nscene/nscenegraph.h"
#include "nscene/nsceneshader.h"
#include "nscene/nshadertree.h"
#include "nscene/nlightnode.h"
#include "nscene/nrenderpathnode.h"
#include "nscene/ncscenelight.h"
#include "nscene/ncsceneclass.h"
#include "kernel/nlogclass.h"

nNebulaScriptClass(nMaterialNode, "nsurfacenode");

//------------------------------------------------------------------------------
/**
*/
nMaterialNode::nMaterialNode() :
    lghtPassIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterialNode::~nMaterialNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Get all material shaders from the material for all passes.
    Assign them as the shader for each pass, and then load all shaders
    through nSurfaceNode.
*/
bool
nMaterialNode::LoadResources()
{
    bool success = false;

    kernelServer->PushCwd(this);

    if (this->refMaterial.isvalid())
    {
        if (this->refMaterial->LoadShaders())
        {
            success = nSurfaceNode::LoadResources();

            // get loaded shaders from the material and get them a shader index
            // loop over all shaders in the material
            int numLevels = this->refMaterial->GetNumLevels();

            // initialize the array of shader trees
            this->shaderTreeArray.SetFixedSize( numLevels );

            int level;
            for (level = 0; level < numLevels; ++level)
            {
                int pass;
                for (pass = 0; pass < this->refMaterial->GetNumPasses(level); ++pass)
                {
                    nFourCC fourcc = this->refMaterial->GetPassAt(level, pass);
                    nShaderTree* shaderTree = this->refMaterial->GetShaderTree(level, fourcc);
                    int i;
                    for (i = 0; i < shaderTree->GetNumNodes(); ++i)
                    {
                        // @todo ma.garcias temporarily handle the case of invalid shaders
                        if (shaderTree->GetShaderAt(i))
                        {
                            nSceneServer* sceneServer = nSceneServer::Instance();
                            int shaderIndex = sceneServer->FindShader(shaderTree->GetShaderNameAt(i), shaderTree->GetTechniqueAt(i));
                            //int shaderIndex = sceneServer->FindShader(shaderTree->GetShaderAt(i), shaderTree->GetTechniqueAt(i));
                            if (shaderIndex == -1)
                            {
                                nSceneShader newSceneShader;
                                newSceneShader.SetShaderName(shaderTree->GetShaderNameAt(i));//name
                                newSceneShader.SetShader(shaderTree->GetShaderNameAt(i));//filename
                                newSceneShader.SetTechnique(shaderTree->GetTechniqueAt(i));//technique name
                                newSceneShader.SetSequence(shaderTree->GetSequenceAt(i));//sequence
                                //newSceneShader.SetShaderObject(shaderTree->GetShaderAt(i));//shader pointer
                                
                                shaderIndex = nSceneServer::Instance()->AddShader(newSceneShader);
                                n_assert(shaderIndex != -1);
                            }

                            shaderTree->SetShaderIndexAt(i, shaderIndex);
                        }
                    }

                    // keep an array of shader trees by level
                    int passIndex = nSceneServer::Instance()->GetPassIndexByFourCC( fourcc );
                    this->shaderTreeArray[level].Set( passIndex, shaderTree );

                    // TODO- cache selectors to optimize access to material
                }
            }
        }
    }

    kernelServer->PopCwd();

    return success;
}

//------------------------------------------------------------------------------
/**
    Unload all shaders from the material.
*/
void
nMaterialNode::UnloadResources()
{
    /// @todo ma.garcias - unload here scene shaders instead
    if (this->refMaterial.isvalid())
    {
        //this->refMaterial->UnloadShaders();
    }
    nSurfaceNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Return number of passes.
*/
int
nMaterialNode::GetNumLevels()
{
    n_assert_return(this->refMaterial.isvalid(), 0);
    return this->refMaterial->GetNumLevels();
}

//------------------------------------------------------------------------------
/**
    Return number of passes.
*/
int
nMaterialNode::GetNumLevelPasses(int level)
{
    n_assert_return(this->refMaterial.isvalid(), 0);
    n_assert_return(level < this->refMaterial->GetNumLevels(), 0);
    return this->refMaterial->GetNumPasses(level);
}

//------------------------------------------------------------------------------
/**
    Return pass for a level and pass
*/
int
nMaterialNode::GetLevelPassIndex(int level, int pass)
{
    n_assert_return(this->refMaterial.isvalid(), -1);
    n_assert_return(level < this->refMaterial->GetNumLevels(), -1);
    n_assert_return(pass < this->refMaterial->GetNumPasses(level), -1);
    nFourCC fourcc = this->refMaterial->GetPassAt(level, pass);
    return nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
}

//------------------------------------------------------------------------------
/**
    Return pass for a level and pass
*/
nShaderTree*
nMaterialNode::GetShaderTree(int level, int passIndex)
{
    n_assert_return(level < this->shaderTreeArray.Size(), 0);
    n_assert_return(passIndex < this->shaderTreeArray[level].Size(), 0);
    int levelIndex = n_min(level, this->shaderTreeArray.Size() - 1);
    return this->shaderTreeArray[levelIndex].At( passIndex );
}

//------------------------------------------------------------------------------
/**
    OBSOLETE- Return number of passes.
*/
int
nMaterialNode::GetNumPasses(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(this->refMaterial.isvalid());
    int materialLevel = entityObject->GetComponent<ncScene>()->GetMaxMaterialLevel();
    materialLevel = max(sceneGraph->GetMaxMaterialLevel(), materialLevel);
    return this->refMaterial->GetNumPasses(materialLevel);
}

//------------------------------------------------------------------------------
/**
    OBSOLETE- Get pass by index.
*/
nFourCC
nMaterialNode::GetPassAt(int index, nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(this->refMaterial.isvalid());
    int materialLevel = entityObject->GetComponent<ncScene>()->GetMaxMaterialLevel();
    materialLevel = max(sceneGraph->GetMaxMaterialLevel(), materialLevel);
    return this->refMaterial->GetPassAt(materialLevel, index);
}

//------------------------------------------------------------------------------
/**
    OBSOLETE- Get pass index by index.
*/
int
nMaterialNode::GetPassIndexAt(int index, nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(this->refMaterial.isvalid());
    int materialLevel = entityObject->GetComponent<ncScene>()->GetMaxMaterialLevel();
    materialLevel = max(sceneGraph->GetMaxMaterialLevel(), materialLevel);
    nFourCC fourcc = this->refMaterial->GetPassAt(materialLevel, index);
    return nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
}

//------------------------------------------------------------------------------
/**
*/
bool
nMaterialNode::IsTextureUsed(nShaderState::Param /*param*/)
{
#if 0 // disable check for texture usage
	/// @todo check all shaders if anywhere the texture specified by param is used
    /// this is called after shaders are loaded, so it's safe to ask the material
    /// shader whether the parameters is used or not.
    if (this->refMaterial.isvalid())
    {
        int numLevels = this->refMaterial->GetNumLevels();
        for (int level = 0; level < numLevels; ++level)
        {
            int numPasses = this->refMaterial->GetNumPasses(level);
            for (int i = 0; i < numPasses; i++)
            {
                nFourCC fourcc = this->refMaterial->GetPassAt(level, i);
                nMaterialTree *tree;
                tree = (nMaterialTree *) this->refMaterial->GetShaderTree(level, fourcc);
                
                /// @todo check every shader in the nMaterialTree
                /// IsParameterUsed should only be for the set of param keys.
                if (tree->IsParameterUsed(param))
                {
                    return true;
                }
            }
        }
    }
    return false;
#else
    return true;
#endif
}
