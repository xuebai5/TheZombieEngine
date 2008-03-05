#ifndef N_RPPHASE_H
#define N_RPPHASE_H
//------------------------------------------------------------------------------
/**
    @class nRpPhase
    @ingroup NebulaRenderPathSystem

    A phase object inside a render path pass encapsulates sequence shaders
    and sets common render state for sequence shaders.
    
    (C) 2004 RadonLabs GmbH
*/
#include "util/nstring.h"
#include "nscene/ncscene.h"
//#include "nscene/nshapebucket.h"
#include "nrenderpath/nrpsequence.h"

class nRenderPath2;
class nSceneGraph;
//------------------------------------------------------------------------------
class nRpPhase
{
public:
    /// sorting orders
    enum SortingOrder
    {
        None,
        Surface,
        FrontToBack,
        BackToFront,
        LocalBoxFrontToBack,
        LocalBoxBackToFront,
    };

    /// light flags
    enum LightModeFlag
    {
        RenderOncePerLight  = (1<<0),
        CheckLightEntity    = (1<<1),
    };

    /// constructor
    nRpPhase();
    /// destructor
    ~nRpPhase();
    /// set parent render path
    void SetRenderPath(nRenderPath2* renderPath);
    /// get parent render path
    nRenderPath2* GetRenderPath();
    /// assignment operator
    void operator=(const nRpPhase& rhs);
    /// set phase name
    void SetName(const nString& n);
    /// get phase name
    const nString& GetName() const;
    /// set phase shader's path
    void SetShaderPath(const nString& p);
    /// get phase shader's path
    const nString& GetShaderPath() const;
    /// set optional technique in shader
    void SetTechnique(const nString& n);
    /// get optional shader technique
    const nString& GetTechnique() const;
    /// set shader fourcc code
    void SetFourCC(nFourCC fcc);
    /// get shader fourcc code
    nFourCC GetFourCC() const;
    /// get shader pass index
    uint GetPassIndex() const;
    /// set sorting order
    void SetSortingOrder(SortingOrder o);
    /// get sorting order
    SortingOrder GetSortingOrder() const;
    /// enable/disable lighting
    void SetLightsEnabled(bool b);
    /// get light enabled flag
    bool GetLightsEnabled() const;
    /// set light mode flags
    void SetLightModeFlags(int flags);
    /// get light mode flags
    int GetLightModeFlags() const;
    /// set visible flags
    void SetVisibleFlags(int flags);
    /// get visible flags
    int GetVisibleFlags() const;
    /// enable/disable restore pass
    void SetRestoreState(bool b);
    /// get if restore pass enabled
    bool GetRestoreState() const;
    /// add a sequence object
    void AddSequence(const nRpSequence& seq);
    /// find a sequence object by name
    nRpSequence& FindSequence(const char* name);
    /// find a sequence index by name
    int FindSequenceIndex(const char* name);
    /// get array of sequences
    const nArray<nRpSequence>& GetSequences() const;
    /// get sequence by index
    nRpSequence& GetSequence(int i) const;
    /// get index of phase bucket
    int GetShaderBucketIndex() const;
    /// convert string to sorting order
    static SortingOrder StringToSortingOrder(const char* str);
    /// convert string to visible flags
    static ncScene::Flag StringToVisibleFlags(const char *str);
    /// convert string to light flags
    static int StringToLightFlags(const char *str);

    /// begin rendering the phase
    int Begin();
    /// finish rendering the phase
    void End();

    /// get shader object
    nShader2* GetShader();

private:
    friend class nRpPass;

    /// validate the pass object
    void Validate();

    bool inBegin;
    nString name;
    nString shaderPath;
    nString technique;
    nFourCC shaderFourCC;
    uint passIndex;
    SortingOrder sortingOrder;
    bool lightsEnabled;
    bool restoreState;
    int visibleFlags;
    int lightModeFlags;
    int bucketIndex;
    nArray<nRpSequence> sequences;
    nRef<nShader2> refShader;
    nRenderPath2* renderPath;

    //<TODO> move back to nRenderPathNode::BeginPhase(nSceneGraph*, nEntityObject*)

    /// this is for shader-sorted phases:
    //nShapeBucket shapeBucket;

