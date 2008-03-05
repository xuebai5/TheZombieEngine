#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nsceneserver_shader.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nsceneserver.h"
#include "nscene/nsceneshader.h"
#include "nscene/nshadertree.h"
#include "kernel/nlogclass.h"

NCREATELOGLEVEL(sceneshader, "Scene Shaders", false, 1)
// log shaders as they are registered into the scene database

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::SetShaderDatabaseFilename(const char* filename)
{
    this->shaderDatabaseFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
const char *
nSceneServer::GetShaderDatabaseFilename()
{
    return this->shaderDatabaseFilename.IsEmpty() ? 0 : this->shaderDatabaseFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneServer::ClearShaders()
{
    this->shaderArray.Clear();// no Reset(), this will have to release memory
}

//------------------------------------------------------------------------------
/**
    find shader entry by alias
    @return index of entry, or -1 if not found
*/
int
nSceneServer::FindShader(const char * shaderName)
{
    n_assert(shaderName);

    int i;
    for (i = 0; i < this->shaderArray.Size(); ++i)
    {
        if (!strcmp(this->shaderArray.At(i).GetShaderName(), shaderName))
        {
            return i;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
    find shader entry by filename and technique
    @return index of entry, or -1 if not found
*/
int
nSceneServer::FindShader(const char* filename, const char* technique)
{
    n_assert(filename);

    int i;
    for (i = 0; i < this->shaderArray.Size(); ++i)
    {
        nSceneShader& sceneShader = this->shaderArray.At(i);
        if (!strcmp(sceneShader.GetShader(), filename))
        {
            if (technique == 0 && sceneShader.GetTechnique() == 0)
            {
                return i;
            }
            /// compare techniques as well
            if (technique != 0 && sceneShader.GetTechnique() != 0 &&
                !strcmp(technique, sceneShader.GetTechnique()))
            {
                return i;
            }
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
    find shader entry by shader object, return index or -1
    if no technique is given, 0 is assumed (old-style, single technique shaders)
    @return index of entry, or -1 if not found
*/
int
nSceneServer::FindShader(nShader2* shader, const char* technique)
{
    n_assert(shader);

    int i;
    for (i = 0; i < this->shaderArray.Size(); ++i)
    {
        nSceneShader& sceneShader = this->shaderArray.At(i);
        if (sceneShader.IsValid() && sceneShader.GetShaderObject() == shader)
        {
            if (technique == 0 && sceneShader.GetTechnique() == 0)
            {
                return i;
            }
            /// compare techniques as well
            if (technique != 0 && sceneShader.GetTechnique() != 0 &&
                !strcmp(technique, sceneShader.GetTechnique()))
            {
                return i;
            }
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
int
nSceneServer::AddShader(nSceneShader& sceneShader)
{
    n_assert_return3(sceneShader.GetShader(), -1, ("nSceneServer::AddShader tried to add a shader without filename:'%s'", sceneShader.GetShaderName()));
    n_assert_return3(sceneShader.GetShaderName(), -1, ("nSceneServer::AddShader tried to add a shader without name: '%s'", sceneShader.GetShader()));

    this->shaderArray.Append(sceneShader);
    int shaderIndex = this->shaderArray.Size() - 1;
    this->shaderArray.Back().shaderIndex = shaderIndex;
    if (!this->loadShadersOnDemand && !this->shaderArray.Back().IsValid())
    {
        this->shaderArray.Back().Validate();
    }

    NLOG(sceneshader, (0, "nSceneServer::AddShader(name: %s, file: %s, technique: %s, shaderIndex: %i)", 
         sceneShader.GetShaderName(), sceneShader.GetShader(), sceneShader.technique.Get(), shaderIndex));

    return shaderIndex;
}

//------------------------------------------------------------------------------
/**
*/
nSceneShader&
nSceneServer::GetShaderAt(int shaderIndex)
{
    return this->shaderArray.At(shaderIndex);
}

//------------------------------------------------------------------------------
/**
*/
int
nSceneServer::GetNumShaders()
{
    return this->shaderArray.Size();
}

//------------------------------------------------------------------------------
/**
    @todo ma.garcias- this is to load a database of persisted shaders
    from a comprehensive xml file. It will be useful to have a prebuilt
    shader library (like the one in nebula2 os) to reference shaders by 
    name using a shader alias and a combination of .fx filename + technique
*/
void
nSceneServer::LoadShaders()
{
    //n_assert(!this->shaderDatabaseFilename.IsEmpty());
    //load shader database
}

//------------------------------------------------------------------------------
/**
    @todo save current shader database
    this is intended to save the whole shader database for a level
    and preload at startup all required shaders.
*/
//void
//nSceneServer::SaveShaders(const char* xmlFilename)
//{
//....
//}

//------------------------------------------------------------------------------
/**
*/
nShaderTree*
nSceneServer::NewShaderTree(const char *treeName)
{
    nRef<nShaderTree> refShaderTree = this->FindShaderTree(treeName);
    if (refShaderTree.isvalid())
    {
        refShaderTree->AddRef();
    }
    else
    {
        refShaderTree = static_cast<nShaderTree*>( kernelServer->New("nshadertree") );
        n_assert(refShaderTree.isvalid());
        this->shaderTreeMap.Add(treeName, &refShaderTree);
    }

    return refShaderTree.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
nShaderTree*
nSceneServer::FindShaderTree(const char *treeName)
{
    if (treeName)
    {
        nRef<nShaderTree>* refShaderTree = this->shaderTreeMap[treeName];
        if (refShaderTree->isvalid())
        {
            return refShaderTree->get();
        }
    }

    return 0;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Build a shader to graphically show that a shader was wrong.
*/
nSceneShader&
nSceneServer::GetErrorShader()
{
    int shaderIndex = this->FindShader("materialerror");
    if (shaderIndex == -1)
    {
        nShader2* errorShader = nGfxServer2::Instance()->NewShader("shaders:materialerror.fx");
        n_assert(errorShader);
        if (!errorShader->IsLoaded())
        {
            errorShader->SetFilename("shaders:materialerror.fx");
            n_verify(errorShader->Load());
        }

        nSceneShader newSceneShader(errorShader);
        newSceneShader.SetShader("shaders:materialerror.fx");
        newSceneShader.SetShaderName("materialerror");
        shaderIndex = nSceneServer::Instance()->AddShader(newSceneShader);
    }

    return this->GetShaderAt(shaderIndex);
}
#endif
