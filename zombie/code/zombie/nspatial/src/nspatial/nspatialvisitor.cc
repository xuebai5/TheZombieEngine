#include "precompiled/pchnspatial.h"
//--------------------------------------------------
// nspatialvisitor.cc
// (C) 2004 Conjurer Services, S.A.
// @author Miquel Angel Rujula <>
//--------------------------------------------------

#include "nspatial/nspatialvisitor.h"
#include "nspatial/ncspatialcell.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/nspatialcollector.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialoccluder.h"
#include "nspatial/ncspatialcell.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialoctree.h"
#include "nspatial/nspatialoctreecell.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatiallightenv.h"
#include "nspatial/nlightregion.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "mathlib/bbox.h"
#include "mathlib/vector.h"
#include "mathlib/sphere.h"
#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
/**
    Search the cell containing the point in all the spaces registered in the
    spatial server.
    If the point is only in the outdoors, it returns the quadtree space's 
    root cell.
*/
ncSpatialCell *
nSpatialVisitor::SearchCell(const vector3 &point) const
{
    ncSpatialCell *cell;

    // this array has all the spaces that can contain the spatial element
    static nArray<nEntityObject*> possibleSpaces;
    possibleSpaces.Reset();
    
    // first we look if it's in any indoor space
    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_spatialServer->m_indoorSpaces.Begin();
                                            pIndoorSpace != this->m_spatialServer->m_indoorSpaces.End();
                                            pIndoorSpace++)
    {
        cell = (*pIndoorSpace)->GetComponentSafe<ncSpatialSpace>()->SearchCellContaining(point, 0);
        if (cell)
        {
            return cell;
        }
    }

    // any indoor space contains the element. Let's see if the outdoor space does it.
    if (this->m_spatialServer->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * outdoorSpace = this->m_spatialServer->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();
        if (outdoorSpace->Contains(point))
        {
            return outdoorSpace->GetRootCell();
        }
    }
  
    // finally, if the spatial server is using octrees, check if the octree contains it
    if (this->m_spatialServer->m_useOctrees && this->m_spatialServer->m_octreeSpace.isvalid())
    {
        ncSpatialOctree *octreeSpace = this->m_spatialServer->m_octreeSpace->GetComponentSafe<ncSpatialOctree>();

        if (octreeSpace->Contains(point))
        {
            return octreeSpace->GetRootCell();
        }
    }

    // the element it's outside all the spaces
    return NULL;
}

//------------------------------------------------------------------------------
/**
    move an entity from a cell to another one. The entity's spatial component 
    contains the origin cell
*/
bool 
nSpatialVisitor::MoveEntity(nEntityObject *entity, ncSpatialCell *destCell)
{
    n_assert(entity);
    n_assert(destCell);

    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComponent, "Error: moving entity hasn't spatial component!");
    ncSpatialCell *origCell = spatialComponent->GetCell();
        
    if (origCell && !origCell->RemoveEntity(entity))
    {
        return false;
    }

    return destCell->AddEntity(entity);
}

//------------------------------------------------------------------------------
/**
*/
void 
nSpatialVisitor::Visit(ncSpatial *visitee, const matrix44 &newTransform)
{
#if __NEBULA_STATS__
    this->profSpa_Visit.StartAccum();
#endif // __NEBULA_STATS__

//    if ( !visitee->GetCell() 
//#ifndef NGAME
//        || nSpatialServer::Instance()->GetExecutionMode() == nSpatialServer::EDITOR 
//#endif // !NGAME
//        )
    {
        this->VisitEdition(visitee, newTransform);
    }
//    else
//    {
//        this->VisitGame(visitee, newTransform);
//    }

#if __NEBULA_STATS__
    this->profSpa_Visit.StopAccum();
#endif // __NEBULA_STATS__
}

//#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void 
nSpatialVisitor::VisitEdition(ncSpatial *visitee, const matrix44 &newTransform)
{
    n_assert2(visitee, "Error: NULL pointer to spatial component!");
    n_assert(!visitee->GetComponent<ncSpatialPortal>());

    // update component's bounding box
    visitee->UpdateBBox(newTransform);

    // if we don't want to determine if the visitee has changed its cell or space, just return
    if (!visitee->DetermineSpace())
    {
        // update light links
        if ( visitee->GetType() != ncSpatial::N_SPATIAL_LIGHT )
        {
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(visitee);
        }
        return;
    }

    ncSpatialCell *cell = visitee->GetCell();
    ncSpatialCell *newCell;
    if (cell)
    {
        ncSpatialSpace *space = cell->GetParentSpace();
        if (space->GetType() == ncSpatialSpace::N_INDOOR_SPACE)
        {
            newCell = space->SearchCellContaining(visitee->GetBBoxCenter(), 0);
            
            if (newCell)
            {
                // the element hasn't left the space it was before
                ncSpatialLight* lightComp = visitee->GetComponent<ncSpatialLight>();
                if ((newCell != cell) || lightComp)
                {
                    // but it has moved to another cell
                    space->MoveEntity(visitee->GetEntityObject(), newCell);
                }
                
                if ( !lightComp )
                {
                    // update light links
                    this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(visitee);
                }

                n_assert(visitee->GetCell());
                return;
            }
        }

        // the entity has left the cell it was before. Let's see if it's traversing a portal
        ncSpatialPortal* contPortal = cell->IsInPortals(visitee->GetBBoxCenter());
            
        if (!contPortal)
        {
            // the entity has left the space it was before or it's in the outdoor
            // remove it from its current space and insert it in its corresponding cell
            n_verify(cell->RemoveEntity(visitee->GetEntityObject()));

            // if it's a facade, just insert it in the outdoor or else in the global space
            if ( visitee->GetIndoorSpace() )
            {
                if ( !nSpatialServer::Instance()->GetQuadtreeSpace()->AddEntity(visitee->GetEntityObject()) )
                {
                    nSpatialServer::Instance()->InsertGlobalEntity(visitee->GetEntityObject());
                }
            }
            else
            {
                nSpatialServer::Instance()->InsertEntity(visitee->GetEntityObject());
            }
        }
    }
    else
    {
        // the entity wasn't inserted yet, let's do it now
        nSpatialServer::Instance()->InsertEntity(visitee->GetEntityObject());
    }

    // update light links
    if ( visitee->GetType() != ncSpatial::N_SPATIAL_LIGHT )
    {
        this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(visitee);
    }

    n_assert(visitee->GetCell());
}
//#endif // !NGAME

//------------------------------------------------------------------------------
/**
*/
void 
nSpatialVisitor::VisitGame(ncSpatial *visitee, const matrix44 &newTransform)
{
    n_assert2(visitee->GetType() != ncSpatial::N_SPATIAL_PORTAL, 
        "Trying to move a spatial portal in game mode!");
    
    // update component's bounding box
    visitee->UpdateBBox(newTransform);

    // if we don't want to determine if the visitee has changed its cell or space, 
    // just return
    if (!visitee->DetermineSpace())
    {
        // update light links
        if ( visitee->GetType() != ncSpatial::N_SPATIAL_LIGHT )
        {
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(visitee);
        }
        return;
    }

    ncSpatialCell *cell = visitee->GetCell();

#ifndef NGAME
    if ( !cell )
    {
        return;
    }
#endif // !NGAME

    // check if the entity has traversed any portal
    bool traversedPortal(this->CheckPortalTraversing(visitee, cell));

    if ( !traversedPortal && (cell->GetType() == ncSpatialCell::N_QUADTREE_CELL) )
    {
        // the entity hasn't traversed any portal and it's in the outdoor
        const bbox3 &box = visitee->GetBBox();
        if ( cell->Contains(box) )
        {
            // it's in the same cell it was before. Let's see if the entity has moved to any of its child
            // cells
            ncSpatialQuadtreeCell *quadCell = static_cast<ncSpatialQuadtreeCell*>(cell);
            ncSpatialCell *newCell = static_cast<ncSpatialQuadtree*>(cell->GetParentSpace())->SearchCellContaining2D(quadCell, box);
            n_assert(newCell);
            cell->GetParentSpace()->MoveEntity(visitee->GetEntityObject(), newCell);
        }
        else
        {
            // the entity isn't in the same cell as before. Let's resituate it
            n_verify(cell->RemoveEntity(visitee->GetEntityObject()));
            n_verify(nSpatialServer::Instance()->GetQuadtreeSpace()->AddEntity(visitee->GetEntityObject()));
        }
    }

    visitee->SetPreviousPos(visitee->GetBBoxCenter());

    // update light links
    if ( visitee->GetType() != ncSpatial::N_SPATIAL_LIGHT )
    {
        this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(visitee);
    }

    n_assert(visitee->GetCell());
}

