#ifndef NC_LOADERCLASS_H
#define NC_LOADERCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncLoaderClass
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Entity component class for asset loaders.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncLoaderClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncLoaderClass,nComponentClass);

public:
    /// constructor
    ncLoaderClass();
    /// destructor
    virtual ~ncLoaderClass();
    /// component class persistence
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// are resources valid
    virtual bool AreResourcesValid();

    /// signal when an entity of this class has been loaded
    virtual void EntityLoaded(nEntityObject*);
    /// signal when an entity of this class has been unloaded
    virtual void EntityUnloaded(nEntityObject*);

    /// get number of loaded entities of this class
    int GetNumLoadedEntities() const;
    /// set resources retained
    void SetRetainResources(bool retain);
    /// get resources retained
    bool GetRetainResources() const;

    /// set resource file
    void SetResourceFile(const char *);
    /// get resource file
    const char * GetResourceFile() const;
    /// set async enabled
    void SetAsyncEnabled(bool);
    /// get async enabled
    bool GetAsyncEnabled() const;
    /// force reloading class resources (unloads resources)
    void Invalidate();
    /// force reinitialization, without unloading resources
    void Refresh();

    /// load node from file, or addref if it already exists
    static nRoot *LoadResource(nRoot *root, const nString& resourceFile);

#ifndef NGAME
    void SetDefaultResourceFile();
#endif

protected:

    /// save as previous one, with a given node name
    nRoot *LoadResourceAs(nRoot *root, const nString& resourceFile, const nString& name);

    /// hot reload all scene resources and entities
    virtual void ReloadResources();

private:
    nString resourceFile;
    bool asyncEnabled;
    int numLoadedEntities;
    bool retainResources;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncLoaderClass::SetResourceFile(const char* fileName)
{
    if (this->resourceFile != fileName)
    {
        this->resourceFile = fileName;
        this->ReloadResources();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
ncLoaderClass::GetResourceFile() const
{
    return this->resourceFile.IsEmpty() ? 0 : this->resourceFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncLoaderClass::SetAsyncEnabled(bool asyncEnabled)
{
    this->asyncEnabled = asyncEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncLoaderClass::GetAsyncEnabled() const
{
    return this->asyncEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncLoaderClass::GetNumLoadedEntities() const
{
    return this->numLoadedEntities;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncLoaderClass::SetRetainResources(bool value)
{
    this->retainResources = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncLoaderClass::GetRetainResources() const
{
    return this->retainResources;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
inline
void
ncLoaderClass::SetDefaultResourceFile()
{
    nString tmp = "wc:export/assets/";
    tmp += this->GetEntityClass()->GetName();
    this->SetResourceFile(tmp.Get());
}
#endif

//------------------------------------------------------------------------------
#endif
