#include "precompiled/pchnspatial.h"
//-----------------------------------------------------------------------------
//  nspatialportal.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "nspatial/nspatialportal.h"
#include "nscene/nscenenode.h"

//------------------------------------------------------------------------------
/**
    constructor 1
*/
nSpatialPortal::nSpatialPortal() : 
nSpatialElement(),
m_otherSide(NULL),
m_twinPortal(NULL),
m_otherSideId(-1),
m_frameId(-1),
m_lightFrameId(-1)
{
    // set type
    this->m_elementType = nSpatialElement::N_SPATIAL_PORTAL;

    // initialize vertices array
    this->m_vertices = n_new_array(vector3, 4);
}

//------------------------------------------------------------------------------
/**
    constructor 2
*/
nSpatialPortal::nSpatialPortal(nSpatialCell *parentCell, nSpatialCell *connectToCell) : 
nSpatialElement(),
m_otherSide(connectToCell),
m_twinPortal(NULL),
m_frameId(-1)
{
    // set type
    this->m_cell = parentCell;

    if (connectToCell)
    {
        this->m_otherSideId = connectToCell->GetId();
    }
    else
    {
        this->m_otherSideId = 0;
    }
    
    this->m_elementType = nSpatialElement::N_SPATIAL_PORTAL;

    // initialize vertices array
    this->m_vertices = n_new_array(vector3, 4);
}

//------------------------------------------------------------------------------
/**
    Check for the intersection of the clip rectangle with a view volume 
    defined by a view-projection matrix.
*/
bbox3::ClipStatus
nSpatialPortal::ClipStatus(const matrix44 &viewProjection) const
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
nSpatialPortal::ClipStatus(const matrix44& viewProjection, const rectangle& rect, bool useRect) const
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

            // special case: a point is behind the camera
            //if (v1.w < 0)
            //{
            //    if (v1.x < (rect.v0.x * v1.w))       
            //        clip |= bbox3::ClipRight;
            //    else if (v1.x > (rect.v1.x * v1.w))  
            //        clip |= bbox3::ClipLeft;

            //    if (v1.y < (rect.v0.y * v1.w))       
            //        clip |= bbox3::ClipTop;
            //    else if (v1.y > (rect.v1.y * v1.w))  
            //        clip |= bbox3::ClipBottom;
            //}
            //else
            //{
            if (v1.x < (rect.v0.x * v1.w))       
                clip |= bbox3::ClipLeft;
            else if (v1.x > (rect.v1.x * v1.w))  
                clip |= bbox3::ClipRight;

            if (v1.y < (rect.v0.y * v1.w))       
                clip |= bbox3::ClipBottom;
            else if (v1.y > (rect.v1.y * v1.w))  
                clip |= bbox3::ClipTop;
            //}
            
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
nSpatialPortal::GetCliprect(const matrix44 &viewProjection, 
                            rectangle &rect) const
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

        //if ((v1.x / v1.w) < rect.v0.x
        if (v1.x < (rect.v0.x * v1.w))
        {
            v1.x = rect.v0.x * v1.w;
        }
        //else if ((v1.x / v1.w) > rect.v1.x)
        else if (v1.x > (rect.v1.x * v1.w))
        {
            v1.x = rect.v1.x * v1.w;
        }
        //if ((v1.y / v1.w) < rect.v0.y)
        if (v1.y < (rect.v0.y * v1.w))
        {
            v1.y = rect.v0.y * v1.w;
        }
        //else if ((v1.y / v1.w) > rect.v1.y)
        else if (v1.y > (rect.v1.y * v1.w))
        {
            v1.y = rect.v1.y * v1.w;
        }

//        n_assert2(((v1.w > 0.00001) && (v1.w > 0)), "miquelangel.rujula");

        float x = (v1.x / v1.w);
        float y = (v1.y / v1.w);

        newRect.extend(x, y);
    }

    return newRect;
}

//------------------------------------------------------------------------------
/**
    Determine the given subtree's bounding box, by adding the bounding box of
    all the shape nodes on it. When it founds a shape node it calculates its 
    bounding box, and the bounding box of the sibling shape nodes. Then it
    extends the box with the clip rectangle's bounding box.
    Extends the given parameter box with the calculated one.
*/
//void 
//nSpatialPortal::CalculateBBox(nSceneNode *node, bbox3 &box)
//{
//    n_assert2(node, "miquelangel.rujula");
//
//    //// calculate the node's bounding box
//    nSpatialElement::CalculateBBox(node, box);
//
//    // extend the box with the clip rectangle's bounding box
//    for (int i = 0; i < 4; i++)
//    {
//        box.extend(this->m_vertices[i]);
//    }
//}

