#ifndef NC_BATCHLOADER_H
#define NC_BATCHLOADER_H
//------------------------------------------------------------------------------
/**
    @class ncBatchLoader
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Loader Entity component class for batch entities.

    (C) 2006 Conjurer Services, S.A.
*/
#include "zombieentity/ncloader.h"

//------------------------------------------------------------------------------
class ncBatchLoader : public ncLoader
{

    NCOMPONENT_DECLARE(ncBatchLoader, ncLoader);

public:
    /// constructor
    ncBatchLoader();
    /// destructor
    virtual ~ncBatchLoader();

    /// load object components
    virtual bool LoadComponents();

    /// unload object components
    virtual void UnloadComponents();

    /// component persistency
    bool SaveCmds(nPersistServer* ps);

    /// set the resource file for the batch
    void SetBatchResource(const char *);
    /// get the resource file for the batch
    const char * GetBatchResource() const;

private:

    nString resourceFile;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncBatchLoader::SetBatchResource(const char *filename)
{
    this->resourceFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
ncBatchLoader::GetBatchResource() const
{
    return this->resourceFile.IsEmpty() ? 0 : this->resourceFile.Get();
}

#endif /*NC_BATCHLOADER_H*/
