#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialportal.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/nspatialvisitor.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "entity/nentityobjectserver.h"

#define PORTAL_EPSILON 0.002f

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialPortal, ncSpatial);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialPortal)
    NSCRIPT_ADDCMD_COMPOBJECT('RSOI', void, SetOtherSideCellId, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSCV', void, SetClipVertex, 2, (int, const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSPP', void, SetOriginalPlane, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSOZ', void, SetOrigOBBoxSize, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSOA', void, SetOrigOBBoxScale, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSOE', void, SetOrigOBBoxEuler, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSOT', void, SetOrigOBBoxTranslation, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RATP', void, Activate, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RDTP', void, Deactivate, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSAF', void, SetActive, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSAT', void, SetActiveAndTwin, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RIPA', bool, IsActive, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSDD', void, SetDeactivationDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGDD', float, GetDeactivationDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGSD', float, GetSqDeactivationDistance, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor 1
*/
ncSpatialPortal::ncSpatialPortal() : 
ncSpatial(),
m_otherSide(NULL),
m_twinPortal(NULL),
m_otherSideId(0),
m_visitFrameId(-1),
m_lightFrameId(-1),
m_lastLight(nEntityObjectServer::IDINVALID),
m_lastLightCam(nEntityObjectServer::IDINVALID),
m_active(true),
m_sqDeactivationDist(FLT_MAX),
brushes(0)
{
    // set type
    this->m_spatialComponentType = ncSpatial::N_SPATIAL_PORTAL;

    // initialize vertices array
    this->m_vertices = n_new_array(vector3, 4);
    // initialize original vertices array
    this->m_originalVertices = n_new_array(vector3, 4);
}

//------------------------------------------------------------------------------
/**
    Check for the intersection of the clip rectangle with a view volume 
    defined by a view-projection matrix.
*/
bbox3::ClipStatus
ncSpatialPortal::ClipStatus(const matrix44 &viewProjection) const
{
    int andFlags = 0xffff;
    int orFlags  = 0;
    int i;
    vector4 v0;
    vector4 v1;
    for (i = 0; i < 4; i++)
    {
        int clip = 0;
        v0.set(this->m_vertices[i]);
        v0.w = 1.0f;

        v1 = viewProjection * v0;

        // special case: a point is behind the camera
        if (v1.w <= 0)
            return bbox3::Inside;

        if (v1.x < -v1.w)       
            clip |= bbox3::ClipLeft;
        else if (v1.x > v1.w)   
            clip |= bbox3::ClipRight;

        if (v1.y < -v1.w)       
            clip |= bbox3::ClipBottom;
        else if (v1.y > v1.w)   
            clip |= bbox3::ClipTop;

        if (v1.z < -v1.w)       
            clip |= bbox3::ClipNear;
        else if (v1.z > v1.w)   
            clip |= bbox3::ClipFar;

        andFlags &= clip;
        orFlags  |= clip;
    }
    if (0 == orFlags)      
        return bbox3::Inside;
    else if (0 != andFlags) 
        return bbox3::Outside;
    else                    
        return bbox3::Clipped;
}

//------------------------------------------------------------------------------
/**
    Check for the intersection of the clip rectangle with a view volume defined 
    by a view-projection matrix and a rectangle.
    If 'useRect' is true, 'rect' will be used to perform the clip test.
*/
bbox3::ClipStatus
ncSpatialPortal::ClipStatus(const matrix44& viewProjection, const rectangle& rect, bool useRect) const
{
    int andFlags = 0xffff;
    int orFlags  = 0;
    int i;
    vector4 v0;
    vector4 v1;
    
    if (useRect)
    {
        for (i = 0; i < 4; i++)
        {
            int clip = 0;
            v0.set(this->m_vertices[i]);
            v0.w = 1.0f;

            v1 = viewProjection * v0;

            if (v1.x < (rect.v0.x * v1.w))       
                clip |= bbox3::ClipLeft;
            else if (v1.x > (rect.v1.x * v1.w))  
                clip |= bbox3::ClipRight;

            if (v1.y < (rect.v0.y * v1.w))       
                clip |= bbox3::ClipBottom;
            else if (v1.y > (rect.v1.y * v1.w))  
                clip |= bbox3::ClipTop;
            
            if (v1.z < -v1.w)           
                clip |= bbox3::ClipNear;
            else if (v1.z > v1.w)       
                clip |= bbox3::ClipFar;

            andFlags &= clip;
            orFlags  |= clip;
        }

        if (0 == orFlags)       
            return bbox3::Inside;
        else if (0 != andFlags) 
            return bbox3::Outside;
        else                    
            return bbox3::Clipped;
        
    }
    else
    {
        return this->ClipStatus(viewProjection);
    }
}

//------------------------------------------------------------------------------
/**
    Calculates the projected bounding rectangle of the portal, projecting the
    portal's clip rectangle. It clips the resulting portal with the parameter
    one.
*/
rectangle 
ncSpatialPortal::GetCliprect(const matrix44 &viewProjection, 
                             const rectangle &rect) const
{
    vector4 v0;
    vector4 v1;
    rectangle newRect;
    newRect.begin_extend();

    for (int i = 0; i < 4; i++)
    {
        v0.set(this->m_vertices[i]);

        v1 = viewProjection * v0;

        if (v1.w <= 0)
        {
            return rect;
        }

        if (v1.x < (rect.v0.x * v1.w))
        {
            v1.x = rect.v0.x * v1.w;
        }
        else if (v1.x > (rect.v1.x * v1.w))
        {
            v1.x = rect.v1.x * v1.w;
        }

        if (v1.y < (rect.v0.y * v1.w))
        {
            v1.y = rect.v0.y * v1.w;
        }
        else if (v1.y > (rect.v1.y * v1.w))
        {
            v1.y = rect.v1.y * v1.w;
        }

        float x = (v1.x / v1.w);
        float y = (v1.y / v1.w);

        newRect.extend(x, y);
    }

    return newRect;
}


//------------------------------------------------------------------------------
/**
    set the active flag
*/
void
ncSpatialPortal::SetActive(bool flag)
{
    this->m_active = flag;
    
    if ( this->m_cell )
    {
        this->m_cell->RefreshLights();
    }

    if ( this->m_otherSide )
    {
        this->m_otherSide->RefreshLights();
    }
}

//------------------------------------------------------------------------------
/**
    update entity's position 
*/
void 
ncSpatialPortal::Update(const matrix44 &worldMatrix)
{
    // update portal's bounding box
    this->UpdateBBox(worldMatrix);
    // update portal's oriented bounding box
    this->UpdateOBBox(worldMatrix);

    // update portal vertices
    for (int i = 0; i < 4; i++)
    {
        this->m_vertices[i] = worldMatrix * this->m_originalVertices[i];
    }

    // recalculate portal's plane
    this->m_clipCenter.set(0.f, 0.f, 0.f);
    for (int i = 0; i < 4; i++)
    {
        // acummulate clip rectangle's positions to calculate the average
        this->m_clipCenter += this->m_vertices[i];
    }

    // calculate the average point, this is, the clip rectangle's center
    this->m_clipCenter *= 0.25f;

    // transform the portal's plane
    vector3 newNormal(this->m_originalPlane.normal());
    newNormal = worldMatrix.get_quaternion().rotate( newNormal );
    newNormal.norm();
    this->m_plane.set(newNormal.x, newNormal.y, newNormal.z, 1.0f);
    this->m_plane.calculate_d(this->m_clipCenter);

    // if the portal is not in an indoor, resituate it (insert)
    if (!this->m_cell || (this->m_cell->GetType() != ncSpatialCell::N_INDOOR_CELL))
    {
        this->SetDetermineSpaceFlag(ncSpatial::DS_TRUE);
        nSpatialServer::Instance()->GetSpatialVisitor().Visit(this, worldMatrix);
        if (this->GetTwinPortal())
        {
            this->GetTwinPortal()->SetTwinPortal(this);
        }

        this->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialPortal::UpdateBBox(const matrix44 & worldMatrix)
{
    this->m_bbox = this->m_originalBBox;
    this->m_bbox.transform(worldMatrix);

    this->bboxCenter = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialPortal::UpdateOBBox(const matrix44 & worldMatrix)
{
    matrix44 boxMatrix = this->m_originalOBox.GetTransform().getmatrix();
    boxMatrix*=worldMatrix;
    transform44 transform;
    this->m_oBox = this->m_originalOBox;
    transform.setmatrix( boxMatrix);
    this->m_oBox.SetTransform( transform );
}

//------------------------------------------------------------------------------
/**
    determines if a given portal is this one's twin
*/
bool 
ncSpatialPortal::IsTwin(ncSpatialPortal *portal)
{
    n_assert2(portal, "miquelangel.rujula: NULL pointer to portal!");
        
    if ( (!this->m_originalVertices[0].isequal(portal->GetOriginalVertices()[1], 0.00001f)) ||
         (!this->m_originalVertices[1].isequal(portal->GetOriginalVertices()[0], 0.00001f)) ||
         (!this->m_originalVertices[2].isequal(portal->GetOriginalVertices()[3], 0.00001f)) ||
         (!this->m_originalVertices[3].isequal(portal->GetOriginalVertices()[2], 0.00001f)) )
    {
        return false;
    }
    
    n_assert2(portal->GetCell(), 
              "miquelangel.rujula: trying to determine if a portal is the twin one, and it has no cell!");
    if (this->m_otherSideId != portal->GetCell()->GetId())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    associate a brush to the portal
*/
void 
ncSpatialPortal::AddBrush(nEntityObject *brush)
{
    n_assert(brush);
    if (!this->brushes)
    {
        this->brushes = n_new(nArray<nEntityObject*>(2, 2));
    }
    else if (this->brushes->FindIndex(brush) != -1)
    {
        // the entity is already inserted
        return;
    }

    // remove the brush from the cell that is in now
    ncSpatial *spatialComp = brush->GetComponentSafe<ncSpatial>();
    if (spatialComp->GetCell())
    {
        spatialComp->GetCell()->RemoveEntity(brush);
    }

    spatialComp->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);

    // add the brush to this portal
    this->brushes->Append(brush);
}

//------------------------------------------------------------------------------
/**
    remove a brush that was associated to this portal
*/
bool 
ncSpatialPortal::RemoveBrush(nEntityObject *brush)
{
    n_assert(brush);
    if (!this->brushes)
    {
        return false;
    }

    int index = this->brushes->FindIndex(brush);
    if (index == -1)
    {
        return false;
    }

    this->brushes->Erase(index);

    return true;
}

//------------------------------------------------------------------------------
/**
    get the array of brushes associated to this portal
*/
const nArray<nEntityObject*> *
ncSpatialPortal::GetBrushes() const
{
    return this->brushes;
}

//------------------------------------------------------------------------------
/**
    says if this portal touches another one
*/
bool 
ncSpatialPortal::Touches(ncSpatialPortal *portal)
{
    return (this->GetClipCenter() - portal->GetClipCenter()).lensquared() < PORTAL_EPSILON;
}

//------------------------------------------------------------------------------
/**
    connect two indoor portals
*/
bool 
ncSpatialPortal::ConnectTo(ncSpatialPortal *portal)
{
    n_assert(this->Touches(portal));

    ncSpatialPortal* currTwinPortal = this->GetTwinPortal();
    n_assert(currTwinPortal);
    if (!currTwinPortal)
    {
        return false;
    }

    ncSpatialCell* currTwinCell = currTwinPortal->GetCell();
    n_assert(currTwinCell);

    if ( currTwinCell && (currTwinCell->GetType() == ncSpatialCell::N_QUADTREE_CELL) )
    {
        // remove current twin portal from the outdoors and destroy it
        currTwinCell->RemoveEntity(currTwinPortal->GetEntityObject());
        nEntityObjectServer::Instance()->RemoveEntityObject(currTwinPortal->GetEntityObject());
        
        // do the same with the portal we want to connect to
        ncSpatialPortal* otherCurrTwin = portal->GetTwinPortal();
        n_assert(otherCurrTwin);
        if (!otherCurrTwin)
        {
            return false;
        }

        ncSpatialCell* otherCurrTwinCell = otherCurrTwin->GetCell();
        if ( otherCurrTwinCell && (otherCurrTwinCell->GetType() == ncSpatialCell::N_QUADTREE_CELL))
        {
            otherCurrTwinCell->RemoveEntity(otherCurrTwin->GetEntityObject());
            nEntityObjectServer::Instance()->RemoveEntityObject(otherCurrTwin->GetEntityObject());
        }
        else
        {
            return false;
        }

        // connect them
        this->SetTwinPortal(portal);
        portal->SetTwinPortal(this);

        // set the biggest original and normal oriented bounding box (of them) to both portals
        obbox3& thisOriginalOBox = this->GetOriginalOBBox();
        obbox3& twinOriginalOBox = portal->GetOriginalOBBox();

        if ( thisOriginalOBox.GetSize().lensquared() >= twinOriginalOBox.GetSize().lensquared() )
        {
            portal->SetOriginalOBBox(thisOriginalOBox);
            portal->SetOBBox(this->GetOBBox());
        }
        else
        {
            this->SetOriginalOBBox(twinOriginalOBox);
            this->SetOBBox(portal->GetOBBox());
        }

        return true;
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    visibility visitor processing for an entity with a spatial portal component
*/
void
ncSpatialPortal::Accept(nVisibleFrustumVisitor &visitor)
{
    if ( this->m_twinPortal && 
         this->m_twinPortal->IsVisibleBy(visitor.GetCurrentCamId()) )
    {
        return;
    }

    n_assert2(this->m_cell, 
              "miquelangel.rujula: trying to accept a spatial component that has no cell!");
    
    this->m_cell->AppendVisEntity(this->GetEntityObject());
    visitor.Visit(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
    actually, portals, are ignored for spatial queries
*/
void 
ncSpatialPortal::Accept(nSpatialVisitor &/*visitor*/)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialPortal::SaveCmds(nPersistServer * ps)
{
    if ( ncSpatial::SaveCmds(ps) )
    {
        //--- setoriginalbbox ---
        vector3 v0, v1;
        this->GetOriginalBBox(v0, v1);
        if (!ps->Put(this->GetEntityObject(), 'RSOB', v0.x, v0.y, v0.z, v1.x, v1.y, v1.z))
        {
            return false;
        }

        //--- setorigobboxsize ---
        const vector3 &size = this->m_originalOBox.GetSize();
        if (!ps->Put(this->GetEntityObject(), 'RSOZ', size.x, size.y, size.z))
        {
            return false;
        }

        //--- setorigobboxscale---
        const vector3 &scale = this->m_originalOBox.GetScale();
        if (!ps->Put(this->GetEntityObject(), 'RSOA', scale.x, scale.y, scale.z))
        {
            return false;
        }
        
        //--- setorigobboxeuler---
        const vector3 &euler = this->m_originalOBox.GetEuler();
        if (!ps->Put(this->GetEntityObject(), 'RSOE', euler.x, euler.y, euler.z))
        {
            return false;
        }

        //--- setorigobboxtranlation---
        const vector3 &translation = this->m_originalOBox.GetTranslation();
        if (!ps->Put(this->GetEntityObject(), 'RSOT', translation.x, translation.y, translation.z))
        {
            return false;
        }

        //--- setothersidecellid ---
        int otherSideCellId = 0;
        if (this->m_otherSide)
        {
            otherSideCellId = this->m_otherSide->GetId();
        }

        if (!ps->Put(this->GetEntityObject(), 'RSOI', otherSideCellId))
        {
            return false;
        }

        //--- setclipvertex ---
        for (int i = 0; i < 4; i++)
        {
            if (!ps->Put(this->GetEntityObject(), 'RSCV', i, this->m_originalVertices[i].x, 
                                                             this->m_originalVertices[i].y, 
                                                             this->m_originalVertices[i].z))
            {
                return false;
            }
        }

        //--- setoriginalplane ---
        if (!ps->Put(this->GetEntityObject(), 'RSPP', this->m_originalPlane.a, 
                                                      this->m_originalPlane.b, 
                                                      this->m_originalPlane.c, 
                                                      this->m_originalPlane.d))
        {
            return false;
        }

        //--- setactive ---
        if (!ps->Put(this->GetEntityObject(), 'RSAF', this->m_active))
        {
            return false;
        }

        //--- setdeactivationdistance ---
        if (!ps->Put(this->GetEntityObject(), 'RSDD', this->GetDeactivationDistance()))
        {
            return false;
        }
    }

    return true;
}

