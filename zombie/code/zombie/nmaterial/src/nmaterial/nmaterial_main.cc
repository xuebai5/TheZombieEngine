#include "precompiled/pchnmaterial.h"
//--------------------------------------------------
//  nmaterial_main.cc
//  (C) 2004 Conjurer Services, S.A.
//--------------------------------------------------
#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialserver.h"
#include "nscene/nshadertree.h"

nNebulaScriptClass(nMaterial, "nroot");

//------------------------------------------------------------------------------
/**
*/
nMaterial::nMaterial() :
    inBeginLevel(false),
    currentLevel(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nMaterial::~nMaterial()
{
    // release shader tree for each level and pass
    for (int level = 0; level < this->levelPasses.Size(); ++level)
    {
        nArray<Pass>& passes = this->levelPasses[level];
        for (int index = 0; index < passes.Size(); ++index)
        {
            Pass& pass = passes[index];
            if (pass.refShaderTree.isvalid())
            {
                pass.refShaderTree->Release();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Load all associated shaders through the material server.
*/
bool
nMaterial::LoadShaders()
{
    if (this->levelPasses.Size() == 0)
    {
        // build and fill the material shader
        nMaterialServer::Instance()->LoadMaterial(this);
    }
    
    // load shaders
    int level;
    for (level = 0; level < this->levelPasses.Size(); level++)
    {
        nArray<Pass>& passes = this->levelPasses[level];
        for (int i = 0; i < passes.Size(); i++)
        {
            n_assert(passes[i].refShaderTree.isvalid());
            if (!passes[i].refShaderTree->LoadResource())
            {
                return false;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Load all associated shaders through the material server.
*/
void
nMaterial::UnloadShaders()
{
    int level;
    for (level = 0; level < this->levelPasses.Size(); level++)
    {
        nArray<Pass>& passes = this->levelPasses[level];
        for (int i = 0; i < passes.Size(); i++)
        {
            Pass& pass = passes[i];
            if (pass.refShaderTree.isvalid())
            {
                pass.refShaderTree->UnloadResource();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Load all associated shaders through the material server.
*/
void
nMaterial::BeginLevel()
{
    n_assert(!this->inBeginLevel);
    this->inBeginLevel = true;
    this->levelPasses.Append(nArray<Pass>(4, 4));
    this->currentLevel = this->levelPasses.Size() - 1;
}

//------------------------------------------------------------------------------
/**
    Load all associated shaders through the material server.
*/
void
nMaterial::EndLevel()
{
    n_assert(this->inBeginLevel);
    this->inBeginLevel = false;
    this->currentLevel = 0;
}

//------------------------------------------------------------------------------
/**
    Load all associated shaders through the material server.
*/
const int
nMaterial::GetCurrentLevel()
{
    return this->currentLevel;
}

//------------------------------------------------------------------------------
/**
    Load all associated shaders through the material server.
*/
nObject*
nMaterial::BeginPass(nFourCC fourcc)
{
    n_assert(this->inBeginLevel);
    nShaderTree *shaderTree = this->GetShaderTree(this->currentLevel, fourcc);
    if (!shaderTree)
    {
        shaderTree = nMaterialServer::Instance()->NewShaderTree(this, this->currentLevel, fourcc);
        n_assert(shaderTree);
        this->SetShaderTree(this->currentLevel, fourcc, shaderTree);
    }

    shaderTree->BeginNode(nVariable::InvalidHandle, 0);

    this->currentPass = fourcc;

    // prepare to load persisted shader tree
    nPersistServer* ps = kernelServer->GetPersistServer();
    ps->BeginObjectLoad(shaderTree, nObject::LoadedInstance);

    return shaderTree;
}

//------------------------------------------------------------------------------
/**
    Load all associated shaders through the material server.
*/
void
nMaterial::EndPass()
{
    this->GetShaderTree(this->currentLevel, this->currentPass)->EndNode();
}

//------------------------------------------------------------------------------
/**
    Get shader object for a given pass.
    It should never be called before resources are loaded.
*/
nShaderTree*
nMaterial::GetShaderTree(const int level, nFourCC fourcc)
{
    int levelIndex = n_min(level, this->GetNumLevels() - 1);
    nArray<Pass>& passes = this->levelPasses[levelIndex];

    for (int i = 0; i < passes.Size(); i++)
    {
        if (passes[i].fourcc == fourcc)
        {
            if (passes[i].refShaderTree.isvalid())
            {
                return passes[i].refShaderTree.get();
            }
            return 0;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Assign shader object from material server.
*/
void
nMaterial::SetShaderTree(const int level, nFourCC fourcc, nShaderTree *shaderTree)
{
    // find and replace existing shader for a pass
    int levelIndex = min(level, this->GetNumLevels() - 1);
    nArray<Pass>& passes = this->levelPasses[levelIndex];
    for (int i = 0; i < passes.Size(); i++)
    {
        if (passes[i].fourcc == fourcc)
        {
            if (passes[i].refShaderTree.isvalid())
            {
                passes[i].refShaderTree->Release();
            }
            passes[i].refShaderTree = shaderTree;
            return;
        }
    }

    // create new shader for a pass
    Pass newPass;
    newPass.level = this->currentLevel;
    newPass.fourcc = fourcc;
    newPass.refShaderTree = shaderTree;
    passes.Append(newPass);
}
