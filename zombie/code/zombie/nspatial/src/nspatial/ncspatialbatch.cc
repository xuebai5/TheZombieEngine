#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialbatch.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialbatch.h"
#include "entity/nentityobjectserver.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/nspatialvisitor.h"
#include "nspatial/nvisiblefrustumvisitor.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialBatch, ncSpatial);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialBatch)
    NSCRIPT_ADDCMD_COMPOBJECT('RSTS', void, SetTestSubentities, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGTS', bool, GetTestSubentities, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RASE', void, AddSubentity, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RASI', void, AddSubentityId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RRSE', void, RemoveSubentity, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RRSI', void, RemoveSubentityId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MCSE', void, ClearSubentities, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGSE', nEntityObject*, GetSubentity, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RUBB', void, UpdateBBox, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSMD', void, SetMinDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGMD', float, GetMinDistance, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSpatialBatch::ncSpatialBatch():
ncSpatial(),
inBeginBatch(false),
testSubentities(true),
areSubentitiesLoaded(true),
minDistance(0.f),
maxDistFactor(FLT_MAX)
{
    this->m_bbox.begin_extend();
    this->m_spatialComponentType = ncSpatial::N_SPATIAL_BATCH;
    this->m_determineSpace = ncSpatial::DS_FALSE;
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialBatch::~ncSpatialBatch()
{
    this->ClearLinks();
}

//------------------------------------------------------------------------------
/**
    update entity's transformation 
*/
void 
ncSpatialBatch::Update(const matrix44 &worldMatrix)
{
    nSpatialServer::Instance()->GetSpatialVisitor().Visit(this, worldMatrix);
}

//------------------------------------------------------------------------------
/**
    set the cell the entity is in
*/
void
ncSpatialBatch::SetCell(ncSpatialCell *cell)
{
    ncSpatial::SetCell(cell);
    for (int i = 0; i < this->subentities.Size(); i++)
    {
        this->subentities[i]->GetComponentSafe<ncSpatial>()->SetCell(cell);
    }
}

//------------------------------------------------------------------------------
/**
    add an entity to the subentities array
*/
void 
ncSpatialBatch::AddSubentity(nEntityObject* entity)
{
    // remove the subentity from the spaces where is inserted
    ncSpatial* spComp = entity->GetComponentSafe<ncSpatial>();
    if (spComp->RemoveFromSpaces())
    {
        // add the entity to the subentities array
        this->subentities.Append(entity);
        
        // set to the entity the same cell as the batch
        spComp->SetCell(this->GetCell());

        // set the entity as batched
        spComp->SetBatch(this);

        if (this->subentities.Size() == 1)
        {
            this->m_bbox.begin_extend();
        }

        // extend the bounding box of the batch with the subentity's one
        this->m_bbox.extend(spComp->GetBBox());
        this->bboxCenter  = this->m_bbox.center();
        this->bboxExtents = this->m_bbox.extents();

        if (!this->inBeginBatch)
        {
            this->RemoveFromSpaces();
            nSpatialServer::Instance()->InsertEntity(this->GetEntityObject());
        }
    }
}

//------------------------------------------------------------------------------
/**
    begin adding entities to the batch- do not update space
*/
void 
ncSpatialBatch::BeginBatch()
{
    n_assert(!this->inBeginBatch);
    this->inBeginBatch = true;
}

//------------------------------------------------------------------------------
/**
    end adding entities to the batch- update in space
*/
void 
ncSpatialBatch::EndBatch()
{
    n_assert(this->inBeginBatch);
    this->inBeginBatch = false;

    // update in space-
    this->RemoveFromSpaces();
    nSpatialServer::Instance()->InsertEntity(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
    add an entity to the subentities array using its id
*/
void 
ncSpatialBatch::AddSubentityId(nEntityObjectId entityId)
{
    this->subentityIds.Append(entityId);
    areSubentitiesLoaded = false;
}

//------------------------------------------------------------------------------
/**
    remove an entity from the subentities array using its id
*/
void 
ncSpatialBatch::RemoveSubentityId(nEntityObjectId entityId)
{
    nArray<nEntityObjectId>::iterator idIter = this->subentityIds.Find(entityId);
    if (idIter)
    {
        this->subentityIds.EraseQuick(idIter);
        areSubentitiesLoaded = !this->subentityIds.Empty();
    }
}

//------------------------------------------------------------------------------
/**
    remove an entity from the subentities array
*/
void 
ncSpatialBatch::RemoveSubentity(nEntityObject* entity)
{
    int index = this->subentities.FindIndex(entity);
    if (index != -1)
    {
        this->subentities.EraseQuick(index);
        this->UpdateBBox();
        // set the entity as not batched
        ncSpatial *spatComp = entity->GetComponentSafe<ncSpatial>();
        spatComp->SetBatch(0);
        spatComp->SetCell(0);
        nSpatialServer::Instance()->InsertEntity(entity);
    }
}

//------------------------------------------------------------------------------
/**
    update the batch's bounding box from its subentities
*/
void 
ncSpatialBatch::UpdateBBox()
{
    this->m_bbox.begin_extend();
    int i;
    for ( i = 0; i < this->subentities.Size(); ++i )
    {
        this->m_bbox.extend(this->subentities.At(i)->GetComponentSafe<ncSpatial>()->GetBBox());
    }

    this->bboxCenter  = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();
}

//------------------------------------------------------------------------------
/**
    load an entity from the array of stored ids.
*/
bool
ncSpatialBatch::LoadSubentities()
{
    if (!this->areSubentitiesLoaded)
    {
        this->BeginBatch();
        nArray<nEntityObjectId>::iterator idIter = this->subentityIds.Begin();
        while (idIter != this->subentityIds.End())
        {
            nEntityObjectId entityId = (*idIter);
            nEntityObject* entity = nEntityObjectServer::Instance()->GetEntityObject(entityId);
            if (entity)
            {
                this->AddSubentity(entity);
            }
            ++idIter;
        }
        this->EndBatch();
        this->subentityIds.Clear();
        this->areSubentitiesLoaded = true;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    load an entity from the array of stored ids.
*/
void
ncSpatialBatch::ClearSubentities()
{
    this->LoadSubentities();

    while (!this->subentities.Empty())
    {
        nEntityObject* subentity = (*this->subentities.Begin());
        this->RemoveSubentity(subentity);
    }
}

//------------------------------------------------------------------------------
/**
    recalculate the light links
*/
void 
ncSpatialBatch::CalculateLightLinks()
{
    nSpatialServer::Instance()->GetVisibilityVisitor().CalculateLightLinks(this);
}

//------------------------------------------------------------------------------
/**
    save state of the component
*/
bool 
ncSpatialBatch::SaveCmds(nPersistServer *ps)
{
    if (!ncSpatial::SaveCmds(ps))
    {
        return false;
    }

    // --- setmindistance ---
    if (!ps->Put(this->GetEntityObject(), 'RSMD', this->GetMinDistance()))
    {
        return false;
    }
    
    // --- addsubentityid ---
    int i;
    for ( i = 0; i < this->subentities.Size(); ++i )
    {
        if (!ps->Put(this->GetEntityObject(), 'RASI', this->subentities.At(i)->GetId()))
        {
            return false;
        }
    }
    
    for ( i = 0; i < this->subentityIds.Size(); ++i )
    {
        //if the batch has not been loaded, re-persist the existing ids
        if (!ps->Put(this->GetEntityObject(), 'RASI', this->subentityIds.At(i)))
        {
            return false;
        }
    }

    return true;
}
