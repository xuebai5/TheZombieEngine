#ifndef N_D3D9MATERIALBUILDER_H
#define N_D3D9MATERIALBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9MaterialBuilder
    @ingroup NebulaMaterialSystem

    @brief Builder of custom shaders from material descriptions

    (C) 2004 Conjurer Services, S.A.
*/
#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialbuilder.h"
#include "kernel/nautoref.h"
#include "file/nmemfile.h"
#include "util/nstack.h"
#include "util/nsortedarray.h"

class nGfxServer2;
class nShader2;
class nScriptServer;
class nVariableServer;
class nShaderTree;
//------------------------------------------------------------------------------
class nD3D9MaterialBuilder : public nMaterialBuilder
{
public:
    /// constructor
    nD3D9MaterialBuilder();
    /// destructor
    ~nD3D9MaterialBuilder();
    /// build passes and shader for a material
    virtual bool LoadMaterial(nMaterial *material);
    
    /// get set of levels for a material
    bool LoadLevels(nMaterial *material);
    /// get set of passes for a material
    bool LoadPasses(nMaterial *material);
    /// load shader from code built on the fly
    bool LoadShader(nMaterial *material, const nFourCC fourcc, nShaderTree *tree);
    /// traverse the case tree for building shaders
    bool Traverse();
    /// build shader for the current case value
    bool BuildShader();
    
    /// set script function for loading detail levels
    void SetOnBuildShaderLevelsFunction(const char *functionName);
    /// set script function for loading passes
    void SetOnBuildShaderPassesFunction(const char *functionName);
    /// set script function for building cases
    void SetOnBuildShaderTreeFunction(const char *functionName);
    /// set script function for building shader file name
    void SetOnBuildShaderNameFunction(const char *functionName);
    /// set script function for building shaders
    void SetOnBuildShaderFunction(const char *functionName);
    
    /// get current material path
    const char *GetMaterial();
    /// set number of levels for current material
    void SetNumLevels(const int numLevels);
    /// get current level for current material
    const int GetLevel();
    /// add pass to the current material
    void AddPass(const nFourCC fourcc);
    /// get current pass
    uint GetPass();
    /// add a fixed attribute to current shader description
    void AddAttribute(const char *name, const char *value);
    /// get an attribute from current shader description
    const char *GetAttribute(const char *name);
    /// get an attribute from current shader case
    const char *GetCaseValue(const char *name);
    /// begin a variable case in current shader description
    void BeginCaseVar(const char *name);
    /// add a new case value to the current shader description
    void AddCaseValue(const char *value);
    /// end a variable case in current shader description
    void EndCaseVar();
    /// get lenght of case path
    int GetCasePathLen();
    /// get case at index
    void GetCaseNodeAt(int index, const char **param, const char **value);
    /// get name of current material shader
    const char *GetShaderName();
    /// set shader filename
    void SetShaderFile(const char *fileName);
    /// set shader technique
    void SetTechnique(const char *technique);
    /// set shader sequence
    void SetSequence(const char *sequence);
    /// set if shader needs to be build
    void SetBuildShader(bool value);
    /// writes a string to the file
    bool PutS(const char* buffer);
    /// append a file to the file shader
    bool AppendFile(const char* filename);
    
private:
    
    /** @struct nD3D9MaterialBuilder::LodParam
        A LOD param is a hint to disable a param past a given level
        of detail. It can be used during the buildshaderlevels phase.
        .setparamlevel paramname paramvalue maxlevel
        Disables the (paramname, paramvalue) surface parameter 
        for levels of detail higher than maxlevel. If there aren't
        that many levels, the lod param is ignored.
    */
    struct ParamLevel
    {
        /// constructor
        ParamLevel();
        /// constructor
        ParamLevel(const char *name, const char *value, int level);
        
        int maxLevel;
        nString paramName;
        nString paramvalue;
    };