//------------------------------------------------------------------------------
/**
    checks if the given spatial component has traversed any portal and resituates
    the entity if necessary
*/
bool 
nSpatialVisitor::CheckPortalTraversing(ncSpatial *spatialComp, ncSpatialCell *cell)
{
    float dist(-1.0f);
    const vector3 &pos = spatialComp->GetBBoxCenter();
    const nArray<nEntityObject*> &portalsArray = cell->GetCategory(nSpatialTypes::CAT_PORTALS);
    ncSpatialPortal* portal = 0;
    for (int i(0); i < portalsArray.Size(); ++i)
    {
        portal = portalsArray[i]->GetComponentSafe<ncSpatialPortal>();
        const plane &portalPlane = portal->GetPlane();
        
        dist = portalPlane.distance(pos);

        if ( dist < 0.0f )
        {
            // if the entity is in the same side of the plane it means that it can't
            // have traversed the portal
            if (portalPlane.distance(spatialComp->GetPreviousPos()) >= 0.0f)
            {
                // We are behind the portal's plane. Check if we have traversed it.
                // Test the two triangles forming the clip rectangle
                const vector3 *clipRect = portal->GetVertices();

                if (this->IntersectsTriangle(spatialComp->GetPreviousPos(), 
                                             spatialComp->GetBBoxCenter(),
                                             clipRect[0],
                                             clipRect[1],
                                             clipRect[2]) 
                                             ||
                    this->IntersectsTriangle(spatialComp->GetPreviousPos(), 
                                             spatialComp->GetBBoxCenter(),
                                             clipRect[2],
                                             clipRect[3],
                                             clipRect[0]))
                {
                    // The camera has traversed the portal. Resituate the camera in the next cell
                    ncSpatialCell *otherSideCell = portal->GetOtherSideCell();
                    n_assert(otherSideCell);
                    
                    if ( otherSideCell->GetType() == ncSpatialCell::N_INDOOR_CELL )
                    {
                        // move the camera to the next cell
                        otherSideCell->GetParentSpace()->MoveEntity(spatialComp->GetEntityObject(),
                                                                    otherSideCell);
                    }
                    else
                    {
                        n_assert(otherSideCell->GetType() == ncSpatialCell::N_QUADTREE_CELL);
                        n_verify(otherSideCell->GetParentSpace()->AddEntity(spatialComp->GetEntityObject()));
                    }

                    return true;
                }
            }
        }
    }

    // if the camera hasn't traversed any portal in this cell, and the cell is an outdoor cell
    // let's see if it has traversed any subcell's portal
    ncSpatialQuadtreeCell *quadCell = 0;
    if ((cell->GetType() == ncSpatialCell::N_QUADTREE_CELL))
    {
        quadCell = static_cast<ncSpatialQuadtreeCell*>(cell);
        if (!quadCell->IsLeaf())
        {
            ncSpatialQuadtreeCell **subcells = quadCell->GetSubcells();
            for (int i(0); i < 4; ++i)
            {
                if (subcells[i]->Contains(pos) || 
                    subcells[i]->Contains(spatialComp->GetPreviousPos())) 
                {
                    if (this->CheckPortalTraversing(spatialComp, subcells[i]))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Check if a segment (orig -> dest) intersects with a triangle (v0, v1, v2).
*/
bool 
nSpatialVisitor::IntersectsTriangle(vector3 const &orig, 
    vector3 const &dest,
    vector3 const &v0,
    vector3 const &v1,
    vector3 const &v2) const
{
    vector3 dir(dest - orig);
    vector3 edge1(v1 - v0);
    vector3 edge2(v2 - v0);

    vector3 pvec(dir * edge2);

    float det = edge1 % pvec;

    if ( det > -0.0001f && det < 0.0001f )
    {
        return false; 
    } 
    else
    {
        float u,v;
        float inv_det = 1.0f / det;
        vector3 tvec(orig - v0);

        u = (tvec % pvec) * inv_det ;
        if ( u < 0.0f  || u > 1.0f ) return false;

        vector3 qvec(tvec * edge1);

        v = ( dir % qvec) * inv_det;

        if ( v < 0.0f ||  u + v > 1.0f ) return false;

        return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nSpatialVisitor::Visit(ncSpatialPortal *portal, const matrix44 &newTransform)
{
    n_assert2(portal, "Error: NULL pointer to spatial portal!");

    // update component's bounding box
    portal->UpdateBBox(newTransform);

    // if we don't want to determine if the visitee has changed its cell or space, just return
    if (!portal->DetermineSpace())
    {
        return;
    }

    ncSpatialCell *cell = portal->GetCell();
    if (cell)
    {
        n_verify(cell->RemoveEntity(portal->GetEntityObject()));
    }

    if ( !nSpatialServer::Instance()->GetQuadtreeSpace()->AddEntity(portal->GetEntityObject()) )
    {
        nSpatialServer::Instance()->InsertGlobalEntity(portal->GetEntityObject());
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nSpatialVisitor::Visit(ncSpatialCamera *camera, const vector3 &newPos)
{
    n_assert2(camera, "miquelangel.rujula: NULL pointer to spatial camera!");

    ncSpatialOctree* octreeSpace = 0;
    if (nSpatialServer::Instance()->GetUseOctrees() &&
        (0 != (octreeSpace = nSpatialServer::Instance()->GetOctreeSpace())))
    {
        ncSpatialCell *cell = camera->GetCell();
        if (cell && 
            (cell->GetParentSpace()->GetEntityObject()->GetId() != octreeSpace->GetEntityObject()->GetId()))
        {
            n_verify(cell->RemoveEntity(camera->GetEntityObject()));
            octreeSpace->GetCellsArray().At(0)->AddEntity(camera->GetEntityObject());
        }

        camera->SetPreviousPos(newPos);
        return;
    }

    ncSpatialCell *cell = camera->GetCell();
    ncSpatialCell *newCell;

    if (cell)
    {
        camera->SetBBox(newPos, newPos);
        ncSpatialSpace *space = cell->GetParentSpace();
        if (space->GetType() == ncSpatialSpace::N_INDOOR_SPACE)
        {
#ifndef NGAME
            if (!static_cast<ncSpatialIndoor*>(space)->IsEnabled())
            {
                n_verify(cell->RemoveEntity(camera->GetEntityObject()));
                nSpatialServer::Instance()->InsertEntity(camera->GetEntityObject());
            }
#endif // !NGAME
            // the camera was in an indoor cell. Let's see where is now
            newCell = space->SearchCellContaining(newPos, 0);
            
            if (newCell)
            {
                // the camera hasn't left the space it was before
                if (newCell != cell)
                {
                    // but it has moved to another cell
                    space->MoveEntity(camera->GetEntityObject(), newCell);
                }
                camera->SetPreviousPos(newPos);
                return;
            }
        }

        // Let's see if it's traversing a portal
        ncSpatialPortal *inPortal = cell->IsInPortals(newPos);
        
        bool traversed;
#ifndef NGAME
        traversed = false;
        if ( !this->m_spatialServer->IsInEditIndoorMode() )
#endif // !NGAME
        {
            traversed = this->CheckPortalTraversing(camera, camera->GetCell());
        }

        if ( !inPortal && !traversed )
        {
#ifndef NGAME
            if ( this->m_spatialServer->IsInEditIndoorMode() && 
                (cell->GetType() == ncSpatialCell::N_INDOOR_CELL))
            {
                camera->SetPreviousPos(newPos);
                return;
            }
#endif // !NGAME

            // the camera has left the space it was before or it's in the outdoor
            // remove it from its actual space and insert it in its corresponding cell
            n_verify(cell->RemoveEntity(camera->GetEntityObject()));
            this->m_spatialServer->InsertEntity(camera->GetEntityObject());

        }
    }
    else
    {
        // the element was outside all the spaces. But now it can be inside. Let's check it
        camera->SetBBox(newPos, newPos);
        nSpatialServer::Instance()->InsertEntity(camera->GetEntityObject());
    }

    camera->SetPreviousPos(newPos);
}

//------------------------------------------------------------------------------
/**
    resituate the occluder in the corresponding cell only if is necessary
*/
void 
nSpatialVisitor::Visit(ncSpatialOccluder *visitee)
{
    n_assert2(visitee, "Error: NULL pointer to spatial occluder component!");

    // if we don't want to determine if the visitee has changed its cell or space, just return
    if (!visitee->DetermineSpace())
    {
        return;
    }

    ncSpatialCell *cell = visitee->GetCell();
    ncSpatialCell *newCell;
    if (cell)
    {
        // the entity was in a cell. Let's see if it's still in the same one or not
        ncSpatialQuadtree *quadtreeSpace = nSpatialServer::Instance()->GetQuadtreeSpace();
        newCell = quadtreeSpace->SearchCellContaining(visitee->GetBBox());
        if (newCell)
        {
            if (newCell != cell)
            {
                // the occluder has changed the cell
                this->MoveEntity(visitee->GetEntityObject(), newCell);
            }
        }
        else if (cell->GetType() == ncSpatialCell::N_QUADTREE_CELL)
        {
            // the occluder was in the quadtree space, but now it isn't, so, 
            // insert it in the global space
            n_verify(cell->RemoveEntity(visitee->GetEntityObject()));
            nSpatialServer::Instance()->InsertGlobalEntity(visitee->GetEntityObject());
        }
    }
    else
    {
        // the occluder wasn't inserted yet, let's do it now
        if ( !nSpatialServer::Instance()->GetQuadtreeSpace()->AddEntity(visitee->GetEntityObject()) )
        {
            nSpatialServer::Instance()->InsertGlobalEntity(visitee->GetEntityObject());
        }
    }
}

//------------------------------------------------------------------------------
/**
    Resituate the light and create its new regions
*/
void 
nSpatialVisitor::Visit(ncSpatialLight *light, const matrix44 &newTransform)
{
    n_assert2(light, "Error: NULL pointer to spatial light component!");

    // first of all, resituate the light
    this->Visit(light->GetComponentSafe<ncSpatial>(), newTransform);

    // recalculate new light regions
    if ( light->IsOn() && light->GetCell() )
    {
        this->CreateLightRegions(light);
    }
}

//------------------------------------------------------------------------------
/**
    Recalculate the light regions.
    Erases the old ones and creates the new ones.
*/
void
nSpatialVisitor::CreateLightRegions(ncSpatialLight *light)
{
    // remove all the old regions
    light->DestroyLightRegions();

    ncSpatialCell *cell = light->GetCell();
    n_assert(cell);
    if ( cell->GetType() == ncSpatialCell::N_GLOBAL_CELL )
    {
        return;
    }

    // create the region in the same cell as the light without frustum clipper, because
    // we will use its model for the intersection tests
    nLightRegion *newRegion = n_new(nLightRegion);
    light->AddLightRegion(newRegion);
    
    // create the new light regions
    nSpatialModel *model = light->GetTestModel();

    // array of traversed portals
    static nArray<ncSpatialPortal*> traversedPortals(8, 4);
    traversedPortals.Reset();

    if ( model )
    {
        if ( model->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE )
        {
            if ( cell->GetType() == ncSpatialCell::N_INDOOR_CELL )
            {
                // add the region to the light's cell
                cell->AddLightRegion(newRegion);
                // calculate the light links for the entities in the cell
                this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(cell, newRegion);
                if ( !light->AffectsOnlyACell() )
                {
                    this->BuildRegionsPoint(newRegion, cell, 0, traversedPortals);
                }
            }
            else
            {
#ifndef NGAME
                if ( cell->GetType() != ncSpatialCell::N_QUADTREE_CELL )
                {
                    return;
                }
#endif // !NGAME
                n_assert(cell->GetType() == ncSpatialCell::N_QUADTREE_CELL);
                ncSpatialQuadtreeCell *rootCell = static_cast<ncSpatialQuadtree*>(cell->GetParentSpace())->GetRootCell();
                // add the region to the root cell
                rootCell->AddLightRegion(newRegion);
                // calculate the light links for the entities and the terrain of the cell
                this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(rootCell, newRegion);
                if ( !light->AffectsOnlyACell() )
                {
                    this->BuildRegionsPointQuadtree(newRegion, rootCell, 0, traversedPortals);
                }
            }
        }
        else
        {
            // spot light
            // set the light's frustum clipper to the region, as they are the same
            nSpatialModel *model = light->GetTestModel();
            n_assert(model);
            n_assert( model->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM );
            nSpatialFrustumModel *frustumModel = static_cast<nSpatialFrustumModel*>(model);
            newRegion->SetClipper(frustumModel->GetFrustum());
                        
            VisitorFlags vf(false, false);
            if ( cell->GetType() == ncSpatialCell::N_INDOOR_CELL )
            {
                // add the region to the light's cell
                cell->AddLightRegion(newRegion);
                // calculate the light links for the entities in the cell
                this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(cell, newRegion);
                if ( !light->AffectsOnlyACell() )
                {
                    this->BuildRegionsSpot(newRegion, cell, 0, vf, traversedPortals);
                }
            }
            else
            {
#ifndef NGAME
                if ( cell->GetType() != ncSpatialCell::N_QUADTREE_CELL )
                {
                    return;
                }
#endif // !NGAME
                n_assert(cell->GetType() == ncSpatialCell::N_QUADTREE_CELL);
                ncSpatialQuadtreeCell *rootCell = static_cast<ncSpatialQuadtree*>(cell->GetParentSpace())->GetRootCell();
                // add the region to the root cell
                rootCell->AddLightRegion(newRegion);
                // calculate the light links for the entities and the terrain of the cell
                this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(rootCell, newRegion);
                if ( !light->AffectsOnlyACell() )
                {
                    this->BuildRegionsSpotQuadtree(newRegion, rootCell, 0, vf, traversedPortals);
                }
            }
        }
    }
    else
    {
        // omni light
        if ( cell->GetType() == ncSpatialCell::N_INDOOR_CELL )
        {
            // add the region to the light's cell
            cell->AddLightRegion(newRegion);

            // calculate the light links for the entities in the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(cell, newRegion);

            if ( !light->AffectsOnlyACell() )
            {
                this->BuildRegionsOmni(newRegion, cell, 0, traversedPortals);
            }
        }
        else
        {
#ifndef NGAME
            if ( cell->GetType() != ncSpatialCell::N_QUADTREE_CELL )
            {
                return;
            }
#endif // !NGAME
            n_assert(cell->GetType() == ncSpatialCell::N_QUADTREE_CELL);
            ncSpatialQuadtreeCell *rootCell = static_cast<ncSpatialQuadtree*>(cell->GetParentSpace())->GetRootCell();
            
            // add the region to the root cell
            rootCell->AddLightRegion(newRegion);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(rootCell, newRegion);

            // calculate the light links for the global entities
            if ( !light->GetComponent<ncSpatialLightEnv>() )
            {
                nArray<nEntityObject*> globalEntities;
                this->m_spatialServer->GetAllGlobalEntities(globalEntities);
                this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinksGlobals(globalEntities, newRegion);
            }

            if ( !light->AffectsOnlyACell() )
            {
                this->BuildRegionsOmniQuadtree(newRegion, rootCell, 0, traversedPortals);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Build the light regions for a point light
*/
void
nSpatialVisitor::BuildRegionsPoint(nLightRegion *region, ncSpatialCell *cell, ncSpatialCell *prevCell,
    nArray<ncSpatialPortal*> &traversedPortals)
{
    n_assert(region);
    n_assert(!region->GetCells().Empty());
    ncSpatialLight *light = region->GetParentLight();
    n_assert(light);
    n_assert(!region->GetClipper());
    n_assert(cell);

    const vector3 &lightPos = light->GetComponentSafe<ncTransform>()->GetPosition();
    nSpatialModel *model = light->GetTestModel();
    n_assert(model);

    vector3 normal;
    plane farPlane;
    ncSpatialPortal *portal;
    nLightRegion *newRegion;

    float farDist;
    ncSpatialCell *otherSideCell;
    const nArray<nEntityObject*> &portals = cell->GetCategory(nSpatialTypes::CAT_PORTALS);
    for ( int i(0); i < portals.Size(); ++i )
    {
        portal = portals[i]->GetComponentSafe<ncSpatialPortal>();

        otherSideCell = portal->GetOtherSideCell();
        n_assert(otherSideCell);

        if ( !portal->IsActive() || (otherSideCell == prevCell) || 
             (portal->GetPlane().distance(lightPos) < 0) || !model->Intersects(*portal)  )   
        {
            // clipped
            continue;
        }
        
        if ( !light->AffectsOtherSpaceTypes() && (cell->GetType() != otherSideCell->GetType()))
        {
            continue;
        }

        // check if we already have passed through this portal
        if ( traversedPortals.FindIndex(portal) != -1 )
        {
            continue;
        }
        else
        {
            traversedPortals.Append(portal);
        }

        // calculate far plane
        const vector3 &portalCenter = portal->GetClipCenter();
        normal.set(lightPos - portalCenter);
        normal.norm();
        farPlane.set(normal.x, normal.y, normal.z, 0);
        farDist = static_cast<nSpatialSphereModel*>(model)->GetRadius() - (portalCenter - lightPos).len();
        vector3 invNormal(-normal.x, -normal.y, -normal.z);
        farPlane.calculate_d(portalCenter + (invNormal * n_abs(farDist)));
        
        // create a new region
        newRegion = n_new(nLightRegion);
        newRegion->SetClipper(this->BuildNewFrustumNoTest(portal, lightPos, &farPlane));
        
        // link the region to the previous one
        newRegion->SetPrevLightRegion(region);

        // add the region to the light
        light->AddLightRegion(newRegion);

        VisitorFlags vf(false, false);
        if ( otherSideCell->GetType() == ncSpatialCell::N_INDOOR_CELL )
        {
            // add the region to the corresponding cell
            otherSideCell->AddLightRegion(newRegion);

            // calculate the light links for the entities in the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(otherSideCell, newRegion);
            this->BuildRegionsSpot(newRegion, otherSideCell, cell, vf, traversedPortals);
        }
        else
        {
#ifndef NGAME
            if ( otherSideCell->GetType() != ncSpatialCell::N_QUADTREE_CELL )
            {
                return;
            }
#endif // !NGAME
            n_assert(otherSideCell->GetType() == ncSpatialCell::N_QUADTREE_CELL);
            ncSpatialQuadtreeCell *rootCell = static_cast<ncSpatialQuadtree*>(otherSideCell->GetParentSpace())->GetRootCell();
            // add the region to the root cell
            rootCell->AddLightRegion(newRegion);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(rootCell, newRegion);
            this->BuildRegionsSpotQuadtree(newRegion, rootCell, cell, vf, traversedPortals);
        }

        traversedPortals.EraseQuick(traversedPortals.Size() - 1);
    }
}

//------------------------------------------------------------------------------
/**
    Build the light regions for point light in a quadtree
*/
void
nSpatialVisitor::BuildRegionsPointQuadtree(nLightRegion *region, ncSpatialQuadtreeCell *cell, 
    ncSpatialCell *prevCell, nArray<ncSpatialPortal*> &traversedPortals)
{
    n_assert(region);
    n_assert(cell);
    n_assert(!region->GetClipper());

    n_assert(region->GetParentLight());
    nSpatialModel *model = region->GetParentLight()->GetTestModel();
    n_assert(model);
    n_assert(model->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE);

    // build the regions for the cells whose portals connect with this one
    this->BuildRegionsPoint(region, cell, prevCell, traversedPortals);

    // recurse to subcells
    if ( !cell->IsLeaf() )
    {
        // check if the cells are touching the light or not
        ncSpatialQuadtreeCell **subcells = cell->GetSubcells();
        
        if ( model->Intersects(subcells[0]->GetBBox()) )
        {
            // not clipped
            // add the region to the cell
            subcells[0]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[0], region);
            this->BuildRegionsPointQuadtree(region, subcells[0], prevCell, traversedPortals);
        }
        
        if ( model->Intersects(subcells[1]->GetBBox()) )
        {
            // not clipped
            // add the region to the cell
            subcells[1]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[1], region);
            this->BuildRegionsPointQuadtree(region, subcells[1], prevCell, traversedPortals);
        }

        if ( model->Intersects(subcells[2]->GetBBox()) )
        {
            // not clipped
            // add the region to the cell
            subcells[2]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[2], region);
            this->BuildRegionsPointQuadtree(region, subcells[2], prevCell, traversedPortals);
        }

        if ( model->Intersects(subcells[3]->GetBBox()) )
        {
            // not clipped
            // add the region to the cell
            subcells[3]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[3], region);
            this->BuildRegionsPointQuadtree(region, subcells[3], prevCell, traversedPortals);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Build the light regions for a spot light
*/
void
nSpatialVisitor::BuildRegionsSpot(nLightRegion *region, ncSpatialCell *cell, 
    ncSpatialCell *prevCell, const VisitorFlags &clipstatus, nArray<ncSpatialPortal*> &traversedPortals)
{
    n_assert(region);

    // build the regions for the intersecting portals
    ncSpatialPortal *portal;
    nLightRegion *newRegion;

    ncSpatialLight *light = region->GetParentLight();
    const vector3 &lightPos = light->GetComponentSafe<ncTransform>()->GetPosition();
        
    nFrustumClipper *clipper = region->GetClipper();
    n_assert(clipper);

    n_assert(!region->GetCells().Empty());
    ncSpatialCell *otherSideCell;
    const nArray<nEntityObject*> &portals = cell->GetCategory(nSpatialTypes::CAT_PORTALS);
    for ( int i(0); i < portals.Size(); ++i )
    {
        portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
        
        otherSideCell = portal->GetOtherSideCell();
        n_assert(otherSideCell);
        if ( !portal->IsActive() || otherSideCell == prevCell || 
             (portal->GetPlane().distance(lightPos) < 0) )
        {
            continue;
        }

        if ( !light->AffectsOtherSpaceTypes() && (cell->GetType() != otherSideCell->GetType()))
        {
            continue;
        }

        // check if we already have passed through this portal
        if ( traversedPortals.FindIndex(portal) != -1 )
        {
            continue;
        }
        else
        {
            traversedPortals.Append(portal);
        }

        VisitorFlags vf(false, false);
        struct ClipVertexInfo vertsInfo[4];
        vf = clipper->TestPortalVertices(portal->GetVertices(), 4, vf, vertsInfo);
        if ( vf.AntiTestResult() )
        {
            // clipped
            traversedPortals.EraseQuick(traversedPortals.Size() - 1);
            continue;
        }
        
        // create a new region
        newRegion = n_new(nLightRegion);

        const plane *farPlane = 0;
        if ( clipper->GetFarPlaneIdx() != -1 )
        {
            farPlane = &clipper->GetFarPlane();
        }

        if ( vf.TestResult() )
        {
            newRegion->SetClipper(this->BuildNewFrustumNoTest(portal, lightPos, farPlane));
        }
        else
        {
            newRegion->SetClipper(this->BuildNewFrustumTest(portal, *clipper, farPlane, 
                lightPos, vertsInfo));
        }

        // add the region to the light
        n_assert(region->GetParentLight());
        region->GetParentLight()->AddLightRegion(newRegion);

        // link the region to the previous one
        newRegion->SetPrevLightRegion(region);

        // recurse to the other cell
        if ( otherSideCell->GetType() == ncSpatialCell::N_INDOOR_CELL )
        {
            // add the region to the corresponding cell
            otherSideCell->AddLightRegion(newRegion);

            // calculate the light links for the entities in the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(otherSideCell, newRegion);
            this->BuildRegionsSpot(newRegion, otherSideCell, cell, clipstatus, traversedPortals);
        }
        else
        {
#ifndef NGAME
            if ( otherSideCell->GetType() != ncSpatialCell::N_QUADTREE_CELL )
            {
                return;
            }
#endif // !NGAME
            n_assert(otherSideCell->GetType() == ncSpatialCell::N_QUADTREE_CELL);
            ncSpatialQuadtreeCell *rootCell = static_cast<ncSpatialQuadtree*>(otherSideCell->GetParentSpace())->GetRootCell();
            // add the region to the corresponding cell
            rootCell->AddLightRegion(newRegion);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(rootCell, newRegion);
            this->BuildRegionsSpotQuadtree(newRegion, rootCell, cell, clipstatus, traversedPortals);
        }

        traversedPortals.EraseQuick(traversedPortals.Size() - 1);
    }
}

//------------------------------------------------------------------------------
/**
    Build the light regions for a spot light in a quadtree
*/
void
nSpatialVisitor::BuildRegionsSpotQuadtree(nLightRegion *region, ncSpatialQuadtreeCell *cell, 
    ncSpatialCell *prevCell, const VisitorFlags &clipstatus, nArray<ncSpatialPortal*> &traversedPortals)
{
    n_assert(region);
    n_assert(cell);
    
    nFrustumClipper *clipper = region->GetClipper();
    n_assert(clipper);

    // build the regions for the cells whose portals connect with this one
    this->BuildRegionsSpot(region, cell, prevCell, clipstatus, traversedPortals);

    // recurse to subcells
    if ( !cell->IsLeaf() )
    {
        // check if the cells are touching the light or not
        ncSpatialQuadtreeCell **subcells = cell->GetSubcells();
        
        VisitorFlags vf(false, false);
        clipper->TestBBox(subcells[0]->GetBBoxCenter(), subcells[0]->GetBBoxExtents(), clipstatus, vf);
        if ( !vf.AntiTestResult() )
        {
            // not clipped
            // add the region to the corresponding cell
            subcells[0]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[0], region);
            this->BuildRegionsSpotQuadtree(region, subcells[0], prevCell, vf, traversedPortals);
        }
        
        clipper->TestBBox(subcells[1]->GetBBoxCenter(), subcells[1]->GetBBoxExtents(), clipstatus, vf);
        if ( !vf.AntiTestResult() )
        {
            // not clipped
            // add the region to the corresponding cell
            subcells[1]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[1], region);
            this->BuildRegionsSpotQuadtree(region, subcells[1], prevCell, vf, traversedPortals);
        }

        clipper->TestBBox(subcells[2]->GetBBoxCenter(), subcells[2]->GetBBoxExtents(), clipstatus, vf);
        if ( !vf.AntiTestResult() )
        {
            // not clipped
            // add the region to the corresponding cell
            subcells[2]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[2], region);
            this->BuildRegionsSpotQuadtree(region, subcells[2], prevCell, vf, traversedPortals);
        }

        clipper->TestBBox(subcells[3]->GetBBoxCenter(), subcells[3]->GetBBoxExtents(), clipstatus, vf);
        if ( !vf.AntiTestResult() )
        {
            // not clipped
            // add the region to the corresponding cell
            subcells[3]->AddLightRegion(region);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[3], region);
            this->BuildRegionsSpotQuadtree(region, subcells[3], prevCell, vf, traversedPortals);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Build the light regions for an omni light
*/
void
nSpatialVisitor::BuildRegionsOmni(nLightRegion *region, ncSpatialCell *cell, ncSpatialCell *prevCell,
    nArray<ncSpatialPortal*> &traversedPortals)
{
    n_assert(region);
    n_assert(!region->GetClipper());
    ncSpatialLight *light = region->GetParentLight();
    n_assert(light);
    n_assert(!light->GetTestModel());
    n_assert(cell);
    
    ncSpatialPortal *portal;
    nLightRegion *newRegion;
    ncSpatialCell *otherSideCell;

    const vector3 &lightPos = light->GetComponentSafe<ncTransform>()->GetPosition();
    const nArray<nEntityObject*> &portals = cell->GetCategory(nSpatialTypes::CAT_PORTALS);
    for ( int i(0); i < portals.Size(); ++i )
    {
        portal = portals[i]->GetComponentSafe<ncSpatialPortal>();

        otherSideCell = portal->GetOtherSideCell();
        n_assert(otherSideCell);
        if ( !portal->IsActive() || otherSideCell == prevCell || 
             (portal->GetPlane().distance(lightPos) < 0) )
        {
            continue;
        }

        if ( !light->AffectsOtherSpaceTypes() && (cell->GetType() != otherSideCell->GetType()))
        {
            continue;
        }

        // check if we already have passed through this portal
        if ( traversedPortals.FindIndex(portal) != -1 )
        {
            continue;
        }
        else
        {
            traversedPortals.Append(portal);
        }

        // create a new region
        newRegion = n_new(nLightRegion);
        newRegion->SetClipper(this->BuildNewFrustumNoTest(portal, lightPos, 0));
        
        // add the region to the light
        light->AddLightRegion(newRegion);

        // link the region to the previous one
        newRegion->SetPrevLightRegion(region);

        VisitorFlags vf(false, false);
        if ( otherSideCell->GetType() == ncSpatialCell::N_INDOOR_CELL )
        {
            // recurse to the other cell
            // add the region to the corresponding cell
            otherSideCell->AddLightRegion(newRegion);

            // calculate the light links for the entities in the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(otherSideCell, newRegion);
            this->BuildRegionsSpot(newRegion, otherSideCell, cell, vf, traversedPortals);
        }
        else
        {
#ifndef NGAME
            if ( otherSideCell->GetType() != ncSpatialCell::N_QUADTREE_CELL )
            {
                return;
            }
#endif // !NGAME
            n_assert(otherSideCell->GetType() == ncSpatialCell::N_QUADTREE_CELL);
            ncSpatialQuadtreeCell *rootCell = static_cast<ncSpatialQuadtree*>(otherSideCell->GetParentSpace())->GetRootCell();
            // add the region to the corresponding cell
            rootCell->AddLightRegion(newRegion);

            // calculate the light links for the entities and the terrain of the cell
            this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(rootCell, newRegion);
            this->BuildRegionsSpotQuadtree(newRegion, rootCell, cell, vf, traversedPortals);
        }

        traversedPortals.EraseQuick(traversedPortals.Size() - 1);
    }
}

//------------------------------------------------------------------------------
/**
    Build the light regions for an omni light in a quadtree
*/
void
nSpatialVisitor::BuildRegionsOmniQuadtree(nLightRegion *region, ncSpatialQuadtreeCell *cell, 
    ncSpatialCell *prevCell, nArray<ncSpatialPortal*> &traversedPortals)
{
    n_assert(region);
    n_assert(cell);
    
    this->BuildRegionsOmni(region, cell, prevCell, traversedPortals);

    if ( !cell->IsLeaf() )
    {
        ncSpatialQuadtreeCell **subcells = cell->GetSubcells();
        // add the region to the subcell
        subcells[0]->AddLightRegion(region);

        // calculate the light links for the entities and the terrain of the cell
        this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[0], region);
        this->BuildRegionsOmniQuadtree(region, subcells[0], prevCell, traversedPortals);
        // add the region to the subcell
        subcells[1]->AddLightRegion(region);

        // calculate the light links for the entities and the terrain of the cell
        this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[1], region);
        this->BuildRegionsOmniQuadtree(region, subcells[1], prevCell, traversedPortals);
        // add the region to the subcell
        subcells[2]->AddLightRegion(region);

        // calculate the light links for the entities and the terrain of the cell
        this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[2], region);
        this->BuildRegionsOmniQuadtree(region, subcells[2], prevCell, traversedPortals);
        // add the region to the subcell
        subcells[3]->AddLightRegion(region);

        // calculate the light links for the entities and the terrain of the cell
        this->m_spatialServer->GetVisibilityVisitor().CalculateLightLinks(subcells[3], region);
        this->BuildRegionsOmniQuadtree(region, subcells[3], prevCell, traversedPortals);
    }
}

//------------------------------------------------------------------------------
/**
    Build a new frustum directly using the portal vertices and the given 
    far plane
*/
nFrustumClipper 
nSpatialVisitor::BuildNewFrustumNoTest(ncSpatialPortal *portal, const vector3 &pos,
    const plane *farPlane)
{
    n_assert(portal);

    const vector3 *vertices = portal->GetVertices();
    n_assert2(vertices, "miquelangel.rujula: NULL pointer to portal vertices!");

    plane newClipPlanes[8];
    int numNewClipPlanes = 0;
    
    // set the near plane
    newClipPlanes[numNewClipPlanes] = portal->GetTwinPortal()->GetPlane();
    int nearPlaneIdx(numNewClipPlanes);
    ++numNewClipPlanes;

    int i2;
    // initialize vertex index 1 to the first vertex (in the loop)
    n_assert(numNewClipPlanes == 1);
    for (int i1(0); i1 < 4; ++i1)
    {
        i2 = (i1+1) % 4;
        n_assert(numNewClipPlanes < 5);
        newClipPlanes[numNewClipPlanes].set(pos, vertices[i1], vertices[i2]);
        ++numNewClipPlanes;
    }

    int farPlaneIdx(-1);
    if ( farPlane )
    {
        // set the far plane
        n_assert(numNewClipPlanes == 5);
        newClipPlanes[numNewClipPlanes] = *farPlane;
        farPlaneIdx = numNewClipPlanes;
        ++numNewClipPlanes;
    }

    n_assert(numNewClipPlanes <= 6);
    nFrustumClipper newFrustum(newClipPlanes, numNewClipPlanes);
    newFrustum.SetNearPlaneIdx(nearPlaneIdx);
    newFrustum.SetFarPlaneIdx(farPlaneIdx);

    return newFrustum;
}

//------------------------------------------------------------------------------
/**
    Build a new frustum using the portal vertices, the old frustum and the 
    given far plane
*/
nFrustumClipper 
nSpatialVisitor::BuildNewFrustumTest(ncSpatialPortal *portal, 
    const nFrustumClipper &oldFrustum,
    const plane *farPlane,
    const vector3 &pos,
    const struct ClipVertexInfo *vertsInfo)
{
    n_assert(portal);
    
    const vector3 *vertices = portal->GetVertices();
    n_assert2(vertices, "miquelangel.rujula: NULL pointer to portal vertices!");
    const plane *planes = oldFrustum.GetPlanes();
    n_assert2(planes, "miquelangel.rujula: frustum has no planes!");
    
    plane newClipPlanes[N_PLANECLIPPER_MAXPLANES];
    int numNewClipPlanes = 0;
    unsigned short usedPlanes = 0; // which planes from the original frustum are reused

    // set the near plane
    newClipPlanes[numNewClipPlanes] = portal->GetTwinPortal()->GetPlane();
    int nearPlaneIdx(numNewClipPlanes);
    ++numNewClipPlanes;

    // we have the vertices classified (clipped or not) to build up the new frustum. Let's do it
    int i2;
    // initialize vertex index 1 to the first vertex (in the loop)
    for (int i1(0); i1 < 4; ++i1)
    {
        if (numNewClipPlanes >= N_PLANECLIPPER_MAXPLANES)
        {
            return oldFrustum;
        }

        i2 = (i1+1) % 4;

        if (vertsInfo[i1].clipped)
        {
            if (!vertsInfo[i2].clipped)
            {
                // v1 clipped and v2 not clipped
                newClipPlanes[numNewClipPlanes].set(pos, vertices[i1], vertices[i2]);
                ++numNewClipPlanes;

                // add the planes that clipped 'vertices[i1]' to the new frustum, 
                // if they aren't used yet
                unsigned short planeMask;
                for (int i(0); i < vertsInfo[i1].numPlanes; ++i)
                {
                    planeMask = static_cast<unsigned short>(1<<vertsInfo[i1].planes[i]);
                    if ( (usedPlanes & planeMask) == 0 )
                    {
                        // the plane is not used yet, let's do it
                        newClipPlanes[numNewClipPlanes] = planes[vertsInfo[i1].planes[i]];
                        ++numNewClipPlanes;

                        // mark the plane like used
                        usedPlanes |= planeMask;
                    }
                }
            }
            else
            {
                // v1 and v2 clipped
                unsigned short planeMask = vertsInfo[i1].planesMask & vertsInfo[i2].planesMask;
                if ( planeMask != 0 )
                {
                    // both vertices are clipped by common planes
                    if ( (usedPlanes & planeMask) == 0 )
                    {
                        // the planes are stil not used, let's do it
                        int bit = 1;
                        for (int i(0); i < oldFrustum.GetNumPlanes(); ++i, bit <<= 1)
                        {
                            if ((bit & planeMask) != 0)
                            {
                                newClipPlanes[numNewClipPlanes] = planes[i];
                                ++numNewClipPlanes;
                            }
                        }

                        // mark the planes like used
                        usedPlanes |= planeMask;
                    }
                }
                else
                {
                    // v1 and v2 are clipped by different planes (they haven't a common clipping plane)
                    newClipPlanes[numNewClipPlanes].set(pos, vertices[i1], vertices[i2]);
                    ++numNewClipPlanes;
                }
            }
        }
        else
        {
            if (!vertsInfo[i2].clipped)
            {
                // v1 and v2 not clipped
                newClipPlanes[numNewClipPlanes].set(pos, vertices[i1], vertices[i2]);
                ++numNewClipPlanes;
            }
            else
            {
                // v1 not clipped and v2 clipped
                newClipPlanes[numNewClipPlanes].set(pos, vertices[i1], vertices[i2]);
                ++numNewClipPlanes;

                // add the planes that clipped 'vertices[i2]' to the new frustum, 
                // if they aren't used yet
                unsigned short planeMask;
                for (int i(0); i < vertsInfo[i2].numPlanes; ++i)
                {
                    planeMask = static_cast<unsigned short>(1<<vertsInfo[i2].planes[i]);
                    if ( (usedPlanes & planeMask) == 0 )
                    {
                        // the plane is not used yet, let's do it
                        newClipPlanes[numNewClipPlanes] = planes[vertsInfo[i2].planes[i]];
                        ++numNewClipPlanes;

                        // mark the plane like used
                        usedPlanes |= planeMask;
                    }
                }
            }
        }
    }

    if (numNewClipPlanes >= N_PLANECLIPPER_MAXPLANES)
    {
        return oldFrustum;
    }

    int farPlaneIdx(-1);
    if ( farPlane )
    {
        // set the far plane
        newClipPlanes[numNewClipPlanes] = *farPlane;
        farPlaneIdx = numNewClipPlanes;
        ++numNewClipPlanes;
    }

    if (numNewClipPlanes >= N_PLANECLIPPER_MAXPLANES)
    {
        return oldFrustum;
    }
    nFrustumClipper newFrustum(newClipPlanes, numNewClipPlanes);
    newFrustum.SetNearPlaneIdx(nearPlaneIdx);
    newFrustum.SetFarPlaneIdx(farPlaneIdx);

    return newFrustum;
}

//------------------------------------------------------------------------------
/**
    Search all the spatial elements in the sphere and give them to the collector.
    The sphere has to be in world coordinates. The collector will decide if it
    wants to keep the given spatial element.
*/
void
nSpatialVisitor::Visit(const sphere &sph, nSpatialCollector *collector)
{
    // collect all the entities in the indoors that intersect the sphere
    static nArray<nEntityObject*> intersectingEntities;
    intersectingEntities.Reset();

    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_spatialServer->m_indoorSpaces.Begin();
                                     pIndoorSpace != this->m_spatialServer->m_indoorSpaces.End();
                                     pIndoorSpace++)
    {
        (*pIndoorSpace)->SearchEntitiesIntersecting(sph, &intersectingEntities);
    }

    // collect all the entities in the outdoors that intersect the sphere
    if (this->m_spatialServer->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * outdoorSpace = this->m_spatialServer->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();
        outdoorSpace->SearchEntitiesIntersecting(sph, &intersectingEntities);
    }

    // pass the entities to the collector
    for (nArray<nEntityObject*>::iterator pEntity  = intersectingEntities.Begin();
                                          pEntity != intersectingEntities.End();
                                          pEntity++)
    {
        collector->Accept((*pEntity));
    }
}

//------------------------------------------------------------------------------
/**
    Search all the spatial elements in the bounding box and give them to 
    the collector.
    The bounding box has to be in world coordinates. The collector will 
    decide if it wants to keep the given spatial element.
*/
void
nSpatialVisitor::Visit(const bbox3 &box, nSpatialCollector *collector)
{
    // collect all the entities in the indoors that intersect the box
    static nArray<nEntityObject*> intersectingEntities;
    intersectingEntities.Reset();

    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_spatialServer->m_indoorSpaces.Begin();
                                     pIndoorSpace != this->m_spatialServer->m_indoorSpaces.End();
                                     pIndoorSpace++)
    {
        (*pIndoorSpace)->SearchEntitiesIntersecting(box, &intersectingEntities);
    }

    // collect all the entities in the outdoors that intersect the box
    if (this->m_spatialServer->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * outdoorSpace = this->m_spatialServer->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();
        outdoorSpace->SearchEntitiesIntersecting(box, &intersectingEntities);
    }

    // pass the entities to the collector
    for (nArray<nEntityObject*>::iterator pEntity  = intersectingEntities.Begin();
                                          pEntity != intersectingEntities.End();
                                          pEntity++)
    {
        collector->Accept((*pEntity));
    }
}

//------------------------------------------------------------------------------
/**
    search all the entities whose positions are in the sphere and give them to the collector
*/
void 
nSpatialVisitor::GetEntitiesByPos(const sphere &sph, nSpatialCollector *collector)
{
    // collect all the entities in the indoors that are into the sphere
    nArray<nEntityObject*> containedEntities;
    containedEntities.Reset();

    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_spatialServer->m_indoorSpaces.Begin();
                                     pIndoorSpace != this->m_spatialServer->m_indoorSpaces.End();
                                     pIndoorSpace++)
    {
        (*pIndoorSpace)->SearchEntitiesContainedByPos(sph, &containedEntities);
    }

    // collect all the entities in the outdoors that are into the sphere
    if (this->m_spatialServer->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * outdoorSpace = this->m_spatialServer->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();
        outdoorSpace->SearchEntitiesContainedByPos(sph, &containedEntities);
    }

    // pass the entities to the collector
    for (nArray<nEntityObject*>::iterator pEntity  = containedEntities.Begin();
                                          pEntity != containedEntities.End();
                                          pEntity++)
    {
        collector->Accept((*pEntity));
    }
}

//------------------------------------------------------------------------------
/**
    get the entities whose positions are in the sphere and add them to the given array
*/
void 
nSpatialVisitor::GetEntitiesByPos(const sphere &sph, nArray<nEntityObject*> *entities)
{
    // collect all the entities in the indoors that are into the sphere
    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_spatialServer->m_indoorSpaces.Begin();
                                            pIndoorSpace != this->m_spatialServer->m_indoorSpaces.End();
                                            pIndoorSpace++)
    {
        (*pIndoorSpace)->SearchEntitiesContainedByPos(sph, entities);
    }

    // collect all the entities in the outdoors that are into the sphere
    if (this->m_spatialServer->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * outdoorSpace = this->m_spatialServer->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();
        outdoorSpace->SearchEntitiesContainedByPos(sph, entities);
    }
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
bool 
nSpatialVisitor::GetEntitiesCategory(const bbox3 &box,
                                     int category, 
                                     const int flags, 
                                     nSpatialCollector *collector)
{
    n_assert2(collector, "Null pointer to a spatial collector!");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatBoxCol.StartAccum();
#endif // __NEBULA_STATS__

    static nArray<nEntityObject*> entities;
    entities.Reset();

    bool result(false);
    result = this->GetEntitiesCategory(box, category, flags, entities);
    
    // pass the entities to the collector
    for (int i(0); i < entities.Size(); ++i)
    {
        collector->Accept(entities[i]);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatBoxCol.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
bool 
nSpatialVisitor::GetEntitiesCategory(const sphere &sph,
                                     int category, 
                                     const int flags, 
                                     nSpatialCollector *collector)
{
    n_assert2(collector, "Null pointer to a spatial collector!");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatSphCol.StartAccum();
#endif // __NEBULA_STATS__

    static nArray<nEntityObject*> entities;
    entities.Reset();

    bool result(false);
    result = this->GetEntitiesCategory(sph, category, flags, entities);

    // pass the entities to the collector
    for (int i = 0; i < entities.Size(); i++)
    {
        collector->Accept(entities[i]);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatSphCol.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
bool 
nSpatialVisitor::GetEntitiesCategory(const vector3 &point, 
                                     int category, 
                                     const int flags, 
                                     nSpatialCollector *collector)
{
    n_assert2(collector, "Null pointer to a spatial collector!");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatPointCol.StartAccum();
#endif // __NEBULA_STATS__

    static nArray<nEntityObject*> entities;
    entities.Reset();

    bool result(false);
    result = this->GetEntitiesCategory(point, category, flags, entities);
    
    // pass the entities to the collector
    for (int i = 0; i < entities.Size(); i++)
    {
        collector->Accept(entities[i]);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatPointCol.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
bool 
nSpatialVisitor::GetEntitiesCategory(const bbox3 &box,
                                     int category, 
                                     const int flags, 
                                     nArray<nEntityObject*> &entities)
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCategories(box, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
bool 
nSpatialVisitor::GetEntitiesCategory(const sphere &sph,
                                     int category, 
                                     const int flags, 
                                     nArray<nEntityObject*> &entities)
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCategories(sph, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialVisitor::GetEntitiesCategory(const vector3 &point, 
                                     int category, 
                                     const int flags, 
                                     nArray<nEntityObject*> &entities)
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCategories(point, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities various categories using the given flags and a collector
*/
bool 
nSpatialVisitor::GetEntitiesCategories(const bbox3 &box,
                                       const nArray<int> &categories,
                                       const int flags, 
                                       nArray<nEntityObject*> &entities)
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsBox.StartAccum();
#endif // __NEBULA_STATS__

    bool result(false);
    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        result = this->GetEntitiesCategoriesOutdoors(box, categories, flags, entities);
    }
    
    if (flags & nSpatialTypes::SPF_ALL_INDOORS || flags & nSpatialTypes::SPF_ONE_INDOOR )
    {
        // search indoors
        result |= this->GetEntitiesCategoriesIndoors(box, categories, flags, entities);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsBox.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags and a collector
*/
bool 
nSpatialVisitor::GetEntitiesCategories(const sphere &sph,
                                       const nArray<int> &categories,
                                       const int flags, 
                                       nArray<nEntityObject*> &entities)
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsSph.StartAccum();
#endif // __NEBULA_STATS__

    bool result(false);
    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        result = this->GetEntitiesCategoriesOutdoors(sph, categories, flags, entities);
    }

    if (flags & nSpatialTypes::SPF_ALL_INDOORS || flags & nSpatialTypes::SPF_ONE_INDOOR )
    {
        // search indoors
        result |= this->GetEntitiesCategoriesIndoors(sph, categories, flags, entities);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsSph.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags
*/
bool 
nSpatialVisitor::GetEntitiesCategories(const vector3 &point, 
                                       const nArray<int> &categories,
                                       const int flags, 
                                       nArray<nEntityObject*> &entities)
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsPoint.StartAccum();
#endif // __NEBULA_STATS__

    bool result(false);
    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        result = this->GetEntitiesCategoriesOutdoors(point, categories, flags, entities);
    }
    if ((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS))
    {
        // search indoors
        result |= this->GetEntitiesCategoriesIndoors(point, categories, flags, entities);
    }
    //else
    //{
    //    // try to find an indoor that contains the point and collect the entities
    //    const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
    //    for (int i = 0; i < indoors.Size(); i++)
    //    {
    //        if (indoors[i]->GetEntitiesCategories(point, categories, flags, entities))
    //        {
    //            #if __NEBULA_STATS__
    //            this->profSpa_GetEntsCatsPoint.StopAccum();
    //            #endif // __NEBULA_STATS__
    //            return true;
    //        }
    //    }

    //    // there's no indoor containing the point, let's check the outdoors
    //    int newFlags = flags;
    //    newFlags |= nSpatialTypes::SPF_OUTDOORS;
    //    result = this->GetEntitiesCategoriesOutdoors(point, categories, newFlags, entities);
    //}

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsPoint.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the box and the flags. 
    It returns the intersecting cells
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategory(const bbox3 &box,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by category not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatBox.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoryOutdoors(box, category, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoryIndoors(box, category, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the sphere and the flags. 
    It returns the intersecting cells
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategory(const sphere &sph,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by category not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatSph.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoryOutdoors(sph, category, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoryIndoors(sph, category, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the point and the flags. It returns the intersecting cells
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategory(const vector3 &point,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by category not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatPoint.StartAccum();
#endif // __NEBULA_STATS__

    bool result(false);

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        result = this->GetEntitiesCellsCategoryOutdoors(point, category, flags, entities, cells);
    }
    else if ((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS))
    {
        // search indoors
        result = this->GetEntitiesCellsCategoryIndoors(point, category, flags, entities, cells);
    }
    else
    {
        // try to find an indoor that contains the point and collect the entities
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCellsCategory(point, category, flags, entities, cells))
            {
                #if __NEBULA_STATS__
                this->profSpa_GetEntsCatPoint.StopAccum();
                #endif // __NEBULA_STATS__
                return true;
            }
        }

        // there's no indoor containing the point, let's check the outdoors
        int newFlags = flags;
        newFlags |= nSpatialTypes::SPF_OUTDOORS;
        result = this->GetEntitiesCellsCategoryOutdoors(point, category, newFlags, entities, cells);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatPoint.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. 
    It returns the intersecting cells
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategories(const bbox3 &box, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsBox.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoriesOutdoors(box, categories, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoriesIndoors(box, categories, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. 
    It returns the intersecting cells
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategories(const sphere &sph, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsSph.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoriesOutdoors(sph, categories, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesCellsCategoriesIndoors(sph, categories, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. It returns the intersecting cells
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategories(const vector3 &point, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsPoint.StartAccum();
#endif // __NEBULA_STATS__

    bool result(false);
    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        result = this->GetEntitiesCellsCategoriesOutdoors(point, categories, flags, entities, cells);
    }
    else if ((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS))
    {
        // search indoors
        result = this->GetEntitiesCellsCategoriesIndoors(point, categories, flags, entities, cells);
    }
    else
    {
        // try to find an indoor that contains the point and collect the entities
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i(0); i < indoors.Size(); ++i)
        {
            if (indoors[i]->GetEntitiesCellsCategories(point, categories, flags, entities, cells))
            {
                #if __NEBULA_STATS__
                this->profSpa_GetEntsCatsPoint.StopAccum();
                #endif // __NEBULA_STATS__
                return true;
            }
        }

        // there's no indoor containing the point, let's check the outdoors
        int newFlags = flags;
        newFlags |= nSpatialTypes::SPF_OUTDOORS;
        result = this->GetEntitiesCellsCategoriesOutdoors(point, categories, newFlags, entities, cells);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsPoint.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the box and the flags and 
    the given cells
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategory(const bbox3 &box,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by category not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatBox.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoryOutdoors(box, category, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoryIndoors(box, category, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the sphere and the flags and the given cells
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategory(const sphere &sph,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by category not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatSph.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoryOutdoors(sph, category, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoryIndoors(sph, category, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the point and the flags and 
    the given cells
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategory(const vector3 &point,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by category not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatPoint.StartAccum();
#endif // __NEBULA_STATS__

    bool result(false);
    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        result = this->GetEntitiesUsingCellsCategoryOutdoors(point, category, flags, entities, cells);
    }
    else if ((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS))
    {
        // search indoors
        result = this->GetEntitiesUsingCellsCategoryIndoors(point, category, flags, entities, cells);
    }
    else
    {
        // try to find an indoor that contains the point and collect the entities
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategory(point, category, flags, entities, cells))
            {
                #if __NEBULA_STATS__
                this->profSpa_GetEntsCatPoint.StopAccum();
                #endif // __NEBULA_STATS__
                return true;
            }
        }

        // there's no indoor containing the point, let's check the outdoors
        int newFlags = flags;
        newFlags |= nSpatialTypes::SPF_OUTDOORS;
        result = this->GetEntitiesUsingCellsCategoryOutdoors(point, category, newFlags, entities, cells);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatPoint.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags and the given cells
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategories(const bbox3 &box, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsBox.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoriesOutdoors(box, categories, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsBox.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoriesIndoors(box, categories, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags and the given cells
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategories(const sphere &sph, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsSph.StartAccum();
#endif // __NEBULA_STATS__

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoriesOutdoors(sph, categories, flags, entities, cells);
    }
    else
    {
        // search indoors
        #if __NEBULA_STATS__
        this->profSpa_GetEntsCatsSph.StopAccum();
        #endif // __NEBULA_STATS__
        return this->GetEntitiesUsingCellsCategoriesIndoors(sph, categories, flags, entities, cells);
    }
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags and the given cells
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategories(const vector3 &point, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert2(!(flags & nSpatialTypes::SPF_OCTREE), "Octrees query by categories not implemented yet.");

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsPoint.StartAccum();
#endif // __NEBULA_STATS__

    bool result(false);

    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        // search outdoors
        result = this->GetEntitiesUsingCellsCategoriesOutdoors(point, categories, flags, entities, cells);
    }
    else if ((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS))
    {
        // search indoors
        result = this->GetEntitiesUsingCellsCategoriesIndoors(point, categories, flags, entities, cells);
    }
    else
    {
        // try to find an indoor that contains the point and collect the entities
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i(0); i < indoors.Size(); ++i)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells))
            {
                #if __NEBULA_STATS__
                this->profSpa_GetEntsCatsPoint.StopAccum();
                #endif // __NEBULA_STATS__
                return true;
            }
        }

        // there's no indoor containing the point, let's check the outdoors
        int newFlags = flags;
        newFlags |= nSpatialTypes::SPF_OUTDOORS;
        result = this->GetEntitiesUsingCellsCategoriesOutdoors(point, categories, newFlags, entities, cells);
    }

#if __NEBULA_STATS__
    this->profSpa_GetEntsCatsPoint.StopAccum();
#endif // __NEBULA_STATS__

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCategoriesIndoors(const bbox3 &box,
                                              const nArray<int> &categories,
                                              const int flags, 
                                              nArray<nEntityObject*> &entities)
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCategories(box, categories, flags, entities))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCategories(box, categories, flags, entities);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCategoriesOutdoors(const bbox3 &box,
                                               const nArray<int> &categories,
                                               const int flags, 
                                               nArray<nEntityObject*> &entities)
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesCategories(box, categories, flags, entities);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCategoriesIndoors(const sphere &sph,
                                              const nArray<int> &categories,
                                              const int flags, 
                                              nArray<nEntityObject*> &entities)
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCategories(sph, categories, flags, entities))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCategories(sph, categories, flags, entities);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCategoriesOutdoors(const sphere &sph,
                                               const nArray<int> &categories,
                                               const int flags, 
                                               nArray<nEntityObject*> &entities)
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesCategories(sph, categories, flags, entities);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCategoriesIndoors(const vector3 &point,
                                              const nArray<int> &categories,
                                              const int flags, 
                                              nArray<nEntityObject*> &entities)
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);

    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCategories(point, categories, flags, entities))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCategories(point, categories, flags, entities);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCategoriesOutdoors(const vector3 &point,
                                               const nArray<int> &categories,
                                               const int flags, 
                                               nArray<nEntityObject*> &entities)
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesCategories(point, categories, flags, entities);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoryIndoors(const bbox3 &box,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCellsCategory(box, category, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCellsCategory(box, category, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSpatialVisitor::GetEntitiesCellsCategoriesIndoors(const bbox3 &box,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCellsCategories(box, categories, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCellsCategories(box, categories, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoryOutdoors(const bbox3 &box,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesCellsCategory(box, category, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoriesOutdoors(const bbox3 &box,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    bool result(false);
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    if (outdoor)
    {
        result = outdoor->GetEntitiesCellsCategories(box, categories, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoryIndoors(const sphere &sph,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);

    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCellsCategory(sph, category, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCellsCategory(sph, category, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoriesIndoors(const sphere &sph,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCellsCategories(sph, categories, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCellsCategories(sph, categories, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoryOutdoors(const sphere &sph,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    bool result(false);
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    if (outdoor)
    {
        result = outdoor->GetEntitiesCellsCategory(sph, category, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoriesOutdoors(const sphere &sph,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    bool result(false);
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    if (outdoor)
    {
        result = outdoor->GetEntitiesCellsCategories(sph, categories, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoryIndoors(const vector3 &point,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCellsCategory(point, category, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCellsCategory(point, category, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool  
nSpatialVisitor::GetEntitiesCellsCategoriesIndoors(const vector3 &point,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesCellsCategories(point, categories, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesCellsCategories(point, categories, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoryOutdoors(const vector3 &point,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    bool result(false);
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    if (outdoor)
    {
        result = outdoor->GetEntitiesCellsCategory(point, category, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesCellsCategoriesOutdoors(const vector3 &point,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    bool result(false);
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    if (outdoor)
    {
        result = outdoor->GetEntitiesCellsCategories(point, categories, flags, entities, cells);
    }

    return result;
}





//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoryIndoors(const bbox3 &box,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategory(box, category, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result = indoors[i]->GetEntitiesUsingCellsCategory(box, category, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoriesIndoors(const bbox3 &box,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);

    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategories(box, categories, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesUsingCellsCategories(box, categories, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoryOutdoors(const bbox3 &box,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesUsingCellsCategory(box, category, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoriesOutdoors(const bbox3 &box,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesUsingCellsCategories(box, categories, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoryIndoors(const sphere &sph,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategory(sph, category, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result = indoors[i]->GetEntitiesUsingCellsCategory(sph, category, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoriesIndoors(const sphere &sph,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategories(sph, categories, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesUsingCellsCategories(sph, categories, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoryOutdoors(const sphere &sph,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesUsingCellsCategory(sph, category, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoriesOutdoors(const sphere &sph,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesUsingCellsCategories(sph, categories, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoryIndoors(const vector3 &point,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategory(point, category, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result = indoors[i]->GetEntitiesUsingCellsCategory(point, category, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoriesIndoors(const vector3 &point,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_ONE_INDOOR) || (flags & nSpatialTypes::SPF_ALL_INDOORS));

    bool result(false);
    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            if (indoors[i]->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells))
            {
                return true;
            }
        }
    }
    else
    {
        const nArray<ncSpatialIndoor*> &indoors = nSpatialServer::Instance()->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); i++)
        {
            result |= indoors[i]->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells);
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoryOutdoors(const vector3 &point,
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesUsingCellsCategory(point, category, flags, entities, cells);
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialVisitor::GetEntitiesUsingCellsCategoriesOutdoors(const vector3 &point,
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert((flags & nSpatialTypes::SPF_OUTDOORS));
    ncSpatialQuadtree *outdoor = nSpatialServer::Instance()->GetQuadtreeSpace();
    bool result(false);
    if (outdoor)
    {
        result = outdoor->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells);
    }

    return result;
}




//------------------------------------------------------------------------------
/**
    Get the nearest entity to a given position. 
    
    It will ignore the entity with the given id. This is useful if we want 
    to find the nearest entity to a concrete one. In this case we have to 
    pass the id of this one.

    If we pass nEntityServer::IDINVALID it means that the algorithm will return the
    really nearest entity to that position.
*/
nEntityObject *
nSpatialVisitor::GetNearestEntityTo(const vector3 &pos, nEntityObjectId id) const
{
    sphere initSph(pos, N_MIN_SEARCH_SPHERE_RADIUS);
    float maxRadius = nSpatialServer::Instance()->GetWorldBBox().diagonal_size();
    
    if (maxRadius < initSph.r)
    {
        initSph.r = maxRadius;
    }

    return this->GetNearestEntityTo(initSph, id, maxRadius);
}

//------------------------------------------------------------------------------
/**
    Get the nearest entity to the center of the given sphere and into the 
    given sphere
    
    It will ignore the entity with the given id. This is useful if we want 
    to find the nearest entity to a concrete one. In this case we have to 
    pass the id of this one.

    If we pass nEntityServer::IDINVALID it means that the algorithm will return the
    really nearest entity to the sphere's center.
*/
nEntityObject *
nSpatialVisitor::GetNearestEntityTo(const sphere &sph, nEntityObjectId id) const
{
    sphere initSph(sph.p, N_MIN_SEARCH_SPHERE_RADIUS);
    float maxRadius = sph.r;
    
    if (sph.r < N_MIN_SEARCH_SPHERE_RADIUS)
    {
        initSph.r = sph.r;
    }

    return this->GetNearestEntityTo(initSph, id, maxRadius);
}

//------------------------------------------------------------------------------
/**
    Get the nearest entity to the given initial sphere's center. This initial sphere can grow
    until it reaches the given maximun radius.
*/
nEntityObject *
nSpatialVisitor::GetNearestEntityTo(const sphere &initSph, nEntityObjectId id, float maxRadius) const
{
    sphere sph(initSph);
    bool lastTime = false;
    while (sph.r <= maxRadius)
    {
        // collect all the entities in the indoors that are into the sphere
        static nArray<nEntityObject*> containedEntities;
        containedEntities.Reset();

        for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_spatialServer->m_indoorSpaces.Begin();
                                         pIndoorSpace != this->m_spatialServer->m_indoorSpaces.End();
                                         pIndoorSpace++)
        {
            (*pIndoorSpace)->SearchEntitiesContainedByPos(sph, &containedEntities);
        }

        // collect all the entities in the outdoors that are into the sphere
        if (this->m_spatialServer->m_outdoorSpace.isvalid())
        {
            ncSpatialQuadtree * outdoorSpace = this->m_spatialServer->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();
            outdoorSpace->SearchEntitiesContainedByPos(sph, &containedEntities);
        }

        if (!containedEntities.Empty())
        {
            vector3 distVect;
            float minSqrdDist = FLT_MAX;
            float sqrdDist;
            ncTransform *trComp = 0;
            nEntityObject *nearestEntity = 0;
            bool found = false;

            // search the nearest one
            for (nArray<nEntityObject*>::iterator pEntity  = containedEntities.Begin();
                                                  pEntity != containedEntities.End();
                                                  pEntity++)
            {
                if ((*pEntity)->GetId() == id)
                {
                    continue;
                }

                trComp = (*pEntity)->GetComponent<ncTransform>();
                if (trComp)
                {
                    distVect = trComp->GetPosition() - sph.p;
                    sqrdDist = distVect.lensquared();
                    if (sqrdDist < minSqrdDist)
                    {
                        minSqrdDist = sqrdDist;
                        nearestEntity = *pEntity;
                    }
                }

                found = true;
            }
            
            if (found)
            {
                return nearestEntity;
            }
        }
        
        if (lastTime || (sph.r == maxRadius))
        {
            break;
        }

        sph.r *= 2;
        if (sph.r > maxRadius)
        {
            lastTime = true;
            sph.r = maxRadius;
        }
    }

    return 0;
}

