#ifndef N_LENSFLARENODE_H
#define N_LENSFLARENODE_H
//------------------------------------------------------------------------------
/**
    @class nLensFlareNode
    @ingroup SceneNodes

    @brief Geometry node implementing a lens flare overlay effect.
    Ported from original lenseflare implementation in nebula1.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ngeometrynode.h"
#include "gfx2/ndynamicmesh.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
struct nFlareDesc
{
    friend class nLensFlareNode;

    /// constructor
    nFlareDesc();
    /// destructor
    ~nFlareDesc();

private:
    float fSize;        //  length of one side of quad (flare)
    float fHalfSize;    //  1/2 of the size (used when placing quad on line)
    float fPosition;    //  % distance from light source to center of screen;
                        //  0.0f places the flare over the light source, 1.0f
                        //  places the flare at the center of the screen; higher
                        //  values place the flare further along the line (2.0f
                        //  places the flare opposite the light source, etc.),
                        //  and negative values place the flare behind the light
    vector4 color;      //  the vertex coloring of the flare
};

//------------------------------------------------------------------------------
class nLensFlareNode : public nGeometryNode
{
public:
    /// constructor
    nLensFlareNode();
    /// destructor
    virtual ~nLensFlareNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// compute and render the lens flare geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// @name Script Interface
    //@{

    /// set color for the flare effect
    void SetBaseColor(const vector4&);
    /// get base color for the effect
    const vector4& GetBaseColor();
    /// set color for the blind effect
    void SetBlindColor(const vector4&);
    /// set color for the blind effect
    const vector4& GetBlindColor();
    /// allocate memory for the number of flares
    void BeginFlares(int);
    /// set flare size
    void SetFlareSizeAt(int, float);
    /// get flare size
    float GetFlareSizeAt(int);
    /// set flare position relative to the center
    void SetFlarePosAt(int, float);
    /// get flare position relative to the center
    float GetFlarePosAt(int);
    /// set color to an individual flare
    void SetFlareColorAt(int, const vector4&);
    /// get color to an individual flare
    const vector4& GetFlareColorAt(int);
    /// end defining flares
    void EndFlares();
    /// get number of flares
    int GetNumFlares();

    //@}

private:

    void CalcSource2d(const matrix44& modelView, const matrix44& proj);
    void Blind();

    enum
    {
        MAXFLARES = 16,
    };

    nArray<nFlareDesc> flares;
    vector4 vSource2d;      // eventual pseudo-screen space position of light
    bool bBegun;            // signals that BeginFlares(...) has completed
    bool bEnded;            // signals that EndFlares(...) has completed
    bool bWithinBounds;     // signals that the lenseflare is visible
    int numVertices;

    /// base color that is combined with float values received from
    /// a mixer (fix so that nlenseflare works with a mixer
    vector4 baseColor;
    /// blinding color when looking at light source
    vector4 blindColor;
    /// dynamic mesh
    nDynamicMesh dynMesh;
};

//------------------------------------------------------------------------------
/**
*/
inline
nFlareDesc::nFlareDesc() : 
    fSize(0.125f),
    fHalfSize(fSize / 2),
    fPosition(1.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nFlareDesc::~nFlareDesc()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nLensFlareNode::GetNumFlares() 
{
    return this->flares.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nLensFlareNode::GetFlareSizeAt(int index)
{
    return this->flares[index].fSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nLensFlareNode::GetFlarePosAt(int index)
{
    return this->flares[index].fPosition;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLensFlareNode::GetFlareColorAt(int index)
{
    return this->flares[index].color;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLensFlareNode::SetBaseColor(const vector4& color)
{
    this->baseColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLensFlareNode::GetBaseColor()
{
    return this->baseColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLensFlareNode::SetBlindColor(const vector4& color)
{
    this->blindColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nLensFlareNode::GetBlindColor()
{
    return this->blindColor;
}

//------------------------------------------------------------------------------
#endif