    //</TODO>
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetRenderPath(nRenderPath2* renderPath)
{
    this->renderPath = renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRenderPath2*
nRpPhase::GetRenderPath()
{
    return this->renderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::operator=(const nRpPhase& rhs)
{
    this->inBegin       = rhs.inBegin;
    this->name          = rhs.name;
    this->shaderPath    = rhs.shaderPath;
    this->technique     = rhs.technique;
    this->shaderFourCC  = rhs.shaderFourCC;
    this->passIndex     = rhs.passIndex;
    this->sortingOrder  = rhs.sortingOrder;
    this->lightsEnabled = rhs.lightsEnabled;
    this->restoreState  = rhs.restoreState;
    this->visibleFlags  = rhs.visibleFlags;
    this->lightModeFlags= rhs.lightModeFlags;
    this->sequences     = rhs.sequences;
    this->refShader     = rhs.refShader;
    if (this->refShader.isvalid())
    {
        this->refShader->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetTechnique(const nString& t)
{
    this->technique = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPhase::GetTechnique() const
{
    return this->technique;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString& 
nRpPhase::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetShaderPath(const nString& p)
{
    this->shaderPath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nRpPhase::GetShaderPath() const
{
    return this->shaderPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nRpPhase::GetShader()
{
    return this->refShader.get_unsafe();
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nRpPhase::GetFourCC() const
{
    return this->shaderFourCC;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
nRpPhase::GetPassIndex() const
{
    return this->passIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetSortingOrder(SortingOrder o)
{
    this->sortingOrder = o;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpPhase::SortingOrder
nRpPhase::GetSortingOrder() const
{
    return this->sortingOrder;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetLightsEnabled(bool b)
{
    this->lightsEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpPhase::GetLightsEnabled() const
{
    return this->lightsEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetRestoreState(bool b)
{
    this->restoreState = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nRpPhase::GetRestoreState() const
{
    return this->restoreState;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetVisibleFlags(int flags)
{
    this->visibleFlags = flags;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpPhase::GetVisibleFlags() const
{
    return this->visibleFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::SetLightModeFlags(int flags)
{
    this->lightModeFlags = flags;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpPhase::GetLightModeFlags() const
{
    return this->lightModeFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRpPhase::AddSequence(const nRpSequence& seq)
{
    this->sequences.Append(seq);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<nRpSequence>&
nRpPhase::GetSequences() const
{
    return this->sequences;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpSequence&
nRpPhase::GetSequence(int i) const
{
    return this->sequences[i];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpPhase::GetShaderBucketIndex() const
{
    return this->bucketIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
nRpPhase::SortingOrder
nRpPhase::StringToSortingOrder(const char* str)
{
    n_assert(str);
    if (0 == strcmp("None", str)) return None;
    else if (0 == strcmp("Surface", str)) return Surface;
    else if (0 == strcmp("FrontToBack", str)) return FrontToBack;
    else if (0 == strcmp("BackToFront", str)) return BackToFront;
    else if (0 == strcmp("LocalBoxFrontToBack", str)) return LocalBoxFrontToBack;
    else if (0 == strcmp("LocalBoxBackToFront", str)) return LocalBoxBackToFront;
    else
    {
        n_error("nRpPhase::StringToSortingOrder(): invalid string '%s'!", str);
        return None;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
ncScene::Flag
nRpPhase::StringToVisibleFlags(const char* str)
{
    n_assert(str);
    if (0 == strcmp("ShapeVisible", str)) return ncScene::ShapeVisible;
    else if (0 == strcmp("ShadowVisible", str)) return ncScene::ShadowVisible;
    else if (0 == strcmp("CastShadows", str)) return ncScene::CastShadows;
    else if (0 == strcmp("AlwaysOnTop", str)) return ncScene::AlwaysOnTop;
    else
    {
        n_error("nRpPhase::StringToVisibleFlags(): invalid string '%s'!", str);
        return ncScene::NoFlags;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nRpPhase::StringToLightFlags(const char* str)
{
    n_assert(str);
    nString strFlags(str);
    int flags = 0;
    const char* curToken = strFlags.GetFirstToken("| ");
    while (curToken)
    {
        if (strcmp("RenderOncePerLight", curToken) == 0)
        {
            flags |= RenderOncePerLight;
        }
        else if (strcmp("CheckLightEntity", curToken) == 0)
        {
            flags |= CheckLightEntity;
        }
        else
        {
            n_printf("nRpPhase::StringToLightFlags(): invalid token '%s'\n", curToken);
        }
        curToken = strFlags.GetNextToken("| ");
    }
    return flags;
}

//------------------------------------------------------------------------------
#endif    
