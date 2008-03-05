#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nrpsequence.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nrenderpath/nrpsequence.h"
#include "nrenderpath/nrenderpath2.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nRpSequence::nRpSequence() :
    renderPath(0),
    rpShaderIndex(-1),
    bucketIndex(-1),
    inBegin(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRpSequence::~nRpSequence()
{
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRpSequence::Validate()
{
    n_assert(this->renderPath);
/*
    // validate shader
    nShader2* shd = 0;
    if (!this->refShader.isvalid())
    {
        n_assert(!this->shaderPath.IsEmpty());
        shd = nGfxServer2::Instance()->NewShader(this->shaderPath.Get());
        //shd->SetShaderIndex(this->renderPath->GetSequenceShaderAndIncrement());
        this->refShader = shd;
    }
    else
    {
        shd = this->refShader;
    }

    n_assert(shd);
    if (!shd->IsLoaded())
    {
        shd->SetFilename(this->shaderPath);
        if (!shd->Load())
        {
            shd->Release();
            n_error("nRpPass: could not load shader '%s'!", this->shaderPath.Get());
        }
    }*/

    // find the bucket index for this sequence shader, if any
    if (this->rpShaderIndex == -1 && !this->shaderAlias.IsEmpty())
    {
        this->rpShaderIndex = this->renderPath->FindShaderIndex(this->shaderAlias.Get());
        if (this->rpShaderIndex == -1)
        {
            n_error("nRpSequence::Validate(): couldn't find shader alias '%s' in render path xml file!", this->shaderAlias.Get());
        }
        this->refShader = this->renderPath->GetShaderAt(this->rpShaderIndex).GetShader();
        this->technique = this->renderPath->GetShaderAt(this->rpShaderIndex).GetTechnique();
        n_assert(this->refShader.isvalid());
        this->refShader->AddRef();
    }

    this->bucketIndex = this->renderPath->GetSequenceShaderAndIncrement();
}

//------------------------------------------------------------------------------
/**
*/
void
nRpSequence::Begin()
{
    n_assert(!this->inBegin);
    this->inBegin = true;

    if (this->refShader.isvalid())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        //nShader2* shd = this->renderPath->GetShaderAt(this->rpShaderIndex).GetShader();
        nShader2* shd = this->refShader.get();
        if (!this->technique.IsEmpty())
        {
            shd->SetTechnique(this->technique.Get());
        }
        gfxServer->SetShader(shd);

        NLOG(renderpass, (2, "nRpSequence::Begin(name: %s, shader: %s, technique: %s)",
             this->GetShaderAlias(), shd->GetFilename().Get(), this->technique.Get()))

        N_IFDEF_ASSERTS(int numPasses =) shd->Begin(false); // expect exactly two passes, one for restore
        n_assert(numPasses == 2);
        shd->BeginPass(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRpSequence::End()
{
    n_assert(this->inBegin);

    if (this->refShader.isvalid())
    {
        nShader2* shd = this->refShader;
        shd->EndPass();

        // apply restoring pass
        shd->BeginPass(1);
        shd->EndPass();

        shd->End();
    }

    this->inBegin = false;
}
