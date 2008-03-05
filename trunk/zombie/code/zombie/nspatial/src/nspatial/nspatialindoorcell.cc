//------------------------------------------------------------------------------
#include "precompiled/pchnspatial.h"
#include "nspatial/nspatialtypes.h"
#include "nspatial/nspatialindoorcell.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialportal.h"
#include "entity/nentityobjectserver.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif //NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(nSpatialIndoorCell, ncSpatialCell);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nSpatialIndoorCell)
    NSCRIPT_ADDCMD_COMPOBJECT('RSOB', void, SetOriginalBBox, 6, (float, float, float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGOB', void, GetOriginalBBox, 0, (), 2, (vector3&, vector3&));
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('R_RT', void, RemoveTemporary, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RRFT', void, RestoreTempArray, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
nSpatialIndoorCell::nSpatialIndoorCell():
ncSpatialCell()
{
    this->m_cellType = ncSpatialCell::N_INDOOR_CELL;
    this->m_originalBBox.begin_extend();
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialIndoorCell::~nSpatialIndoorCell()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Determine if a given bounding box belongs to this cell using the wrapper 
    mesh. All the points of the bounding box have to be into the wrapper mesh
    to be contained.
*/
bool 
nSpatialIndoorCell::Contains(const bbox3 &box) const
{
    vector3 vert;

    for (int i = 0; i < 8; i++)
    {
        if (i & 1) 
        {
            vert.x = box.vmin.x;
        }
        else       
        {
            vert.x = box.vmax.x;
        }
        if (i & 2) 
        {
            vert.y = box.vmin.y;
        }
        else       
        {
            vert.y = box.vmax.y;
        }
        if (i & 4) 
        {
            vert.z = box.vmin.z;
        }
        else 
        {
            vert.z = box.vmax.z; 
        }

        if (!this->m_wrapperMesh.Contains(vert))
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell and doesn't destroy it, unless it's an indoor
    brush or a portal
*/
bool 
nSpatialIndoorCell::RemoveEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");

    if (!ncSpatialCell::RemoveEntity(entity))
    {
        return false;
    }

    if (entity->IsA("neindoorbrush") ||
        entity->GetComponent<ncSpatialPortal>())
    {
        nEntityObjectServer::Instance()->RemoveEntityObject(entity);
    }

    return true;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    set the layer id to all the indoor entities (indoor brushes and portals) 
    into this cell
*/
void 
nSpatialIndoorCell::SetLayerId(int layerId)
{
    this->ncSpatialCell::SetLayerId(layerId);
    
    ncEditor *editorComp = 0;
    nArray<nEntityObject*> &indoorBrushes = this->m_categories[nSpatialTypes::CAT_INDOOR_BRUSHES];
    for (int i = 0; i < indoorBrushes.Size(); i++)
    {
        editorComp = indoorBrushes[i]->GetComponentSafe<ncEditor>();
        editorComp->SetLayerId(layerId);
    }
}
#endif

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    remove temporary
*/
void 
nSpatialIndoorCell::RemoveTemporary()
{
    nArray<nEntityObject*> &portalsArray = this->m_categories[nSpatialTypes::CAT_PORTALS];
    ncSpatialPortal *portal = 0;
    ncSpatialPortal *twinPortal = 0;
    for (int i = 0; i < portalsArray.Size(); ++i)
    {
        n_assert2(portalsArray[i]->GetComponent<ncSpatialPortal>(), 
                  "miquelangel.rujula: portal entity expected in the PORTALS category!");
        this->RemoveTempEntity(portalsArray[i]);
        portal = portalsArray[i]->GetComponentSafe<ncSpatialPortal>();
        twinPortal = portal->GetTwinPortal();
        if (twinPortal)
        {
            twinPortal->RemoveTemporary();
        }
    }

    nArray<nEntityObject*> &indoorBrushes = this->m_categories[nSpatialTypes::CAT_INDOOR_BRUSHES];
    for (int i = 0; i < indoorBrushes.Size(); ++i)
    {
        this->RemoveTempEntity(indoorBrushes[i]);
    }
}

//------------------------------------------------------------------------------
/**
    restore this cell's entities from the temporal array
*/
void 
nSpatialIndoorCell::RestoreTempArray()
{
    nArray<nEntityObject*> &portalsArray = this->m_categories[nSpatialTypes::CAT_PORTALS];
    ncSpatialPortal *portal = 0;
    ncSpatialPortal *twinPortal = 0;
    for (int i = 0; i < portalsArray.Size(); ++i)
    {
        n_assert2(portalsArray[i]->GetComponent<ncSpatialPortal>(), 
                  "miquelangel.rujula: portal entity expected in the PORTALS category!");
        this->RestoreTempEntity(portalsArray[i]);
        portal = portalsArray[i]->GetComponentSafe<ncSpatialPortal>();
        twinPortal = portal->GetTwinPortal();
        if (twinPortal)
        {
            twinPortal->RestoreFromTempArray();
        }
    }

    nArray<nEntityObject*> &indoorBrushes = this->m_categories[nSpatialTypes::CAT_INDOOR_BRUSHES];
    for (int i = 0; i < indoorBrushes.Size(); ++i)
    {
        this->RestoreTempEntity(indoorBrushes[i]);
    }
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    get the all the indoor brushes into this indoor cell
*/
void 
nSpatialIndoorCell::GetIndoorBrushes(nArray<nEntityObject*> &indoorBrushes)
{
    nArray<nEntityObject*> &cellIndoorBrushes = this->m_categories[nSpatialTypes::CAT_INDOOR_BRUSHES];
    for (int i = 0; i < cellIndoorBrushes.Size(); i++)
    {
        indoorBrushes.Append( cellIndoorBrushes[i] );
    }
}

//------------------------------------------------------------------------------
/**
    get the all the portals into this indoor cell
*/
void 
nSpatialIndoorCell::GetPortals(nArray<ncSpatialPortal*> &portals)
{
    nArray<nEntityObject*> &cellPortals = this->m_categories[nSpatialTypes::CAT_PORTALS];
    for (int i = 0; i < cellPortals.Size(); i++)
    {
        portals.Append( cellPortals[i]->GetComponentSafe<ncSpatialPortal>() );
    }
}

//------------------------------------------------------------------------------
/**
    Get the entities that are not indoor brushes nor portals. It fills the given
    array with the entities. It doesn't reset the array.
*/
void 
nSpatialIndoorCell::GetDynamicEntities(nArray<nEntityObject*> &entitiesArray)
{
    for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
    {
        // ignore portals and indoor brushes
        if ( (catIndex == nSpatialTypes::CAT_PORTALS) ||
             (catIndex == nSpatialTypes::CAT_INDOOR_BRUSHES) )
        {
            continue;
        }

        // append all the other entities
        const nArray<nEntityObject*> &category = this->m_categories[catIndex];
        for (int i = 0; i < category.Size(); i++)
        {
            entitiesArray.Append(category[i]);
        }

    }
}

//------------------------------------------------------------------------------
/**
    visibility visitor processing for a basic spatial cell.
*/
void 
nSpatialIndoorCell::Accept(nVisibleFrustumVisitor & /*visitor*/)
{
    this->SetVisibleFrameId(nSpatialServer::Instance()->GetFrameId());
}

//------------------------------------------------------------------------------
/**
    update the transformation of all the entities into the cell
*/
void 
nSpatialIndoorCell::Update(vector3 &incPos, quaternion &incQuat, const matrix44 &matrix)
{
    ncSpatialCell::Update(incPos, incQuat, matrix);

    // transform the bounding box
    this->m_bbox = this->m_originalBBox;
    this->m_bbox.transform(matrix);

    // transform the wrapper mesh
    this->m_wrapperMesh.SetTransform(matrix);
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialIndoorCell::ReadStaticInfo(const TiXmlElement *cellElem)
{
    if (ncSpatialCell::ReadStaticInfo(cellElem))
    {
        // read the original bbox information
        TiXmlElement *origBBoxElem = cellElem->FirstChildElement("OriginalBBox");
        n_assert2(origBBoxElem, 
                  "miquelangel.rujula: can't read TiXmlElement for the indoor cell's original bounding box!");
        bbox3 box;
        box.vmin.x = (float)atof(origBBoxElem->Attribute("VminX"));
        box.vmin.y = (float)atof(origBBoxElem->Attribute("VminY"));
        box.vmin.z = (float)atof(origBBoxElem->Attribute("VminZ"));
        box.vmax.x = (float)atof(origBBoxElem->Attribute("VmaxX"));
        box.vmax.y = (float)atof(origBBoxElem->Attribute("VmaxY"));
        box.vmax.z = (float)atof(origBBoxElem->Attribute("VmaxZ"));
        this->SetOriginalBBox(box);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialIndoorCell::WriteStaticInfo(TiXmlElement *cellElem)
{
    if (ncSpatialCell::WriteStaticInfo(cellElem))
    {
        char longCharBuff[50];

        // save the original bounding box
        TiXmlElement originalBBoxElem("OriginalBBox");
        sprintf(longCharBuff, "%f", this->m_bbox.vmin.x);
        originalBBoxElem.SetAttribute("VminX", longCharBuff);
        sprintf(longCharBuff, "%f", this->m_bbox.vmin.y);
        originalBBoxElem.SetAttribute("VminY", longCharBuff);
        sprintf(longCharBuff, "%f", this->m_bbox.vmin.z);
        originalBBoxElem.SetAttribute("VminZ", longCharBuff);
        sprintf(longCharBuff, "%f", this->m_bbox.vmax.x);
        originalBBoxElem.SetAttribute("VmaxX", longCharBuff);
        sprintf(longCharBuff, "%f", this->m_bbox.vmax.y);
        originalBBoxElem.SetAttribute("VmaxY", longCharBuff);
        sprintf(longCharBuff, "%f", this->m_bbox.vmax.z);
        originalBBoxElem.SetAttribute("VmaxZ", longCharBuff);

        cellElem->InsertEndChild(originalBBoxElem);

        // save the wrapper mesh
        nString filename(this->m_parentSpace->GetSpatialPath());
        filename.Append("wrapperMesh");
        filename.AppendInt(this->m_cellId);
        filename.Append(".n3d2");

        bool result = this->m_wrapperMesh.GetMeshBuilder().SaveN3d2(nKernelServer::Instance()->GetFileServer(),
                                                                    filename.Get());

        return result;
    }

    return false;
}
