#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nrpphase.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nrenderpath/nrpphase.h"
#include "nscene/nsceneserver.h"
#include "nscene/nscenegraph.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/nrenderpathnode.h"
#include "gfx2/ngfxserver2.h"

#include "kernel/nlogclass.h"

NCREATELOGLEVEL(scenephase, "Renderpath Phases", false, 2)
//0- log begin/end phase, num buckets collected
//1- detailed per-shape log

//------------------------------------------------------------------------------
/**
*/
nRpPhase::nRpPhase() :
    renderPath(0),
    inBegin(false),
    sortingOrder(FrontToBack),
    lightsEnabled(true),
    restoreState(false),
    visibleFlags(ncScene::ShapeVisible),
    lightModeFlags(0)
{
    // push default sequence
    this->sequences.At(0);
}

//------------------------------------------------------------------------------
/**
*/
nRpPhase::~nRpPhase()
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
nRpSequence&
nRpPhase::FindSequence(const char* name)
{
    if (name && this->sequences.Size() > 1)
    {
        int numSequences = this->sequences.Size();
        for (int index = 1; index < numSequences; ++index)
        {
            if (strcmp(this->sequences[index].GetShaderAlias(), name) == 0)
            {
                return this->sequences[index];
            }
        }
    }
    return this->sequences.At(0);
}

//------------------------------------------------------------------------------
/**
*/
int
nRpPhase::FindSequenceIndex(const char* name)
{
    if (name && this->sequences.Size() > 1)
    {
        int numSequences = this->sequences.Size();
        for (int index = 1; index < numSequences; ++index)
        {
            if (strcmp(this->sequences[index].GetShaderAlias(), name) == 0)
            {
                return index;
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nRpPhase::SetFourCC(nFourCC fcc)
{
    this->shaderFourCC = fcc;
    this->passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(fcc);
}

//------------------------------------------------------------------------------
/**
*/
int
nRpPhase::Begin()
{
    n_assert(!this->inBegin);

    // note: save/restore state for phase shaders!
    if (this->refShader.isvalid())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        nShader2* shd = this->refShader;
        if (!this->technique.IsEmpty())
        {
            shd->SetTechnique(this->technique.Get());
        }
        gfxServer->SetShader(shd);
        N_IFDEF_ASSERTS(int numPasses =) shd->Begin(false);
        N_IFDEF_ASSERTS(int numExpectedPasses = this->GetRestoreState() ? 2 : 1);
        n_assert(numPasses == numExpectedPasses);
        shd->BeginPass(0);
    }

    this->inBegin = true;
    return this->sequences.Size();
}

//------------------------------------------------------------------------------
/**
*/
void
nRpPhase::End()
{
    n_assert(this->inBegin);

    if (this->refShader.isvalid())
    {
        nShader2* shd = this->refShader;
        shd->EndPass();

        // apply restoring pass, if it exists
        if (this->GetRestoreState())
        {
            shd->BeginPass(1);
            shd->EndPass();
        }

        shd->End();
    }

    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nRpPhase::Validate()
{
    n_assert(this->renderPath);

    // get index in render path
    this->bucketIndex = this->renderPath->GetPhaseShaderAndIncrement();

    // invoke validate on sequences
    int i;
    int num = this->sequences.Size();
    for (i = 0; i < num; i++)
    {
        this->sequences[i].SetRenderPath(this->renderPath);
        this->sequences[i].Validate();
    }

    // validate shader
    nShader2* shd = 0;
    if (!this->refShader.isvalid())
    {
        if (!this->shaderPath.IsEmpty())
        {
            shd = nGfxServer2::Instance()->NewShader(this->shaderPath.Get());
            this->refShader = shd;
        }
    }
    else
    {
        shd = this->refShader;
    }

    if (shd && !shd->IsLoaded())
    {
        shd->SetFilename(this->shaderPath);
        if (!shd->Load())
        {
            shd->Release();
            n_error("nRpPass: could not load shader '%s'!", this->shaderPath.Get());
        }
    }
}
