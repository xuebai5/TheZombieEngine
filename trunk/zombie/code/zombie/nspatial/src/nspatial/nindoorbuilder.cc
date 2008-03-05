#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nindoorbuilder.cc
//  (C) 2004 Conjurer Services, S.A.
//  @author Miquel Angel Rujula <>
//------------------------------------------------------------------------------

#include "nspatial/nindoorbuilder.h"
#include "entity/nentityobjectserver.h"
#include "tools/nmeshbuilder.h"
#include "nspatial/nspatialindoorcell.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialindoorclass.h"
#include "nspatial/ncspatialindoor.h"
#include "mathlib/obbox.h"
#include "kernel/nlogclass.h"
#include "zombieentity/ncloaderclass.h"


NCREATELOGLEVEL ( indoorBuilderLog, "Indoor Builder", true, 1 ) 

//------------------------------------------------------------------------------
/**
    constructor 1
*/
nIndoorBuilder::nIndoorBuilder(nEntityClass *indoorClass):
    m_indoorClass(indoorClass)
{
    // also, we create an instance (entity) of this class to build the cells structure
    this->m_tempIndoor = nEntityObjectServer::Instance()->NewEntityObject("neindoor");

    this->m_cellBrushesInstancers = n_new(nKeyArray<nObjectInstancer*>(16, 16));
    this->m_cellPortalsInstancers = n_new(nKeyArray<nObjectInstancer*>(8, 8));
    this->m_cellPhysicsInstancers = n_new(nKeyArray<nObjectInstancer*>(16, 16));
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nIndoorBuilder::~nIndoorBuilder()
{
    this->m_tempIndoor->GetComponentSafe<ncSpatialSpace>()->DestroySpace();
    for (int i = 0; i < this->m_cellBrushesInstancers->Size(); i++)
    {
        this->m_cellBrushesInstancers->GetElementAt(i)->Release();
    }
    n_delete(this->m_cellBrushesInstancers);
    for (int i = 0; i < this->m_cellPortalsInstancers->Size(); i++)
    {
        this->m_cellPortalsInstancers->GetElementAt(i)->Release();
    }
    n_delete(this->m_cellPortalsInstancers);
    for (int i = 0; i < this->m_cellPhysicsInstancers->Size(); i++)
    {
        this->m_cellPhysicsInstancers->GetElementAt(i)->Release();
    }
    n_delete(this->m_cellPhysicsInstancers);
}

//------------------------------------------------------------------------------
/**
    set the indoor's shell
*/
void 
nIndoorBuilder::SetIndoorShell(const nString &name)
{
    n_assert2(this->m_indoorClass.isvalid(), 
              "miquelangel.rujula: indoor class is not set! Please, do it before setting the indoor shell's name.");
    this->m_indoorClass->GetComponent<ncSpatialIndoorClass>()->SetShellName(name.Get());
}

//------------------------------------------------------------------------------
/**
    save all the indoor information. Call it when the indoor is built.
*/
bool
nIndoorBuilder::Save()
{
    nString spatialPath = this->m_indoorClass->GetComponent<ncLoaderClass>()->GetResourceFile();
    spatialPath.Append("/spatial/");

    // save the indoor's cells structure
    if (!this->m_tempIndoor->GetComponentSafe<ncSpatialIndoor>()->SaveCellsStructure(spatialPath.Get()))
    {
        return false;
    }

    nString fileName;
    
    bool result = false;
    /// save all the indoor brushes intancers
    for (int i = 0; i < this->m_cellBrushesInstancers->Size(); i++)
    {
        fileName.Set(spatialPath.Get());
        fileName.Append("IBInstancer_");
        fileName.AppendInt(this->m_cellBrushesInstancers->GetKeyAt(i));
        fileName.Append(".n2");
        result = this->m_cellBrushesInstancers->GetElementAt(i)->SaveAs(fileName.Get());
        if (!result)
        {
            n_message("Can't save brushes instancer for the cell number %d", i);
        }
    }
    
    /// save all the portals instancers
    for (int i = 0; i < this->m_cellPortalsInstancers->Size(); i++)
    {
        fileName.Set(spatialPath.Get());
        fileName.Append("PortalsInstancer_");
        fileName.AppendInt(this->m_cellPortalsInstancers->GetKeyAt(i));
        fileName.Append(".n2");
        result = this->m_cellPortalsInstancers->GetElementAt(i)->SaveAs(fileName.Get());
        if (!result)
        {
            n_message("Can't save portals instancer for the cell number %d", i);
        }
    }

    /// save all the physics instancers (for the spatial cells, not the global indoor one)
    for (int i = 0; i < this->m_cellPhysicsInstancers->Size(); i++)
    {
        fileName.Set(spatialPath.Get());
        fileName.Append("PhysicsInstancer_");
        fileName.AppendInt(this->m_cellPhysicsInstancers->GetKeyAt(i));
        fileName.Append(".n2");
        result = this->m_cellPhysicsInstancers->GetElementAt(i)->SaveAs(fileName.Get());
        if (!result)
        {
            n_message("Can't save physics instancer for the cell number %d", i);
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    create a cell from a given mesh that wraps it
*/
bool 
nIndoorBuilder::CreateNewCell(nMeshBuilder& meshBuilder, int id)
{
    n_assert2( id != -1 , "miquelangel.rujula: wrong cell id!");

    if (!this->m_indoorClass.isvalid())
    {
        return false;
    }

    nSpatialIndoorCell *newCell = n_new(nSpatialIndoorCell);
    
    // set cell's identificator;
    newCell->SetId(id);

    // set cell's wrapper mesh
    nWrapperMesh& wrapperMesh = newCell->GetWrapperMesh();
    wrapperMesh.SetMeshBuilder(meshBuilder);

    // cell's bounding box will be wrapper mesh's bounding box
    bbox3 cellBox = meshBuilder.GetBBox();
    newCell->SetOriginalBBox(cellBox);

    // put the cell's bounding box with its original bounding box to situate objects
    // during exportation
    newCell->SetBBox(cellBox);

    ncSpatialIndoor* spatialIndoor = this->m_tempIndoor->GetComponentSafe<ncSpatialIndoor>();

    // finally, add the new cell to the space
    spatialIndoor->AddSpatialCell(newCell);
    // put the temporal indoor's bounding box with its original bounding box to situate objects
    // during exportation
    spatialIndoor->SetBBox(spatialIndoor->GetOriginalBBox());
    this->m_indoorClass->GetComponentSafe<ncSpatialIndoorClass>()->GetOriginalBBox().extend(newCell->GetOriginalBBox());

    //Create always instancer
    nObjectInstancer *instancer;
    instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->New("nobjectinstancer"));
    instancer->Init(32, 32);
    this->m_cellBrushesInstancers->Add(id, instancer);
    return true;
}

//------------------------------------------------------------------------------
/**
    insert a new object in the space without setting the rendercontext's root
*/
bool
nIndoorBuilder::InsertNewObject(nEntityObject *indoorBrush, vector3 &point)
{
    n_assert2(indoorBrush, "miquelangel.rujula: trying to insert an indoor brush whose pointer is NULL!");

    n_assert2(this->m_indoorClass.isvalid(), 
              "miquelangel.rujula: indoor class doesn't exist!");

    n_assert2(this->m_tempIndoor.isvalid(), 
              "miquelangel.rujula: temporal indoor not created!");

    // insert the new indoor brush into the temporal indoor
    ncSpatialCell *cell = this->m_tempIndoor->GetComponentSafe<ncSpatialSpace>()->SearchCellContaining(point, 0);
    if (cell)
    {
        n_verify(cell->AddEntity(indoorBrush));
    }
    else
    {
        return false;
    }

    // add the new indoor brush to the indoor class' instancer
    this->AddBrushToInstancer(indoorBrush->GetComponentSafe<ncSpatial>()->GetCell()->GetId(), indoorBrush);

    return true;
}

//------------------------------------------------------------------------------
/**
    insert a physics object
*/
bool 
nIndoorBuilder::InsertPhysicsObject(nObject *object, bbox3 &box)
{
    n_assert2(object, 
              "miquelangel.rujula: NULL pointer to physics object, when trying to insert it into an indoor!");

    n_assert2(this->m_tempIndoor.isvalid(), "miquelangel.rujula: temporal indoor not created!");

    
    ncSpatialCell *spatialCell = this->m_tempIndoor->GetComponentSafe<ncSpatialIndoor>()->SearchCellContaining(box);
    if (spatialCell)
    {
        this->AddPhysicsObjectToInstancer(spatialCell->GetId(), object);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Determine the cell that contains the given point.
    Returns the cell id.
*/
int
nIndoorBuilder::GetContainerCellId(const vector3 &point)
{
    n_assert2(this->m_indoorClass.isvalid(), 
              "miquelangel.rujula: indoor class doesn't exist!");
    
    n_assert2(this->m_tempIndoor.isvalid(), 
              "miquelangel.rujula: temporal indoor not created!");

    // search the cell that contains the given point
    ncSpatialCell *containerCell = this->m_tempIndoor->GetComponentSafe<ncSpatialIndoor>()->SearchCellContaining(point, 0);

    if (!containerCell)
    {
        // cell not found
        return -1;
    }

    // we've found the cell, return its id
    return containerCell->GetId();
}

//------------------------------------------------------------------------------
/**
    set the portals' class
*/
void 
nIndoorBuilder::SetPortalsClass(nEntityClass* portalClass)
{
    this->m_portalsClass = portalClass;
}

//------------------------------------------------------------------------------
/**
    create and insert a portal in a cell
*/
ncSpatialPortal*
nIndoorBuilder::InsertPortalInCell(nSpatialIndoorCell *parentCell,
                                   bool active, 
                                   float deactDist)
{
    n_assert2(parentCell, "miquelangel.rujula: trying to insert a portal into a cell whose pointer is null!");
    n_assert2(this->m_portalsClass.isvalid(), "miquelangel.rujula: portal entities' class is not set!");

    // create a new portal
    nEntityObject *newPortal = nEntityObjectServer::Instance()->NewEntityObject(this->m_portalsClass->nClass::GetName());
    ncSpatialPortal *spatialPortal = newPortal->GetComponentSafe<ncSpatialPortal>();

    // add the portal to the cell
    n_verify(parentCell->AddEntity(newPortal));

    // add the portal to the corresponding instancer
    this->AddPortalToInstancer(parentCell->GetId(), newPortal);

    // set the active flag
    spatialPortal->SetActive(active);

    // set deactivation distance
    spatialPortal->SetDeactivationDistance(deactDist);

    return spatialPortal;
}

//------------------------------------------------------------------------------
/**
    create and insert a portal in the outdoor
*/
ncSpatialPortal*
nIndoorBuilder::InsertPortalOutdoor(bool active,
                                    float deactDist)
{
    n_assert2(this->m_portalsClass.isvalid(), "miquelangel.rujula: portal entities' class is not set!");

    // create a new portal
    nEntityObject *newPortal = nEntityObjectServer::Instance()->NewEntityObject(this->m_portalsClass->nClass::GetName());
    ncSpatialPortal *spatialPortal = newPortal->GetComponentSafe<ncSpatialPortal>();

    // add the portal to the corresponding instancer
    this->AddPortalToInstancer(0, newPortal);

    // set the active flag
    spatialPortal->SetActive(active);

    // set the deactivation distance
    spatialPortal->SetDeactivationDistance(deactDist);

    return spatialPortal;
}

//------------------------------------------------------------------------------
/**
    Reverse a list of portal vertices
*/
void
nIndoorBuilder::ReverseVertices(const vector3 portalVertices[4], vector3 *reversedVertices)
{
    n_assert(reversedVertices);
    reversedVertices[0] = portalVertices[1];
    reversedVertices[1] = portalVertices[0];
    reversedVertices[2] = portalVertices[3];
    reversedVertices[3] = portalVertices[2];
}

//------------------------------------------------------------------------------
/**
    Insert a new portal into the space.
    It duplicates the portal, putting one in each cell, but each one poiting to the
    other cell.

    A cell id equal to zero means that is an external cell/space.
*/
bool 
nIndoorBuilder::InsertNewPortal(const vector3 portalVertices[4],
                                int cellId1, 
                                int cellId2,
                                bool c1ToC2Active,
                                bool c2ToC1Active,
                                ncSpatialPortal* &portal1,
                                ncSpatialPortal* &portal2,
                                float deactDist1,
                                float deactDist2)
{
    n_assert2(this->m_indoorClass.isvalid(), "miquelangel.rujula: indoor class doesn't exist!");
    n_assert2(this->m_tempIndoor.isvalid(),  "miquelangel.rujula: temporal indoor not created!");

    if ( cellId1 == -1 || cellId2 == -1 )
    {   
        // not a valid cell id
        return false;
    }

    ncSpatialIndoor * spatialIndoor = this->m_tempIndoor->GetComponentSafe<ncSpatialIndoor>();

    nSpatialIndoorCell *cell1 = static_cast<nSpatialIndoorCell*>(spatialIndoor->SearchCellById(cellId1));
    nSpatialIndoorCell *cell2 = static_cast<nSpatialIndoorCell*>(spatialIndoor->SearchCellById(cellId2));
    
    portal1 = 0;
    portal2 = 0;

    // initialize the portal's vertices and plane information
    PortalInfo portInfo;
    PortalInfo reversedPortInfo;

    portInfo.vertices[0] = portalVertices[1];
    portInfo.vertices[1] = portalVertices[0];
    portInfo.vertices[2] = portalVertices[3];
    portInfo.vertices[3] = portalVertices[2];
    portInfo.p = plane(portalVertices[1], portalVertices[2], portalVertices[0]);

    this->ReverseVertices(portInfo.vertices, reversedPortInfo.vertices);
    reversedPortInfo.p.set(-portInfo.p.a, -portInfo.p.b, -portInfo.p.c, -portInfo.p.d);

    float dist1 = -1.0f;
    float dist2 = -1.0f;

    vector3 clipRectCenter;
    vector3 contactPoints1[4];
    vector3 contactPoints2[4];
    for (int i = 0; i < 4; i++)
    {
        // acummulate clip rectangle's positions to calculate the average
        clipRectCenter += portInfo.vertices[i];

        // initialize contact points of the oriented bounding box to the portal's clip rectangle
        contactPoints1[i] = portInfo.vertices[i];
        contactPoints2[i] = portInfo.vertices[i];
    }

    // calculate the average point, this is, the clip rectangle's center
    clipRectCenter *= 0.25f;

    // create necessary portals
    if (cell1)
    {
        portal1 = this->InsertPortalInCell(cell1, c1ToC2Active, deactDist1);
        if (!portal1)
        {
            return false;
        }

        dist1 = cell1->GetWrapperMesh().GetIntersectionDist(clipRectCenter, portInfo.p.normal());
    }

    if (cell2)
    {
        portal2 = this->InsertPortalInCell(cell2, c2ToC1Active, deactDist2);
        if (!portal2)
        {
            if (portal1)
            {
                this->RemovePortalFromInstancer(cellId1, portal1->GetEntityObject());
                portal1->GetCell()->RemoveEntity(portal1->GetEntityObject());
            }
            return false;
        }
        dist2 = cell2->GetWrapperMesh().GetIntersectionDist(clipRectCenter, portInfo.p.normal());
    }

    bool portal1Contains = false;
    bool portal2Contains = false;
    bool putDirectInfo = false;

    if (portal1 && portal2)
    {
        // assign twin portals
        portal1->SetTwinPortal(portal2);
        portal2->SetTwinPortal(portal1);
        
        n_assert2((dist1 != -1 || dist2 != -1), "miquelangel.rujula");

        if (dist1 < dist2)
        {
            if (cell1->GetWrapperMesh().Contains(clipRectCenter))
            {
                portal1Contains = true;
                portal1->SetOriginalPlane(reversedPortInfo.p);
                portal1->SetOriginalVertices(reversedPortInfo.vertices);
                portal2->SetOriginalPlane(portInfo.p);
                portal2->SetOriginalVertices(portInfo.vertices);
            }
            else
            {
                portal1->SetOriginalPlane(portInfo.p);
                portal1->SetOriginalVertices(portInfo.vertices);
                portal2->SetOriginalPlane(reversedPortInfo.p);
                portal2->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }
        else
        {
            if (cell2->GetWrapperMesh().Contains(clipRectCenter))
            {
                portal2Contains = true;
                portal2->SetOriginalPlane(portInfo.p);
                portal2->SetOriginalVertices(portInfo.vertices);
                portal1->SetOriginalPlane(reversedPortInfo.p);
                portal1->SetOriginalVertices(reversedPortInfo.vertices);
            }
            else
            {
                portal2->SetOriginalPlane(portInfo.p);
                portal2->SetOriginalVertices(portInfo.vertices);
                portal1->SetOriginalPlane(reversedPortInfo.p);
                portal1->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }

        // check if both portal's planes are opposited
        plane plane1(portal1->GetOriginalPlane());
        vector3 dir1 = plane1.normal();
        plane plane2(portal2->GetOriginalPlane());
        vector3 dir2 = plane2.normal();

        if (dir1.isequal(dir2, 0.000001f))
        {
            // both planes are oriented to the same direction
            n_message("ERROR: twin portals for cells %d and %d have the same plane!!",
                      portal1->GetCell()->GetId(), portal2->GetCell()->GetId());
            this->RemovePortalFromInstancer(cellId1, portal1->GetEntityObject());
            portal1->GetCell()->RemoveEntity(portal1->GetEntityObject());
            this->RemovePortalFromInstancer(cellId2, portal2->GetEntityObject());
            portal2->GetCell()->RemoveEntity(portal2->GetEntityObject());
            return false;
        }
    }
    else if (portal1 && !portal2)
    {
        float otherDirDist = cell1->GetWrapperMesh().GetIntersectionDist(clipRectCenter, 
                                                                         reversedPortInfo.p.normal());
        if (otherDirDist < dist1)
        {
            if (cell1->GetWrapperMesh().Contains(clipRectCenter))
            {
                portal1Contains = true;
                putDirectInfo = false;
                portal1->SetOriginalPlane(portInfo.p);
                portal1->SetOriginalVertices(portInfo.vertices);
            }
            else
            {
                putDirectInfo = true;
                portal1->SetOriginalPlane(reversedPortInfo.p);
                portal1->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }
        else
        {
            if (!cell1->GetWrapperMesh().Contains(clipRectCenter))
            {
                putDirectInfo = false;
                portal1->SetOriginalPlane(portInfo.p);
                portal1->SetOriginalVertices(portInfo.vertices);
            }
            else
            {
                portal1Contains = true;
                putDirectInfo = true;
                portal1->SetOriginalPlane(reversedPortInfo.p);
                portal1->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }
    }
    else // portal2 && !portal1
    {
        n_assert2((portal2 && !portal1), "miquelangel.rujula");

        float otherDirDist = cell2->GetWrapperMesh().GetIntersectionDist(clipRectCenter, 
                                                                         reversedPortInfo.p.normal());
        if (otherDirDist < dist2)
        {
            if (cell2->GetWrapperMesh().Contains(clipRectCenter))
            {
                portal2Contains = true;
                putDirectInfo = false;
                portal2->SetOriginalPlane(portInfo.p);
                portal2->SetOriginalVertices(portInfo.vertices);
            }
            else
            {
                putDirectInfo = true;
                portal2->SetOriginalPlane(reversedPortInfo.p);
                portal2->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }
        else
        {
            if (!cell2->GetWrapperMesh().Contains(clipRectCenter))
            {
                putDirectInfo = false;
                portal2->SetOriginalPlane(portInfo.p);
                portal2->SetOriginalVertices(portInfo.vertices);
            }
            else
            {
                putDirectInfo = true;
                portal2->SetOriginalPlane(reversedPortInfo.p);
                portal2->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }
    }
    
    // calculate the oriented bounding box
    obbox3 obox;
    obox.SetTranslation(clipRectCenter);
    obox.SetScale(vector3(1,1,1));
    
    // calculate oriented bounding box orientation
    vector3 a = portalVertices[0] - portalVertices[1];
    vector3 b = portalVertices[2] - portalVertices[1];
    vector3 c = a * b;
    a = b * c;

    matrix33 orientMatrix(a, b, c);

    obox.SetEuler(orientMatrix.to_euler());

    float pointDist;
    if (portal1)
    {
        if (!portal1Contains)
        {
            // extend the obbox to the portal1 direction
            for (int i = 0; i < 4; i++)
            {
                pointDist = cell1->GetWrapperMesh().GetIntersectionDist(contactPoints1[i], 
                                                                        portal1->GetOriginalPlane().normal());
                if (pointDist > (FLT_MAX / 2.0f))
                {
                    pointDist = cell1->GetWrapperMesh().GetIntersectionDist(clipRectCenter, 
                                                                            portal1->GetOriginalPlane().normal());
                    if (pointDist > (FLT_MAX / 2.0f))
                    {
                        NLOG( indoorBuilderLog,  
                              (0, "Can't calculate obbox! There's a portal whose center isn't pointing to any of the cells that connects, or the cells are touching between them.") );
                        this->RemovePortalFromInstancer(cellId1, portal1->GetEntityObject());
                        portal1->GetCell()->RemoveEntity(portal1->GetEntityObject());
                        if (portal2)
                        {
                            this->RemovePortalFromInstancer(cellId2, portal2->GetEntityObject());
                            portal2->GetCell()->RemoveEntity(portal2->GetEntityObject());
                        }
                        return false;
                    }
                }

                pointDist += N_OBBOX_MARGIN;
                vector3 portalPlaneNormal = portal1->GetOriginalPlane().normal();
                portalPlaneNormal.norm();
                portalPlaneNormal *= pointDist;
                contactPoints1[i] += portalPlaneNormal;
                obox.extents(contactPoints1[i]);
            }
        }
    }

    if (portal2)
    {
        if (!portal2Contains)
        {
            // extend the obbox to the portal2 direction
            for (int i = 0; i < 4; i++)
            {
                pointDist = cell2->GetWrapperMesh().GetIntersectionDist(contactPoints2[i], 
                                                                        portal2->GetOriginalPlane().normal());
                if (pointDist > (FLT_MAX / 2.0f))
                {
                    pointDist = cell2->GetWrapperMesh().GetIntersectionDist(clipRectCenter, 
                                                                            portal2->GetOriginalPlane().normal());
                    if (pointDist > (FLT_MAX / 2.0f))
                    {
                        NLOG( indoorBuilderLog,  
                              (0, "Can't calculate obbox! There's a portal whose center isn't pointing to any of the cells that connects, or the cells are touching between them.") );
                        this->RemovePortalFromInstancer(cellId2, portal2->GetEntityObject());
                        portal2->GetCell()->RemoveEntity(portal2->GetEntityObject());
                        if (portal1)
                        {
                            this->RemovePortalFromInstancer(cellId1, portal1->GetEntityObject());
                            portal1->GetCell()->RemoveEntity(portal1->GetEntityObject());
                        }
                        return false;
                    }
                }

                pointDist += N_OBBOX_MARGIN;
                vector3 portalPlaneNormal = portal2->GetOriginalPlane().normal();
                portalPlaneNormal.norm();
                portalPlaneNormal *= pointDist;
                contactPoints2[i] += portalPlaneNormal;
                obox.extents(contactPoints2[i]);
            }
        }
    }

    if (cellId1 == 0)
    {
        n_assert(!portal1);

        // portal1 is in the outdoor, let's create it
        portal1 = this->InsertPortalOutdoor(c1ToC2Active, deactDist1);
        if (!portal1)
        {
            if (portal2)
            {
                this->RemovePortalFromInstancer(cellId2, portal2->GetEntityObject());
                portal2->GetCell()->RemoveEntity(portal2->GetEntityObject());
            }
            return false;
        }
        else
        {
            if (putDirectInfo == true)
            {
                portal1->SetOriginalPlane(portInfo.p);
                portal1->SetOriginalVertices(portInfo.vertices);
            }
            else
            {
                portal1->SetOriginalPlane(reversedPortInfo.p);
                portal1->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }
    }

    if (cellId2 == 0)
    {
        n_assert(!portal2);

        // portal2 is in the outdoor, let's create it
        portal2 = this->InsertPortalOutdoor(c2ToC1Active, deactDist2);
        if (!portal2)
        {
            if (portal1)
            {
                this->RemovePortalFromInstancer(cellId1, portal1->GetEntityObject());
                portal1->GetCell()->RemoveEntity(portal1->GetEntityObject());
            }
            return false;
        }
        else
        {
            if (putDirectInfo == true)
            {
                portal2->SetOriginalPlane(portInfo.p);
                portal2->SetOriginalVertices(portInfo.vertices);
            }
            else
            {
                portal2->SetOriginalPlane(reversedPortInfo.p);
                portal2->SetOriginalVertices(reversedPortInfo.vertices);
            }
        }
    }

    n_assert(portal1 && portal2);
    // set twin portals
    portal1->SetTwinPortal(portal2);
    portal2->SetTwinPortal(portal1);

    // set the oriented bounding box
    portal1->SetOriginalOBBox(obox);
    portal2->SetOriginalOBBox(obox);

    return true;
}