//------------------------------------------------------------------------------
/**
    Recalculate the bounding box from the actual render context's root node and
    the clip rectangle.
*/
//void 
//nSpatialPortal::RecalculateBBox()
//{
//    this->m_bbox.begin_extend();
//
//    // extend the box with the clip rectangle's bounding box
//    for (int i = 0; i < 4; i++)
//    {
//        this->m_bbox.extend(this->m_vertices[i]);
//    }
//
//    // check if there is a root node
//    bbox3 newBox;
//    if (this->m_renderContext->IsValid())
//    {
//        nSceneNode *rootNode = this->m_renderContext->GetRootNode();
//        newBox = rootNode->GetLocalBox();
//        vector3 nullVec;
//        if (newBox.vmin.isequal(nullVec, 0.000001f) && 
//            newBox.vmax.isequal(nullVec, 0.000001f))
//        {
//            // the local box wasn't calculated. Let's do it.
//            //this->CalculateBBox(rootNode, newBox);
//            newBox.begin_extend();
//            nSpatialElement::CalculateBBox(rootNode, newBox);
//        }
//    }
//    
//    // extend with the bounding box calculated from the root node
//    this->m_bbox.extend(newBox);
//}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialPortal::ReadXmlElement(const TiXmlElement *spatialPortalElem)
{
    n_assert2(spatialPortalElem, "miquelangel.rujula");

    // read the spatial element's information of the portal
    nSpatialElement::ReadXmlElement(spatialPortalElem);

    // read the portal's own information (other side cell)
    TiXmlElement *otherSideCellElem = spatialPortalElem->FirstChildElement("OtherSideCell");
    if (!otherSideCellElem)
    {
        return false;
    }
    this->m_otherSideId = atoi(otherSideCellElem->Attribute("Value"));

    // read the clip rectangle's information
    TiXmlElement *clipRectElem = spatialPortalElem->FirstChildElement("ClipRectangle");
    n_assert2(clipRectElem, "miquelangel.rujula");

    TiXmlElement *vertexElem;
    for (int i = 0; i < 4; i++)
    {
        nString vertexName("V");
        vertexName.AppendInt(i);
        vertexElem = clipRectElem->FirstChildElement(vertexName.Get());
        this->m_vertices[i].x = static_cast<float>(atof(vertexElem->Attribute("x")));
        this->m_vertices[i].y = static_cast<float>(atof(vertexElem->Attribute("y")));
        this->m_vertices[i].z = static_cast<float>(atof(vertexElem->Attribute("z")));
    }

    // read the portal plane's information
    TiXmlElement *portalPlaneElem = spatialPortalElem->FirstChildElement("Plane");
    n_assert2(portalPlaneElem, "miquelangel.rujula");

    float a = static_cast<float>(atof(portalPlaneElem->Attribute("A")));
    float b = static_cast<float>(atof(portalPlaneElem->Attribute("B")));
    float c = static_cast<float>(atof(portalPlaneElem->Attribute("C")));
    float d = static_cast<float>(atof(portalPlaneElem->Attribute("D")));

    plane portalPlane(a, b, c, d);
    this->SetPlane(portalPlane);




/////// REMOVE THIS LINE!!!!!!
//    this->RecalculateBBox();
/////// REMOVE THIS LINE!!!!!!




    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialPortal::FillXmlElement(TiXmlElement *spatialPortalElem)
{
    n_assert2(spatialPortalElem, "miquelangel.rujula");

    // fill the xml element with the spatial element's information of the portal
    if (!nSpatialElement::FillXmlElement(spatialPortalElem))
    {
        return false;
    }

    // add the portal's own information (other side cell)
    TiXmlElement otherSideCellElem("OtherSideCell");
    nSpatialCell *otherSideCell = this->m_otherSide;

    if (!otherSideCell)
    {   
        // communicates with an external space
        otherSideCellElem.SetAttribute("Value", 0);
    }
    else
    {
        n_assert2(this->m_otherSide->GetId() >= 0, "miquelangel.rujula");

        // communicates with another cell in the current space
        otherSideCellElem.SetAttribute("Value", otherSideCell->GetId());
    }

    spatialPortalElem->InsertEndChild(otherSideCellElem);

    // add the clip rectangle's information
    TiXmlElement clipRectElem("ClipRectangle");
    for (int i = 0; i < 4; i++)
    {
        nString vertexName("V");
        vertexName.AppendInt(i);
        TiXmlElement vertexElem(vertexName.Get());
        nString value(this->m_vertices[i].x);
        vertexElem.SetAttribute("x", value.Get());
        value.SetFloat(this->m_vertices[i].y);
        vertexElem.SetAttribute("y", value.Get());
        value.SetFloat(this->m_vertices[i].z);
        vertexElem.SetAttribute("z", value.Get());
        clipRectElem.InsertEndChild(vertexElem);
    }
    
    spatialPortalElem->InsertEndChild(clipRectElem);

    // add the portal plane's information
    TiXmlElement portalPlaneElem("Plane");
    nString tempString(this->m_plane.a);
    portalPlaneElem.SetAttribute("A", tempString.Get());
    tempString.SetFloat(this->m_plane.b);
    portalPlaneElem.SetAttribute("B", tempString.Get());
    tempString.SetFloat(this->m_plane.c);
    portalPlaneElem.SetAttribute("C", tempString.Get());
    tempString.SetFloat(this->m_plane.d);
    portalPlaneElem.SetAttribute("D", tempString.Get());

    spatialPortalElem->InsertEndChild(portalPlaneElem);

    return true;
}

//------------------------------------------------------------------------------
/**
   visibility visitor processing for a basic portal.  We call accept on the underlying cell, 
   unless we've hit the bottom of the allow recursion, in which case the portal is accepted instead
*/

void 
nSpatialPortal::Accept(nVisibilityVisitor &visitor, int recursiondepth)
{
    //if (this->m_renderContext->IsValid())
    //{
    //    this->m_cell->AppendVisElement(this);
    //}

    visitor.Visit(this, recursiondepth);
}

//------------------------------------------------------------------------------
/**
    actually, portals, are ignored for spatial queries
*/
void 
nSpatialPortal::Accept(nSpatialVisitor &/*visitor*/, int /*recursiondepth*/)
{
}
