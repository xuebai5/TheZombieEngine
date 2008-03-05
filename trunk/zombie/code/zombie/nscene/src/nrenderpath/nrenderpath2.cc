#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nRenderPath2.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nrenderpath/nRenderPath2.h"
//#include "nrenderpath/nrprendertarget.h"
#include "nrenderpath/nrpxmlparser.h"

//------------------------------------------------------------------------------
/**
*/
nRenderPath2::nRenderPath2() :
    isOpen(false),
    inBegin(false),
    sequenceShaderIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRenderPath2::~nRenderPath2()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Open the XML document. This will just load the XML document and
    initialize the shader path. The rest of the initialization happens
    inside nRenderPath2::Open(). This 2-step approach is necessary to
    prevent a shader initialization chicken/egg problem 
*/
bool
nRenderPath2::OpenXml()
{
    this->xmlParser.SetRenderPath(this);
    if (!this->xmlParser.OpenXml())
    {
        return false;
    }
    n_assert(!this->shaderPath.IsEmpty());
    n_assert(!this->name.IsEmpty());
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the XML document. This method should be called after
    nRenderPath2::Open() to release the memory assigned to the XML
    document data.
*/
void
nRenderPath2::CloseXml()
{
    this->xmlParser.CloseXml();
}

//------------------------------------------------------------------------------
/**
    Open the render path. This will parse the xml file which describes
    the render path and configure the render path object from it.
*/
bool
nRenderPath2::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->inBegin);

    this->phaseShaderIndex = 0;
    this->sequenceShaderIndex = 0;

    if (!xmlParser.ParseXml())
    {
        return false;
    }
    this->Validate();
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Close the render path. This will delete all embedded objects.
*/
void
nRenderPath2::Close()
{
    n_assert(this->isOpen);
    n_assert(!this->inBegin);
    this->name.Clear();

    for (int index = 0; index < this->passes.Size(); ++index)
    {
        this->passes[index].ReleaseTextures();
    }
    this->passes.Clear();

    this->renderTargets.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Begin rendering the render path. This will validate all embedded objects.
    Returns the number of scene passes in the render path.
    After begin, each pass should be "rendered" recursively.
*/
int
nRenderPath2::Begin()
{
    n_assert(!this->inBegin);
    this->inBegin = true;
    return this->passes.Size();
}

//------------------------------------------------------------------------------
/**
    Finish rendering the render path.
*/
void
nRenderPath2::End()
{
    n_assert(this->inBegin);
    this->inBegin = false;
}

//------------------------------------------------------------------------------
/**
    Validate the render path. This will simply invoke Validate() on all
    render targets and pass objects.
*/
void
nRenderPath2::Validate()
{
    n_assert(!this->isOpen);

    // reset index of phases and indices
    this->phaseShaderIndex = 0;
    this->sequenceShaderIndex = 0;

    // validate render targets
    int numRenderTargets = this->renderTargets.Size();
    for (int index = 0; index < numRenderTargets; ++index)
    {
        this->renderTargets[index].Validate();
    }

    // validate shaders
    int numShaders = this->shaders.Size();
    for (int index = 0; index < numShaders; ++index)
    {
        this->shaders[index].Validate();
    }

    // validate passes
    int passIndex;
    int numPasses = this->passes.Size();
    for (passIndex = 0; passIndex < numPasses; passIndex++)
    {
        this->passes[passIndex].SetRenderPath(this);
        this->passes[passIndex].Validate();
    }

    // at the end of this process, we have the number of sequence shaders
}

//------------------------------------------------------------------------------
/**
    Find a render target by name.
*/
nRpRenderTarget*
nRenderPath2::FindRenderTarget(const nString& n) const
{
    int i;
    int num = this->renderTargets.Size();
    for (i = 0; i < num; i++)
    {
        if (this->renderTargets[i].GetName() == n)
        {
            return &(this->renderTargets[i]);
        }
    }
    // fallthrough: not found
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nRenderPath2::AddRenderTarget(nRpRenderTarget& rt)
{
    rt.Validate();
    this->renderTargets.Append(rt);
}

//------------------------------------------------------------------------------
/**
    Find a shader definition index by its name. Return -1 if not found.
*/
int
nRenderPath2::FindShaderIndex(const char* name) const
{
    int num = this->shaders.Size();
    for (int index = 0; index < num; ++index)
    {
        if (strcmp(this->shaders[index].GetName(), name) == 0)
        {
            return index;
        }
    }
    // fallthrough: not found
    return -1;
}
