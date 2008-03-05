#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialquadtreecell.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialquadtreecell.h"
#include "variable/nvariableserver.h"
#include "entity/nentity.h"
#include "entity/nentityobjectserver.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialquadtree.h"
#include "nscene/ncscene.h"
#ifndef __ZOMBIE_EXPORTER__
#include "ngeomipmap/ncterraingmmcell.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialQuadtreeCell, ncSpatialCell);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialQuadtreeCell)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    constructor
*/
ncSpatialQuadtreeCell::ncSpatialQuadtreeCell():
    ncSpatialCell(),
    m_parentCell(0),
    m_visTerrainIndex(-1),
    m_active(true),
    m_depth(-1),
    visibleTerrainFrameId(-1),
    lastTerrainVisibleCam(nEntityObjectServer::IDINVALID),
    bx(-1),
    bz(-1)
{
    this->m_cellType = ncSpatialCell::N_QUADTREE_CELL;

    // initialize subcells pointers
    for (int i(0); i < 4; ++i)
    {
        this->m_cellsArray[i] = 0;
    }

    // set to zero the number of subcells
    this->m_numSubcells = 0;
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSpatialQuadtreeCell::~ncSpatialQuadtreeCell()
{
    this->DestroyCell();
    n_assert2(this->m_numSubcells == 0, "miquelangel.rujula");
}

//------------------------------------------------------------------------------
/**
    flush all the lights in the cell
*/
void 
ncSpatialQuadtreeCell::FlushAllLights()
{
    ncSpatialCell::FlushAllLights();
    if ( !this->IsLeaf() )
    {
        this->m_cellsArray[0]->FlushAllLights();
        this->m_cellsArray[1]->FlushAllLights();
        this->m_cellsArray[2]->FlushAllLights();
        this->m_cellsArray[3]->FlushAllLights();
    }
}

//------------------------------------------------------------------------------
/**
    destroy the cell and all its entities
*/
void
ncSpatialQuadtreeCell::DestroyCell()
{
    ncSpatialCell::DestroyCell();

    // destroy subcells
    if (this->m_numSubcells > 0)
    {
        n_assert2((this->m_numSubcells == 4), 
                  "miquelangel.rujula: quadtree cell has childs, but not exactly 4!");

        // destroy subcells
        for (int i = 0; i < 4; i++)
        {
            this->m_cellsArray[i]->DestroyCell();
            if ( this->m_cellsArray[i]->GetEntityObject() )
            {
                this->m_cellsArray[i]->GetEntityObject()->Release();
                nEntityObjectServer::Instance()->RemoveEntityObject(this->m_cellsArray[i]->GetEntityObject());
            }
            else
            {
                n_delete(this->m_cellsArray[i]);
            }
        }
    }
    
    this->m_numSubcells = 0;

    // clear scene links to lights
    this->ClearLinks();
}

//------------------------------------------------------------------------------
/**
    set the bounding box of the cell
*/
void 
ncSpatialQuadtreeCell::SetBBox(const bbox3 &box)
{
    this->m_bbox = box;
    this->m_sph.set(box.center(), box.diagonal_size() * 0.5f);
    this->bboxCenter = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();
}

//------------------------------------------------------------------------------
/**
    It adds a subcell to the subcells array
*/
bool 
ncSpatialQuadtreeCell::AddSubCell(ncSpatialQuadtreeCell *subCell)
{
    n_assert2(subCell, "miquelangel.rujula");
    n_assert2((this->m_numSubcells < 5), 
              "miquelangel.rujula: spatial quadtree cell has more than 4 childs!");
    n_assert2( this->GetDepth() == (subCell->GetDepth() - 1), "Trying to add a subcell with wrong depth!" );

    if ( this->m_numSubcells == 4)
    {
        return false;
    }

    // insert the subcell
    this->m_cellsArray[this->m_numSubcells] = subCell;
    nEntityObject *cellEntity = subCell->GetEntityObject();
    if ( cellEntity )
    {
        cellEntity->AddRef();
    }

    // increase the number of subcells
    this->m_numSubcells++;

    n_assert2(this->m_parentSpace, "miquelangel.rujula: trying to add a subcell to a cell that has no space!");
    
    // set its parent space
    ncSpatialQuadtree *quadtreeSpace = static_cast<ncSpatialQuadtree*>(this->m_parentSpace);
    subCell->SetParentSpace(quadtreeSpace);
    
    return true;
}

//------------------------------------------------------------------------------
/**
    remove a quadtree cell
*/
bool 
ncSpatialQuadtreeCell::RemoveSubcell(ncSpatialQuadtreeCell *cell)
{
    n_assert2(cell, "miquelangel.rujula: trying to remove a cell whose pointer is NULL!");

    if (this->m_numSubcells > 0)
    {
        n_assert2((this->m_numSubcells == 4), 
                  "miquelangel.rujula: quadtree cell has childs, but not exactly 4!");

        // search the cell in the subcells array
        for (int i = 0; i < 4; i++)
        {
            if (this->m_cellsArray[i] == cell)
            {
                this->m_cellsArray[i]->DestroyCell();
                this->m_cellsArray[i] = 0;
                return true;
            }
        }
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    Searches a cell with the given identifier into this subtree
*/
ncSpatialCell* 
ncSpatialQuadtreeCell::SearchCellById(int cellId)
{
    n_assert2( cellId > -1, "miquelangel.rujula: wrong cell id!");

    if ( cellId == 0 )
    {
        return NULL;
    }

    if (this->m_cellId == cellId)
    {
        return this;
    }

    if (!this->IsLeaf())
    {
        ncSpatialCell *cell = 0;
        for (int i = 0; i < 4; i++)
        {
            cell = this->m_cellsArray[i]->SearchCellById(cellId);
            if (cell)
            {
                return cell;
            }
        }
    }
    
    return NULL;
}

//------------------------------------------------------------------------------
/**
    get the number of entities in the cell
*/
int
ncSpatialQuadtreeCell::GetTotalNumEntities()
{
    int totalNumEntities = ncSpatialCell::GetNumEntities();
    if (this->m_numSubcells > 0)
    {
        n_assert2((this->m_numSubcells == 4), "miquelangel.rujula: quadtree cell hasn't 4 childs nodes!");
        for (int i = 0; i < 4; i++)
        {
            ncSpatialQuadtreeCell *childCell = this->m_cellsArray[i];
            totalNumEntities += childCell->GetTotalNumEntities();
        }
    }

    return totalNumEntities;
}

//------------------------------------------------------------------------------
/**
    set this cell as visible from the given camera id in this frame
*/
void 
ncSpatialQuadtreeCell::SetVisibleSubtreeBy(nEntityObjectId cameraId)
{
    // set this cell as visible
    this->SetVisibleFrameId(nSpatialServer::Instance()->GetFrameId());
    this->SetLastVisibleCam(cameraId);
    
    // set child cells as visibles
    if (!this->IsLeaf())
    {
        nVisibleFrustumVisitor &visitor = nSpatialServer::Instance()->GetVisibilityVisitor();
        if (this->m_cellsArray[0]->IsVisibleBy(cameraId))
        {
            visitor.RemoveVisibleTerrainCell(this->m_cellsArray[0]);
        }
        else
        {
            this->m_cellsArray[0]->SetVisibleSubtreeBy(cameraId);
        }

        if (this->m_cellsArray[1]->IsVisibleBy(cameraId))
        {
            visitor.RemoveVisibleTerrainCell(this->m_cellsArray[1]);
        }
        else
        {
            this->m_cellsArray[1]->SetVisibleSubtreeBy(cameraId);
        }

        if (this->m_cellsArray[2]->IsVisibleBy(cameraId))
        {
            visitor.RemoveVisibleTerrainCell(this->m_cellsArray[2]);
        }
        else
        {
            this->m_cellsArray[2]->SetVisibleSubtreeBy(cameraId);
        }

        if (this->m_cellsArray[3]->IsVisibleBy(cameraId))
        {
            visitor.RemoveVisibleTerrainCell(this->m_cellsArray[3]);
        }
        else
        {
            this->m_cellsArray[3]->SetVisibleSubtreeBy(cameraId);
        }
    }
}

//------------------------------------------------------------------------------
/**
    set the entity as linked to the given light in this frame
*/
void 
ncSpatialQuadtreeCell::SetLinkedTo(nEntityObjectId currentLight)
{
    this->m_lastLinkLight = currentLight;
    this->m_linkLightFrameId = nSpatialServer::Instance()->GetFrameId();
}

//------------------------------------------------------------------------------
/**
    says if the entity was linked to the given light in this frame
*/
bool 
ncSpatialQuadtreeCell::IsLinkedTo(nEntityObjectId currentLight)
{
    return (this->m_linkLightFrameId == nSpatialServer::Instance()->GetFrameId() &&
            this->m_lastLinkLight == currentLight);
}

//------------------------------------------------------------------------------
/**
    clear all the scene links of this entity
*/
void
ncSpatialQuadtreeCell::ClearLinks()
{
    ncScene *sceneComp = this->GetComponent<ncScene>();
    if ( sceneComp )
    {
        ncSpatialLight *spatialLight;
        int numLinks = sceneComp->GetNumLinks();
        nEntityObject *link;
        for ( int i(0); i < numLinks; ++i )
        {
            link = sceneComp->GetLinkAt(i);
            spatialLight = link->GetComponentSafe<ncSpatialLight>();
            spatialLight->RemoveLinkedEntity(this->GetEntityObject());
        }

        sceneComp->ClearLinks();
    }
}

//------------------------------------------------------------------------------
/**
    adjust cells' bounding box with the given offset and subcells' ones
*/
void 
ncSpatialQuadtreeCell::AdjustBBox(float offset)
{
#ifndef __ZOMBIE_EXPORTER__
    if ( this->GetComponent<ncTerrainGMMCell>() )
    {
        this->m_bbox = this->GetComponent<ncTerrainGMMCell>()->GetBBox();
    }
    
    this->m_bbox.vmax.y += offset;
    this->m_bbox.vmin.y -= ncSpatialQuadtree::BOXMARGIN;
    if (this == static_cast<ncSpatialQuadtree*>(this->GetParentSpace())->GetRootCell())
    {
        this->m_bbox.vmin.x -= 0.5f;
        this->m_bbox.vmin.z -= 0.5f;
        this->m_bbox.vmax.x += 0.5f;
        this->m_bbox.vmax.z += 0.5f;
    }
    this->bboxCenter = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();

    if (!this->IsLeaf())
    {
        for (int i(0); i < 4; ++i)
        {
            this->m_cellsArray[i]->AdjustBBox(offset);
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
    visibility visitor processing for a quadtree cell.
*/
void 
ncSpatialQuadtreeCell::Accept(nVisibleFrustumVisitor &visitor)
{
    visitor.AddVisibleTerrainCell(this);
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncSpatialQuadtreeCell::ReadStaticInfo(const TiXmlElement *cellElem)
{
    n_assert2(cellElem, "miquelangel.rujula");

    char smallCharBuff[5];

    // cell's transformation matrix
    matrix44 cellMatrix;
    TiXmlElement *cellMatrixElem = cellElem->FirstChildElement("TransformMatrix");
    if (!cellMatrixElem)
    {
        return false;
    }

    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            sprintf(smallCharBuff, "M%d%d", i, j);
            cellMatrix.m[i][j] = (float)atof(cellMatrixElem->Attribute(smallCharBuff));
        }
    }
    this->SetTransformMatrix(cellMatrix);

    // read the bbox information
    TiXmlElement *bboxElem = cellElem->FirstChildElement("BBox");
    bbox3 box;
    box.vmin.x = (float)atof(bboxElem->Attribute("VminX"));
    box.vmin.y = (float)atof(bboxElem->Attribute("VminY"));
    box.vmin.z = (float)atof(bboxElem->Attribute("VminZ"));
    box.vmax.x = (float)atof(bboxElem->Attribute("VmaxX"));
    box.vmax.y = (float)atof(bboxElem->Attribute("VmaxY"));
    box.vmax.z = (float)atof(bboxElem->Attribute("VmaxZ"));
    this->SetBBox(box);

    // process subcells, if any
    TiXmlElement *subCellElem = cellElem->FirstChildElement("Cell");
    if (subCellElem)
    {   // it has subcells, read them
        for (int i=0; i<4; i++)
        {
            // create a new quadtree cell
            ncSpatialQuadtreeCell *newSubCell = n_new(ncSpatialQuadtreeCell);
            newSubCell->SetParentSpace(this->m_parentSpace);
            // fill the new quadtree cell with the information contained in subCellElem
            if (!newSubCell->ReadStaticInfo(subCellElem))
            {
                return false;
            }

            if (!this->AddSubCell(newSubCell))
            {
                return false;
            }

            // get next subCellElem
            subCellElem = subCellElem->NextSiblingElement("Cell");
        }

        if (subCellElem)
        {   // it means that there are more than 4 childs for this quadtree node!!
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncSpatialQuadtreeCell::WriteStaticInfo(TiXmlElement *cellElem)
{
    n_assert2(cellElem, "miquelangel.rujula");

    // fill the TiXmlElement with the information inherited from the parent
    if (ncSpatialCell::WriteStaticInfo(cellElem))
    {
        if (this->m_numSubcells > 0)
        {
            n_assert2((this->m_numSubcells == 4), 
                    "miquelangel.rujula: quadtree cell has childs, but not exactly 4!");

            for (int i = 0; i < 4; i++)
            {
                TiXmlElement subCellElem("Cell");
                // fill the TiXmlElement corresponding to the cell with its info
                if (!this->m_cellsArray[i]->WriteStaticInfo(&subCellElem))
                {
                    return false;
                }
                cellElem->InsertEndChild(subCellElem);
            }
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialQuadtreeCell::SaveCmds(nPersistServer *ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    save the entities contained into this cell
*/
void
ncSpatialQuadtreeCell::Save(WizardEntityArray &wizards)
{
    ncSpatialCell::Save(wizards);

    if (!this->IsLeaf())
    {
        for (int i = 0; i < 4; i++)
        {
            this->m_cellsArray[i]->Save(wizards);
        }
    }
}

//------------------------------------------------------------------------------
/**
    load the entities contained into this cell
*/
bool 
ncSpatialQuadtreeCell::Load(const char *spatialPath)
{
    bool result = ncSpatialCell::Load(spatialPath);

    if (!this->IsLeaf())
    {
        for (int i = 0; i < 4; i++)
        {
            result = this->m_cellsArray[i]->Load(spatialPath);
        }
    }

    return result;
}
