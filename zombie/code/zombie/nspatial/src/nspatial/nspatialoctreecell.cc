#include "precompiled/pchnspatial.h"
//-----------------------------------------------------------------------------
//  nspatialoctreecell.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "nspatial/nspatialoctreecell.h"
#include "entity/nentity.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialcamera.h"

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialOctreeCell::~nSpatialOctreeCell()
{
    // destroy subcells
    nSpatialOctreeCell *octreeCell;
    for (nArray<nSpatialOctreeCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                               pCell != this->m_cellsArray.End();
                                               pCell++)
    {
        octreeCell = static_cast<nSpatialOctreeCell*>((*pCell));
        octreeCell->DestroyCell();
        n_delete(octreeCell);
    }

    this->m_cellsArray.Reset();
}

//------------------------------------------------------------------------------
/**
    Removes all the entities in this cell.
    Call it just before deleting the space.
*/
void 
nSpatialOctreeCell::DestroyCell()
{
    // remove all the entities and all its associated octree elements
    nOctElement *oe, *next;
    oe = (nOctElement *) this->m_octreeNode->elm_list.GetHead();
    bool result;
    while(oe)
    {
        nEntityObject *entity = static_cast<nEntityObject*>(oe->GetPtr());
        result = ncSpatialCell::RemoveEntity(entity);
        n_assert2(result, "miquelangel.rujula");
        next = (nOctElement *) oe->GetSucc();
        this->m_octreeNode->DestroyElement(oe);
        oe = next;
    }
}

//------------------------------------------------------------------------------
/**
    adds an entity to the cell
*/
bool  
nSpatialOctreeCell::AddEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to entity!");

    if (!entity->GetComponent<ncSpatialCamera>())
    {
        nOctElement *newOctElement = n_new(nOctElement);
        ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
        n_assert2(spatialComp, 
                "miquelangel.rujula: trying to insert an entity without spatial component in an octree space!");

        bbox3 box = spatialComp->GetBBox();
        vector3 pos = box.center();
        float radius = box.extents().len();
        // set entity's center and radius
        newOctElement->Set(pos, radius);
        // set pointer to the entity
        newOctElement->SetPtr(entity);
        // set pointer back to the octree element
        //spatialComp->SetAuxPtr(newOctElement);
        spatialComp->SetOctElement(newOctElement);
        // insert the new octree element in the octree node
        this->m_octreeNode->AddElm(newOctElement);
    }
    
    return ncSpatialCell::AddEntity(entity);
}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell 
*/
bool 
nSpatialOctreeCell::RemoveEntity(nEntityObject *entity)
{
    if (!entity->GetComponent<ncSpatialCamera>())
    {
        nOctElement *oe;
        nEntityObject *octEntity = 0;
        ncSpatial *spatialComp = 0;
        for (oe = (nOctElement *) this->m_octreeNode->elm_list.GetHead();
            oe;
            oe = (nOctElement *) oe->GetSucc())
        {
            octEntity = static_cast<nEntityObject*>(oe->GetPtr());
            if (octEntity == entity)
            {   
                // entity found in the octree node. Proceed to remove it.
                if (ncSpatialCell::RemoveEntity(entity))
                {
                    spatialComp = entity->GetComponent<ncSpatial>();
                    //spatialComp->SetAuxPtr(NULL);
                    spatialComp->SetOctElement(0);
                    this->m_octreeNode->RemoveElement(oe);
                    n_delete(oe);
                    return true;
                }
            }
        }
    }
    else
    {
        return ncSpatialCell::RemoveEntity(entity);
    }

    return false;
}

