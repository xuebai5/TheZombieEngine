#ifndef N_SCENESERVER_H
#define N_SCENESERVER_H
//------------------------------------------------------------------------------
/**
    @class nSceneServer
    @ingroup SceneServers
    @brief The scene server controls operation for scene rendering.

    It acts as a scene graph factory and manager, 
    manages a set of scene passes, and exposes an interface for
    application-customized rendering.

    (C) 2004 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/nprofiler.h"
#include "misc/nwatched.h"
#include "util/nmaptabletypes.h"
#include "nscene/nsceneshader.h"

#define N_OUTPUTDEBUGSTRING(x) //OutputDebugString(x)

class nSceneGraph;
class nShader2;
class nShaderTree;

//------------------------------------------------------------------------------
class nSceneServer : public nRoot
{
public:
    /// constructor
    nSceneServer();
    /// destructor
    virtual ~nSceneServer();
    /// get instance pointer
    static nSceneServer *Instance();
    
    /// open the scene server
    bool Open();
    /// close the scene server;
    void Close();
    /// return true if scene server open
    bool IsOpen() const;
    /// set background color
    void SetBgColor(const vector4& c);
    /// get background color
    const vector4& GetBgColor() const;

    /// @name scene passes and types
    //@{
    /// get pass by index
    int GetNumPasses();
    /// get pass by index
    nFourCC GetPassAt(const int index);
    /// register a pass and get its index
    int GetPassIndexByFourCC(nFourCC fourcc);
    /// get number of selector types
    int GetNumSelectorTypes();
    /// set priority by selector type
    void SetPriorityBySelectorType(nFourCC fourcc, int priority);
    /// set priority by selector type
    int GetPriorityBySelectorType(nFourCC fourcc);
    /// set priority by selector type (scripting)
    void SetPriorityBySelectorType(const char *, int priority);
    /// set priority by selector type
    int GetPriorityBySelectorType(const char *);
    //@}
    
    /// @name scene graph management
    //@{
    /// create a new scene graph
    nSceneGraph *NewSceneGraph(const char *name);
    /// attach a scene graph
    void AddGraph(nSceneGraph *sceneGraph);
    /// remove a scene graph
    void RemoveGraph(nSceneGraph *sceneGraph);
    /// get number of scene graphs
    int GetNumGraphs();
    /// get scene graph by index
    nSceneGraph *GetGraphAt(const int index);
    //@}

    /// @name scene rendering
    //@{
    /// begin building scene graph(s) for rendering
    virtual bool BeginScene();
    /// finish building scene graph(s) for rendering
    virtual void EndScene();
    /// render the scene
    virtual void RenderScene();
    /// present the scene
    virtual void PresentScene();
    //@}

    /// @name shader database
    //@{
    /// set filename for shader database
    void SetShaderDatabaseFilename(const char* filename);
    /// get filename for shader database
    const char *GetShaderDatabaseFilename();
    /// clear all entries in the shader database
    void ClearShaders();
    /// find shader entry by shader alias
    int FindShader(const char *shaderAlias);
    /// find shader entry by shader file and technique
    int FindShader(const char *filename, const char* technique);
    /// find shader entry by shader object and technique
    int FindShader(nShader2* shader, const char* technique = 0);
    /// add a shader entry
    int AddShader(nSceneShader& sceneShader);
    /// get number of shader entries
    int GetNumShaders();
    /// load shader database filename
    void LoadShaders();
    /// get shader entry by index
    nSceneShader& GetShaderAt(int shaderIndex);
    
    /// create a shader tree
    nShaderTree* NewShaderTree(const char *name);
    /// find a shader tree by name
    nShaderTree* FindShaderTree(const char *name);

    /// set if shaders are loaded on demand
    void SetLoadShadersOnDemand(bool value);
    /// get if shaders are loaded on demand
    bool GetLoadShadersOnDemand() const;

    /// @todo save current shader database
    //void SaveShaders(const char* xmlFilename);
    #ifndef NGAME
    /// error shader for invalid shaders
    nSceneShader& GetErrorShader();
    #endif
    //@}
    
private:
    static nSceneServer *Singleton;

protected:
    nArray<nFourCC> passes;
    nArray<nSceneGraph*> refGraphArray;
    nRef<nRoot> graphPool;
    bool isOpen;
    bool inBeginScene;
    vector4 bgColor;

    /// filename for the shader database
    nString shaderDatabaseFilename;
    /// indexed shader database (temporary)
    nArray<nSceneShader> shaderArray;

    typedef nRef<nShaderTree> RefShaderTree;

    /// indexed map of shader trees
    nMapTableTypes<RefShaderTree>::NString shaderTreeMap;

    /// indexed array of light priorities by selector type
    nKeyArray<int> priorityByType;

    /// shaders are loaded immediately, or on demand
    bool loadShadersOnDemand;

    #if __NEBULA_STATS__
    nProfiler profPresentScene;
    nWatched watchNumShaders;
    nWatched watchNumValidShaders;
    int statsNumValidShaders;
    friend class nSceneShader;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nSceneServer *
nSceneServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneServer::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetBgColor(const vector4& c)
{
    this->bgColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nSceneServer::GetBgColor() const
{
    return this->bgColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSceneServer::GetNumPasses()
{
    return this->passes.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nFourCC
nSceneServer::GetPassAt(const int index)
{
    return this->passes[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSceneServer::GetNumSelectorTypes()
{
    return this->priorityByType.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSceneServer::GetNumGraphs()
{
    return this->refGraphArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nSceneGraph*
nSceneServer::GetGraphAt(const int index)
{
    return (nSceneGraph*) this->refGraphArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSceneServer::SetLoadShadersOnDemand(bool value)
{
    this->loadShadersOnDemand = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSceneServer::GetLoadShadersOnDemand() const
{
    return this->loadShadersOnDemand;
}

//------------------------------------------------------------------------------
#endif
