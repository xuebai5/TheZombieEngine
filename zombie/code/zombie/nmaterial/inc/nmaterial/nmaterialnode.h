#ifndef N_MATERIALNODE_H
#define N_MATERIALNODE_H
//------------------------------------------------------------------------------
/**
    @class nMaterialNode
    @ingroup NebulaMaterialSystem

    @brief A material node defines a shader resource and associated shader
    variables through an abstract material definition.
    
    See also @ref N2ScriptInterface_nmaterialnode

    (C) 2004 Conjurer Services, S.A.
*/
#include "nscene/nsurfacenode.h"
#include "kernel/ndynautoref.h"
#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialtree.h"

class nMaterialServer;
//------------------------------------------------------------------------------
class nMaterialNode : public nSurfaceNode
{
public:
    /// constructor
    nMaterialNode();
    /// destructor
    virtual ~nMaterialNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// load resources
    virtual void UnloadResources();

    /// get number of render levels
    virtual int GetNumLevels();
    /// get number of render passes by level
    virtual int GetNumLevelPasses(int level);
    /// get scene pass index by level and pass
    virtual int GetLevelPassIndex(int level, int pass);

    /// get shader tree
    virtual nShaderTree* GetShaderTree(int level, int passIndex);

    /// get number of render passes
    virtual int GetNumPasses(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// get render pass at index
    virtual nFourCC GetPassAt(int index, nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// get pass index at index
    virtual int GetPassIndexAt(int index, nSceneGraph* sceneGraph, nEntityObject* entityObject);
    
    /// set material name
    void SetMaterial(const char* name);
    /// get material name
    const char* GetMaterial() const;
    /// get material object
    nMaterial* GetMaterialObject();
    
protected:
    /// checks if shader uses texture passed in param
    virtual bool IsTextureUsed(nShaderState::Param param);

    /// the material referenced by this node
    nDynAutoRef<nMaterial> refMaterial;
    /// array of shader trees, indexed by pass index
    nArray<nArray<nShaderTree*> > shaderTreeArray;
    /// index of light pass
    int lghtPassIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialNode::SetMaterial(const char *name)
{
    this->refMaterial = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nMaterialNode::GetMaterial() const
{
    return this->refMaterial.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
nMaterial*
nMaterialNode::GetMaterialObject()
{
    return this->refMaterial.isvalid() ? this->refMaterial.get() : 0;
}

//------------------------------------------------------------------------------
#endif
