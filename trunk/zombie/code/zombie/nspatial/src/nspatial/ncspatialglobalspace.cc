#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialglobalspace.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialglobalspace.h"

//NSIGNAL_DEFINE(ncSpatialGlobalSpace, set...);

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialGlobalSpace, ncSpatialSpace);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialGlobalSpace)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSpatialGlobalSpace::ncSpatialGlobalSpace()
{
    this->m_spaceType = ncSpatialSpace::N_GLOBAL_SPACE;
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialGlobalSpace::~ncSpatialGlobalSpace()
{
    // do nothing
}

//------------------------------------------------------------------------------
/**
    add an always visible entity
*/
bool
ncSpatialGlobalSpace::RemoveEntity(nEntityObject *entity)
{
    int index(this->alwaysVisEntities.FindIndex(entity));

    if (index != -1)
    {
        this->alwaysVisEntities.EraseQuick(index);
        return true;
    }

    return ncSpatialSpace::RemoveEntity(entity);
}

//------------------------------------------------------------------------------
/**
    add an always visible entity
*/
void
ncSpatialGlobalSpace::AddAlwaysVisibleEntity(nEntityObject *entity)
{
    n_assert_return(this->alwaysVisEntities.FindIndex(entity) == -1, );

    this->alwaysVisEntities.Append(entity);
}

//------------------------------------------------------------------------------
/**
    removes an always visible entity
*/
bool
ncSpatialGlobalSpace::RemoveAlwaysVisibleEntity(nEntityObject *entity)
{
    int index(this->alwaysVisEntities.FindIndex(entity));

    if ( index != -1 )
    {
        this->alwaysVisEntities.EraseQuick(index);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialGlobalSpace::SaveCmds(nPersistServer *ps)
{
    if (ncSpatialSpace::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

