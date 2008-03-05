#ifndef N_VISIBLEENTITYARRAY_H
#define N_VISIBLEENTITYARRAY_H
//------------------------------------------------------------------------------
/**
    @class nVisibleEntityArray
    @ingroup NebulaSpatialSystem
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief nVisibleEntityArray stores an array of visible entities with
    level of detail information, and keeps the set of visible entities
    for a spatial camera from one frame to the next to keep track of
    the previous level of detail in a camera visibility context.
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "util/narray.h"
#include "nspatial/ncspatial.h"
#include "nscene/ncscene.h"

//------------------------------------------------------------------------------
class nVisibleEntityArray
{
public:
    /// constructor
    nVisibleEntityArray();
    /// destructor
    ~nVisibleEntityArray();
    
    /// get read access to the internal array
    const nArray<ncSpatial::VisibleEntity>& Get();
    /// reset the array, clearing all entries
    void Reset();
    /// reset the array, invalidating all entries
    void BeginUpdate();
    /// end adding entities to the array, delete all invalid entries
    void EndUpdate();

    /// find if it exists the entry for a visible entity
    ncSpatial::VisibleEntity* Find(nEntityObject* entity);
    /// append an entity, return reference to the entry
    ncSpatial::VisibleEntity& Append(nEntityObject* entity);
    /// append an spatial comp, return reference to the entry
    ncSpatial::VisibleEntity& Append(ncSpatial* spatialComp);

private:
    nArray<ncSpatial::VisibleEntity> visibleEntities;
    nProfiler profVisVisEntityArray;
    nProfiler profVisVisEntityArrayBegin;
    nProfiler profVisVisEntityArrayAppend;
    nProfiler profVisVisEntityArrayEnd;
};

//------------------------------------------------------------------------------
/**
*/
inline
nVisibleEntityArray::nVisibleEntityArray():
profVisVisEntityArray("profVis_VisEntityArray", true),
profVisVisEntityArrayBegin("profVis_VisEntityArray_Begin", true),
profVisVisEntityArrayAppend("profVis_VisEntityArray_Append", true),
profVisVisEntityArrayEnd("profVis_VisEntityArray_End", true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nVisibleEntityArray::~nVisibleEntityArray()
{
    this->visibleEntities.Reset();
}

//------------------------------------------------------------------------------
/**
*/
inline
const nArray<ncSpatial::VisibleEntity>& 
nVisibleEntityArray::Get()
{
    return this->visibleEntities;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVisibleEntityArray::Reset()
{
    this->visibleEntities.Reset();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVisibleEntityArray::BeginUpdate()
{
    profVisVisEntityArray.StartAccum();
    profVisVisEntityArrayBegin.StartAccum();
    ncSpatial::VisibleEntity* visEntity = this->visibleEntities.Begin();
    while (visEntity != this->visibleEntities.End())
    {
        visEntity->SetValid(false);
        ++visEntity;
    }
    profVisVisEntityArrayBegin.StopAccum();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nVisibleEntityArray::EndUpdate()
{
    profVisVisEntityArrayEnd.StartAccum();
    ncSpatial::VisibleEntity* visEntity = this->visibleEntities.Begin();
    while (visEntity != this->visibleEntities.End())
    {
        if (!visEntity->IsValid() || !visEntity->GetEntity())
        {
            visEntity = this->visibleEntities.Erase(visEntity);
        }
        else
        {
            // copy the visibility flags from the entity
            ncSpatial* spatialComp = visEntity->GetEntity()->GetComponent<ncSpatial>();
            if (spatialComp)
            {
                visEntity->SetFlag(ncScene::ShapeVisible, spatialComp->IsShapeVisible());
                visEntity->SetFlag(ncScene::ShadowVisible, spatialComp->IsShadowVisible());
                spatialComp->SetShapeVisible(false);
                spatialComp->SetShadowVisible(false);
            }
            else
            {
                // HACK- this is to handle terrain cells
                visEntity->SetFlag(ncScene::ShapeVisible, true);
            }
            ++visEntity;
        }
    }
    profVisVisEntityArrayEnd.StopAccum();
    profVisVisEntityArray.StopAccum();
}

//------------------------------------------------------------------------------
/**
*/
inline
ncSpatial::VisibleEntity*
nVisibleEntityArray::Find(nEntityObject* entity)
{
    ncSpatial::VisibleEntity* visEntity = this->visibleEntities.Begin();
    while (visEntity != this->visibleEntities.End())
    {
        if (entity == visEntity->GetEntity())
        {
            return visEntity;
        }
        ++visEntity;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
ncSpatial::VisibleEntity&
nVisibleEntityArray::Append(nEntityObject* entity)
{
    profVisVisEntityArrayAppend.StartAccum();
    //ncSpatial::VisibleEntity* visEntity = this->Find(entity);
    //if (visEntity)
    //{
    //    visEntity->SetValid(true);
    //    profVisVisEntityArrayAppend.StopAccum();
    //    return (*visEntity);
    //}

    this->visibleEntities.Append(entity);
    profVisVisEntityArrayAppend.StopAccum();
    return this->visibleEntities.Back();
}

//------------------------------------------------------------------------------
/**
*/
inline
ncSpatial::VisibleEntity&
nVisibleEntityArray::Append(ncSpatial* spatialComp)
{
    profVisVisEntityArrayAppend.StartAccum();
    //ncSpatial::VisibleEntity* visEntity = this->Find(spatialComp->GetEntityObject());
    //if (visEntity)
    //{
    //    visEntity->SetCurrentLod(spatialComp->GetCurrentLod());
    //    visEntity->SetValid(true);
    //    profVisVisEntityArrayAppend.StopAccum();
    //    return (*visEntity);
    //}

    this->visibleEntities.Append(spatialComp);
    profVisVisEntityArrayAppend.StopAccum();
    return this->visibleEntities.Back();
}

//------------------------------------------------------------------------------
#endif