    /** @struct nD3D9MaterialBuilder::Node
        A case node as defined by the used in script. The interface for
        building a set of nodes from script is:
        .begincasevar varname       Creates a new node with var namevar
                                    and pushes it onto the node stack.
        .addcasevar varvalue        Adds a new value to the current node.
        .endcasevar                 Pops a node from the node stack.
    */
    struct Node
    {
        Node *parent;
        nVariable::Handle param;
        nArray<nString> values;
    };
    
    /** @struct nD3D9MaterialBuilder::Case
        A specific case (variable, value) that's built from a set of
        nodes during the BuildShader process. The array of currently
        active cases can be accessed through the following interface:
        .getcasevalue varname       Gets the value of a variable in the
                                    current path of cases.
        If the variable appears more than once in the path of cases
        (eg. parameters for several lights) the case value can be 
        referenced in array form (eg. "LightType[0]", "LightType[1]").
    */
    struct Case
    {
        nVariable::Handle param;
        nString value;
    };
    
    struct ShaderAttr
    {
        nString name;
        nString value;
    };
    
    enum
    {
        MaxNodeDepth = 16,
    };

    enum BuildState
    {
        None = 0,
        inBuildShaderLevels,
        inBuildShaderPasses,
        inBuildShaderTree,
        inBuildShader,
    };
    
    // states used in BuildShaderTree
    nArray<Node> nodeArray;
    static int __cdecl shaderDescSorter(const ShaderAttr* elm0, const ShaderAttr* elm1);
    nSortedArray<ShaderAttr, shaderDescSorter> shaderDesc;
    Node *nodeStack[MaxNodeDepth];          // stack of nodes
    uint stackDepth;                        // current stack depth
    
    // states used in traverse:
    Node *shaderNode;                       // current case (go back with parent)
    Case casePath[MaxNodeDepth];            // current path of case-values
    int pathLen;

    /// build a user-comprehensive error message
    void ShowErrorForCurrentCasePath();
    
    nAutoRef<nScriptServer> refScriptServer;
    
