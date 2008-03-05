#ifndef N_MATERIAL_H
#define N_MATERIAL_H
//------------------------------------------------------------------------------
/**
    @class nMaterial
    @ingroup NebulaMaterialSystem

    @brief Material definition.
    An interface for defining materials that can be used in scenes as surfaces.

    (C) 2004 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"
#include "kernel/ndynautoref.h"
#include "util/narray.h"
#include "nscene/nsceneserver.h"
#include "variable/nvariable.h"

class nMaterialServer;
class nShaderTree;
class nShader2;
//------------------------------------------------------------------------------
class nMaterial : public nRoot
{
public:
    /// constructor
    nMaterial();
    /// destructor
    virtual ~nMaterial();
    /// object persistence
    virtual bool SaveCmds(nPersistServer *ps);

    /// add a named attribute
    void AddParam(const char *paramName, const char *paramValue);
    /// check for a material attribute
    bool HasParam(const char *paramName, const char *paramValue);
    /// retrieve a material attribute
    const char *GetParam(const char *paramName);
    /// get material attribute by index
    void GetParamAt(int param, const char **paramName, const char **paramValue);
    /// get number of material attributes
    int GetNumParams();
    /// remove a named attribute
    void RemoveParam(const char *paramName, const char *paramValue);

    /// begin a level
    void BeginLevel();
    /// get current level
    const int GetCurrentLevel();
    /// end a level
    void EndLevel();

    /// begin filling a shader tree for a new pass
    nObject* BeginPass(nFourCC fourcc);
    /// OBSOLETE- end current shader tree for the current pass
    void EndPass();
    /// clear all levels and passes
    void ClearLevels();
    /// get number of detail levels
    int GetNumLevels();
    /// get number of render passes
    int GetNumPasses(const int level);
    /// get handle for a pass
    nFourCC GetPassAt(const int level, int index);
    
    /// load shader resources
    bool LoadShaders();
    /// unload shader resources
    void UnloadShaders();
    /// get shader for a pass
    nShaderTree *GetShaderTree(const int level, nFourCC fourcc);
    /// set shader for a pass
    void SetShaderTree(const int level, nFourCC fourcc, nShaderTree* shaderTree);
    
private:
    struct Pass
    {
        int level;
        nFourCC fourcc;
        nRef<nShaderTree> refShaderTree;
    };

    struct Param
    {
        nString name;
        nString value;
    };

    nArray<nArray<Pass> > levelPasses;
    nArray<Param> params;
    bool inBeginLevel;
    int currentLevel;
    nFourCC currentPass;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterial::AddParam(const char *paramName, const char *paramValue)
{
    Param newParam;
    newParam.name = paramName;
    newParam.value = paramValue;
    this->params.Append(newParam);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterial::RemoveParam(const char *paramName, const char *paramValue)
{
    nArray<Param>::iterator paramIter = this->params.Begin();
    while (paramIter != this->params.End())
    {
        Param& param = (*paramIter);
        if (param.name == paramName && param.value == paramValue)
        {
            this->params.Erase(paramIter);
            return;
        }
        ++paramIter;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMaterial::HasParam(const char *paramName, const char *paramValue)
{
    for (int i = 0; i < this->params.Size(); i++)
    {
        Param& param = this->params[i];
        if (param.name == paramName && param.value == paramValue)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nMaterial::GetParam(const char *paramName)
{
    for (int i = 0; i < this->params.Size(); i++)
    {
        Param& param = this->params[i];
        if (param.name == paramName)
        {
            return param.value.Get();
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMaterial::GetNumParams()
{
    return this->params.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterial::GetParamAt(int param, const char **paramName, const char **paramValue)
{
    Param& p = this->params[param];
    *paramName = p.name.Get();
    *paramValue = p.value.Get();
}

//------------------------------------------------------------------------------
/**
    FIXME not needed for now, but it will be.
    when clearing, should release any shaders, etc.
*/
inline
void
nMaterial::ClearLevels()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMaterial::GetNumLevels()
{
    return this->levelPasses.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nMaterial::GetNumPasses(const int level)
{
    int levelIndex = min(level, this->GetNumLevels() - 1);
    return this->levelPasses[levelIndex].Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nMaterial::GetPassAt(const int level, const int index)
{
    int levelIndex = min(level, this->GetNumLevels() - 1);
    return this->levelPasses[levelIndex][index].fourcc;
}

//------------------------------------------------------------------------------
#endif /* N_MATERIAL_H */
