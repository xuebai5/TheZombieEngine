#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nscenesshader.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nsceneshader.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    load shader from filename, check valid technique
*/
void
nSceneShader::Validate()
{
    n_assert(!this->IsValid());
    n_assert(!this->filename.IsEmpty());

    nShader2* shader = nGfxServer2::Instance()->NewShader(this->filename.Get());
    n_assert(shader);
    if (!shader->IsLoaded())
    {
        shader->SetFilename(this->filename.Get());
        if (!shader->Load())
        {
            shader->Release();

            #ifndef NGAME
            //replace with error shader if shader could not be loaded
            NLOG(resource, (0, "Could not validate shader '%s', replacing with error shader", this->filename.Get()))
            shader = nSceneServer::Instance()->GetErrorShader().GetShaderObject();
            n_assert(shader);
            this->refShader = shader;
            this->technique.Clear();
            #endif

            return;
        }

        // check that technique is valid
        if (this->GetTechnique())
        {
            n_assert2(shader->HasTechnique(this->GetTechnique()), 
                ("ma.garcias- shader '%s' has no technique named '%s'", this->filename.Get(), this->GetTechnique()));
        }

        #if __NEBULA_STATS__
        ++nSceneServer::Instance()->statsNumValidShaders;
        #endif
    }

    NLOG(sceneshader, (0, "Validated shader name: '%s' filename: '%s' technique: '%s' index: %d", this->shaderName.Get(), this->filename.Get(), this->technique.Get(), this->shaderIndex))
    this->refShader = shader;
}

//------------------------------------------------------------------------------
/**
*/
int
nSceneShader::Begin()
{
    n_assert(this->IsValid());
    n_assert(!this->inBeginShader);
    this->inBeginShader = true;

    if (!this->technique.IsEmpty())
    {
        this->refShader->SetTechnique(this->technique.Get());
    }
    return this->refShader->Begin(false);
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneShader::End()
{
    n_assert(this->inBeginShader);
    this->inBeginShader = false;
    this->refShader->End();
}