    nString shaderName;
    nString materialName;
    nString shaderFileName;                 // where to store the current file
    nString technique;                      // current technique
    nString sequence;                       // current sequence
    bool shaderNeedsBuilding;               // current shader needs to be built
    int level;                              // current level
    nFourCC fourcc;                         // current pass
    nRef<nMaterial> refMaterial;            // current material
    int numLevels;                          // number of detail levels
    nArray<ParamLevel> paramLevels;         // set of leveled material parameters
    nArray<nFourCC> passes;                 // set of passes for current material
    nMemFile *file;                         // memory file for building the shader code
    nRef<nShaderTree> refShaderTree;        // currently built shader tree
    nString onBuildShaderLevelsFunction;    // script callback for building set of levels
    nString onBuildShaderPassesFunction;    // script callback for building set of passes
    nString onBuildShaderTreeFunction;      // script callback for building shader tree
    nString onBuildShaderNameFunction;      // script callback for building shader name
    nString onBuildShaderFunction;          // script callback for building shader code
    BuildState buildState;                  // current build state
};

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9MaterialBuilder::ParamLevel::ParamLevel() :
    paramName(0),
    paramvalue(0),
    maxLevel(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nD3D9MaterialBuilder::ParamLevel::ParamLevel(const char *name, const char *value, int level) :
    paramName(name),
    paramvalue(value),
    maxLevel(level)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MaterialBuilder::SetOnBuildShaderLevelsFunction(const char *functionName)
{
    this->onBuildShaderLevelsFunction = functionName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MaterialBuilder::SetOnBuildShaderPassesFunction(const char *functionName)
{
    this->onBuildShaderPassesFunction = functionName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MaterialBuilder::SetOnBuildShaderTreeFunction(const char *functionName)
{
    this->onBuildShaderTreeFunction = functionName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MaterialBuilder::SetOnBuildShaderNameFunction(const char *functionName)
{
    this->onBuildShaderNameFunction = functionName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MaterialBuilder::SetOnBuildShaderFunction(const char *functionName)
{
    this->onBuildShaderFunction = functionName;
}

//------------------------------------------------------------------------------
/**
    set number of levels of detail for current material.
*/
inline
void
nD3D9MaterialBuilder::SetNumLevels(const int numLevels)
{
    n_assert(this->refMaterial.isvalid());
    n_assert(this->buildState == inBuildShaderLevels);
    this->numLevels = numLevels;
}

//------------------------------------------------------------------------------
/**
    set number of levels of detail for current material.
*/
inline
const int 
nD3D9MaterialBuilder::GetLevel()
{
    n_assert(this->refMaterial.isvalid());
    n_assert(this->buildState == inBuildShaderPasses || this->buildState == inBuildShaderTree);
    return this->level;
}

//------------------------------------------------------------------------------
/**
    Add a fourcc pass to current material. Intended for use from script.
*/
inline
void
nD3D9MaterialBuilder::AddPass(const nFourCC fourcc)
{
    n_assert(this->refMaterial.isvalid());
    n_assert(this->buildState == inBuildShaderPasses);
    this->passes.Append(fourcc);
}

//------------------------------------------------------------------------------
/**
    Get full path for current material. Intended for use from script.
*/
inline
const char *
nD3D9MaterialBuilder::GetMaterial()
{
    n_assert(this->refMaterial.isvalid());
    n_assert(this->buildState == inBuildShaderLevels ||
             this->buildState == inBuildShaderPasses ||
             this->buildState == inBuildShaderTree ||
             this->buildState == inBuildShader);
    return this->materialName.Get();
}

//------------------------------------------------------------------------------
/**
    Return current render pass. Intended for use from script.
*/
inline
uint
nD3D9MaterialBuilder::GetPass()
{
//    n_assert(this->buildState == inBuildShaderTree);
    return this->fourcc;
}

//------------------------------------------------------------------------------
/**
    Get name of current material shader (shader tree)
*/
inline
const char *
nD3D9MaterialBuilder::GetShaderName()
{
//    n_assert(this->buildState == inBuildShader);
    return this->shaderName.Get();
}

//------------------------------------------------------------------------------
/**
    Set file name for the current shader
*/
inline
void
nD3D9MaterialBuilder::SetShaderFile(const char* fileName)
{
    n_assert(this->buildState == inBuildShader);
    this->shaderFileName = fileName;
}

//------------------------------------------------------------------------------
/**
    Set file name for the current shader
*/
inline
void
nD3D9MaterialBuilder::SetTechnique(const char* technique)
{
    n_assert(this->buildState == inBuildShader);
    this->technique = technique;
}

//------------------------------------------------------------------------------
/**
    Set file name for the current shader
*/
inline
void
nD3D9MaterialBuilder::SetSequence(const char* sequence)
{
    n_assert(this->buildState == inBuildShader);
    this->sequence = sequence;
}

//------------------------------------------------------------------------------
/**
    Set if shader code needs to be built
*/
inline
void
nD3D9MaterialBuilder::SetBuildShader(bool value)
{
    n_assert(this->buildState == inBuildShader);
    this->shaderNeedsBuilding = value;
}

//------------------------------------------------------------------------------
/**
    Add a string to current shader file. Intended for use from script.
*/
inline
bool
nD3D9MaterialBuilder::PutS(const char* buffer)
{
    n_assert(this->file);
    n_assert(this->buildState == inBuildShader);
    return this->file->PutS(buffer);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nD3D9MaterialBuilder::GetCasePathLen()
{
    return this->pathLen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nD3D9MaterialBuilder::GetCaseNodeAt(int index, const char** param, const char** value)
{
    n_assert(index < this->pathLen);
    *param = nVariableServer::Instance()->GetVariableName(this->casePath[index].param);
    *value = this->casePath[index].value.Get();
}

//------------------------------------------------------------------------------
#endif
