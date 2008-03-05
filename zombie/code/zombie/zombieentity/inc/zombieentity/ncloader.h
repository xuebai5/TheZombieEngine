#ifndef NC_LOADER_H
#define NC_LOADER_H
//------------------------------------------------------------------------------
/**
    @class ncLoader
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Base Entity component class to load the asset data for a
    specific kind of entity.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncSpatialIndoor;
class ncSpatialSpace;

//------------------------------------------------------------------------------
class ncLoader : public nComponentObject
{

    NCOMPONENT_DECLARE(ncLoader, nComponentObject);

public:
    /// constructor
    ncLoader();
    /// destructor
    virtual ~ncLoader();
    /// component initialization
    virtual void InitInstance(nObject::InitInstanceMsg initType);

    /// is a resource loading request pending?
    bool IsPending();

    /// load object components
    virtual bool LoadComponents();
    /// unload object components
    virtual void UnloadComponents();
    /// check if object components are valid
    virtual bool AreComponentsValid();

    #ifndef NGAME
    /// reload entity resources
    void SetReload(bool dummy);
    bool GetReload() const;
    #endif

protected:

    friend class nAsyncLoaderServer;

    /// load indoor space component from asset
    bool InitIndoorSpace(ncSpatialIndoor *indoorSpace);
    /// load indoor space entity resources
    bool LoadIndoorSpace(ncSpatialIndoor *indoorSpace);
    /// unload indoor space entity resources
    void UnloadIndoorSpace(ncSpatialIndoor *indoorSpace);
    /// load outdoor space entity resources
    bool LoadOutdoorSpace(ncSpatialSpace *spaceComp);
    /// unload outdoor space entity resources
    void UnloadOutdoorSpace(ncSpatialSpace *spaceComp);

    nNode jobNode; //for linkage into loader server loader job list
    bool areComponentsValid;
    bool isCharacterValid;

};

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncLoader::AreComponentsValid()
{
    return this->areComponentsValid;
}

//------------------------------------------------------------------------------
/**
    Return whether a yet-unfullfilled resource loading request is pending.

    @return     pending flag (true in async mode between Load() and IsValid() = true)
*/
inline
bool
ncLoader::IsPending()
{
    return this->jobNode.IsLinked();
}

//------------------------------------------------------------------------------
#endif
