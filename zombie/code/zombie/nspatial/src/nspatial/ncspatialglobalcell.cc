#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialglobalcell.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialglobalcell.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialmc.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialGlobalCell, ncSpatialCell);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialGlobalCell)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSpatialGlobalCell::ncSpatialGlobalCell()
{
    this->m_cellType = ncSpatialCell::N_GLOBAL_CELL;
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialGlobalCell::~ncSpatialGlobalCell()
{
}

//------------------------------------------------------------------------------
/**
    adds an entity to the cell
*/
bool 
ncSpatialGlobalCell::AddEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    int category = -1;
    if (spatialComp)
    {
        category = spatialComp->GetSpatialCategory();
    }
    else
    {
        category = nSpatialServer::Instance()->GetCategory(entity);
    }
    

    n_assert2((category > -1), 
              "miquelangel.rujula: trying to insert an entity in the spatial system that is uncategorized!");
    if (category < 0)
    {
        return false;
    }

    if (!this->m_categories[category].Find(entity))
    {
        this->m_categories[category].Append(entity);
        entity->AddRef();
        ncSpatial *spatialComp  = entity->GetComponent<ncSpatial>();
        if (spatialComp)
        {
            spatialComp->SetCell(this);
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell
*/
bool 
ncSpatialGlobalCell::RemoveEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    int category = -1;
    if (spatialComp)
    {
        category = spatialComp->GetSpatialCategory();
    }
    else
    {
        category = nSpatialServer::Instance()->GetCategory(entity);
    }
   
    n_assert2((category > -1), 
              "miquelangel.rujula: trying to insert an entity in the spatial system that is uncategorized!");
    if (category < 0)
    {
        return false;
    }

    int index = this->m_categories[category].FindIndex(entity);
    if (index == -1)
    {
        return false;
    }
    this->m_categories[category].EraseQuick(index);

    if (spatialComp)
    {
        if (spatialComp->GetType() == ncSpatial::N_SPATIAL_MC)
        {
            (static_cast<ncSpatialMC*>(spatialComp))->RemoveCell(this);
        }
        else
        {
            spatialComp->SetCell(0);
        }
    }
    entity->Release();

    return true;
}


