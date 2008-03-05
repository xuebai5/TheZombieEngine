#ifndef N_MATERIALSERVER_H
#define N_MATERIALSERVER_H
//------------------------------------------------------------------------------
/**
    @class nMaterialServer
    @ingroup NebulaMaterialSystem

    @brief An interface for registering materials from the scene
    a proper implementation should be provided by the specific gfxServer
    which can render a shader from a material description and other factors.

    (C) 2004 Conjurer Services, S.A.
*/
#include "kernel/nautoref.h"
#include "kernel/ndynautoref.h"
#include "kernel/ncmdprotonativecpp.h"
#include "nmaterial/nmaterialdb.h"
#include "variable/nvariable.h"
#include "util/nmaptabletypes.h"

class nShader2;
class nGfxServer2;
class nVariableServer;
class nMaterial;
class nMaterialBuilder;
class nMaterialNode;
class nSurfaceNode;
class nShaderTree;
class nMemFile;

//------------------------------------------------------------------------------
class nMaterialServer : public nRoot
{
public:
    /// constructor
    nMaterialServer();
    /// destructor
    ~nMaterialServer();
    /// get server instance
    static nMaterialServer* Instance();
    /// open the material server, call before loading scenes
    bool Open();
    /// close the material server
    void Close();

    /// load material from file, create entry in internal database
    nMaterial* LoadMaterialFromFile(const char *);
    /// get the file for the material provided
    bool FindFileFromMaterial(nMaterial* material, nString& filename);
    /// find identical material in internal database
    nMaterial* FindSameMaterial(nMaterial* material);
    /// clone a material
    nMaterial* NewMaterialFromMaterial(nMaterial* material);

    /// load surface from file, create entry in internal database
    nSurfaceNode* LoadSurfaceFromFile(const char *);
    /// get the file for the surface provided
    bool FindFileFromSurface(nSurfaceNode* surface, nString& filename);
    /// find identical material node in internal database
    nSurfaceNode* FindSameSurface(nSurfaceNode* surface);
    /// clone a surface
    nSurfaceNode* NewSurfaceFromSurface(nSurfaceNode* surface);

    /// get access to the internal material database
    const nMaterialDb& GetMaterialDb();
    /// check surface parameters using shader or database
    void CheckSurfaceParameters(nSurfaceNode* surface);
    /// find similar surfaces by comparing textures
    bool FindSharedTextures(nSurfaceNode* surface0, nSurfaceNode* surface1);

    /// clear all material shaders
    void Clear();
    /// get shader tree for a material 
    nShaderTree *NewShaderTree(nMaterial *material, const int level, nFourCC fourcc);
    /// load material from description
    bool LoadMaterial(nMaterial *material);

    /// load saved library of material shaders
    bool LoadMaterialLibrary();
    /// save current library of material shaders
    bool SaveMaterialLibrary();
    /// get if the server is saving library
    bool IsSavingMaterialLibrary();
    /// delete saved library of material shaders
    bool DeleteMaterialLibrary();

    /// set material builder by path
    void SetBuilder(const char *);
    /// get material builder by path
    const char * GetBuilder();
    /// set material database file
    void SetDatabase(const char *);
    /// get material database file
    const char * GetDatabase();

    /// create new material (library persistence only)
    nObject* CreateMaterial(const char *);
    /// is material library dirty
    bool IsLibraryDirty();
    /// save material library
    void SaveLibrary();
    /// delete material library
    void DeleteLibrary();

    /// dump all collected information from material log to a file
    void SaveSurfaceReport(const char *);
    /// clear all collected information from material log
    void ClearSurfaceReport();

    #ifndef NGAME
    /// enable/disable overriding materials for debug
    void SetDebugMaterialEnabled(bool);
    bool GetDebugMaterialEnabled();
    /// set the material to use for overriding
    void SetDebugMaterialFile(const char *);
    const char * GetDebugMaterialFile();
    /// enable/disable overriding surfaces for debug
    void SetDebugSurfaceEnabled(bool);
    bool GetDebugSurfaceEnabled();
    /// set the surface to use for overriding
    void SetDebugSurfaceFile(const char *);
    const char * GetDebugSurfaceFile();
    #endif

private:
    static nMaterialServer* Singleton;
    bool isOpen;
    bool isLibraryDirty;
    bool isSavingLibrary;
    bool isLoadingLibrary;
    nString builderName;
    nRef<nRoot> refShaders;
    nDynAutoRef<nMaterialBuilder> refMaterialBuilder;
    nMaterialDb materialDb;

    struct MaterialEntry
    {
        nRef<nMaterial> refMaterial;
    };
    nMapTableTypes<MaterialEntry>::NString materialNameMap;

    struct SurfaceEntry
    {
        nRef<nSurfaceNode> refSurface;
    };
    nMapTableTypes<SurfaceEntry>::NString surfaceNameMap;

    static int uniqueMaterialId;
    static int uniqueSurfaceId;

    #ifndef NGAME
    bool debugMaterialEnabled;
    bool debugSurfaceEnabled;
    nString debugMaterialFile;
    nString debugSurfaceFile;
    #endif

    nString currentSurfaceFile;
    nMemFile *surfaceReportFile;
};

//------------------------------------------------------------------------------
/**
*/
inline
nMaterialServer*
nMaterialServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nMaterialDb&
nMaterialServer::GetMaterialDb()
{
    n_assert(this->isOpen);
    return this->materialDb;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMaterialServer::IsSavingMaterialLibrary()
{
    return this->isSavingLibrary;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialServer::SetDebugMaterialEnabled(bool enabled)
{
    this->debugMaterialEnabled = enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMaterialServer::GetDebugMaterialEnabled()
{
    return this->debugMaterialEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialServer::SetDebugMaterialFile(const char *filename)
{
    this->debugMaterialFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nMaterialServer::GetDebugMaterialFile()
{
    return this->debugMaterialFile.IsEmpty() ? 0 : this->debugMaterialFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nMaterialServer::GetDebugSurfaceEnabled()
{
    return this->debugSurfaceEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialServer::SetDebugSurfaceEnabled(bool enabled)
{
    this->debugSurfaceEnabled = enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nMaterialServer::SetDebugSurfaceFile(const char *filename)
{
    this->debugSurfaceFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nMaterialServer::GetDebugSurfaceFile()
{
    return this->debugSurfaceFile.IsEmpty() ? 0 : this->debugSurfaceFile.Get();
}
#endif /*NGAME*/

//------------------------------------------------------------------------------
#endif /*N_MATERIALSERVER_H*/
