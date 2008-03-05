#ifndef N_RESOURCEPOOL_H
#define N_RESOURCEPOOL_H
//------------------------------------------------------------------------------
/**
    @class nResourcePool
    @ingroup NebulaResourceSystem
    @brief class to encapsulate resource pool access through a name list

    (C) 2005 Conjurer Services, S.A.
*/
#include "resource/nresource.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------

class nResourcePool
{
public:
    /// constructor
    nResourcePool(const char *rsrcPath);
    /// destructor
    ~nResourcePool();
    /// get list head
    nRoot* GetResourcePool();
    /// return first element of list
    nResource* GetHead();
    /// create a resource object
    nResource* NewResource(const char* className, const char* rsrcName);
    /// find resource by name id
    nResource* Find(const char* rsrcName);
    /// get number of resources
    int GetNumResources();
    /// generate a valid resource id from a resource path
    char* NewResourceId(const char* rsrcName, char* buf, int bufSize);
    /// remove all resource nodes from map table
    void RemoveAllResourceNodes();

private:
    struct ResourceNode
    {
        nRef<nResource> refResource;
    };
    int uniqueId;
    nRef<nRoot> refPool;
    nMapTableTypes<ResourceNode>::NString rsrcNameMap;
};

//------------------------------------------------------------------------------
/**
*/
inline
nRoot*
nResourcePool::GetResourcePool()
{
    n_assert(this->refPool.isvalid());
    return this->refPool.get();
}

//------------------------------------------------------------------------------
#endif
