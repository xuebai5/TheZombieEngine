#include "precompiled/pchnspatial.h"
//-----------------------------------------------------------------------------
//  ncspatialcell.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "nspatial/nspatialcell.h"
#include "nspatial/nspatialserver.h"
#include "util/npolygoncontainer.h"
//#include "nspatial/nspatialportal.h"
//#include "nspatial/nspatiallight.h"
//#include "nspatial/nspatialelements.h"
//#include "nscene/nscenenode.h"
#include "entity/nentity.h"
#include "zombieentity/neLight.h"
#include "zombieentity/nePortal.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialspace.h"
#include "tinyxml/tinyxml.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nSpatialCell::nSpatialCell() : 
m_cellId(-1),
m_cellType(N_SPATIAL_CELL),
m_parentSpace(NULL),
m_frameId(-1)
{
    this->m_polygonContainer = n_new(nPolygonContainer);
    //this->m_categories = n_new(nKeyArray<nArray<nSpatialElement*>*>(4, 4));
    this->m_categories = n_new(nKeyArray<nArray<nEntityObject*>*>(4, 4));
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialCell::~nSpatialCell()
{
    //n_assert2(this->m_elementsArray.Size() == 0, "miquelangel.rujula");
    n_assert2(this->m_entitiesArray.Size() == 0, "miquelangel.rujula");
    n_assert2(this->m_portalsArray.Size()  == 0, "miquelangel.rujula");
    n_assert2(this->m_lightsArray.Size()   == 0, "miquelangel.rujula");
    
    // free the polygons container
    n_delete(this->m_polygonContainer);

    // destroy all the categories
    for (int i = 0; i < this->m_categories->Size(); i++)
    {
        n_delete(this->m_categories->GetElementAt(i));
    }

    // free the categories array
    n_delete(this->m_categories);
}

////------------------------------------------------------------------------------
///**
//    destroy the cell and all its elements
//*/
//void
//nSpatialCell::DestroyCell()
//{
//    // delete all the spatial elements
//    this->DestroyAllSpatialElements();
//
//    // delete all the spatial portals
//    this->DestroyAllSpatialPortals();
//
//    // delete all the spatial lights
//    this->DestroyAllSpatialLights();
//
//    // delete all the spatial elements in the categories
//    this->DestroyAllCatSpatialElements();
//}

//------------------------------------------------------------------------------
/**
    destroy the cell and all its entities
*/
void
nSpatialCell::DestroyCell()
{
    // delete all the entities
    this->DestroyAllEntities();

    // delete all the portals
    this->DestroyAllPortals();

    // delete all the lights
    this->DestroyAllLights();

    // delete all the entities in the categories
    this->DestroyAllCatEntities();
}

//------------------------------------------------------------------------------
/**
    destroy all the entities in this cell (not portals neither lights)
*/
void
//nSpatialCell::DestroyAllSpatialElements()
nSpatialCell::DestroyAllEntities()
{
    //for (nArray<nSpatialElement*>::iterator element  = this->m_elementsArray.Begin();
    //                                        element != this->m_elementsArray.End();
    //                                        element++)
    //{
    //    n_delete((*element));
    //}
    for (nArray<nEntityObject*>::iterator pEntity  = this->m_entitiesArray.Begin();
                                          pEntity != this->m_entitiesArray.End();
                                          pEntity++)
    {
        (*pEntity)->Release();
    }

    //this->m_elementsArray.Reset();
    this->m_entitiesArray.Reset();
}

//------------------------------------------------------------------------------
/**
    destroy all the portals in this cell
*/
void
nSpatialCell::DestroyAllPortals()
{
    //for (nArray<nSpatialPortal*>::iterator portal  = this->m_portalsArray.Begin();
    //                                       portal != this->m_portalsArray.End();
    //                                       portal++)
    //{
    //    n_delete((*portal));
    //}
    
    for (nArray<nePortal*>::iterator pPortal  = this->m_portalsArray.Begin();
                                     pPortal != this->m_portalsArray.End();
                                     pPortal++)
    {
        (*pPortal)->Release();
    }

    this->m_portalsArray.Reset();
}

//------------------------------------------------------------------------------
/**
    destroy all the lights in this cell
*/
void
nSpatialCell::DestroyAllLights()
{
    //for (nArray<nSpatialLight*>::iterator light  = this->m_lightsArray.Begin();
    //                                      light != this->m_lightsArray.End();
    //                                      light++)
    //{
    //    n_delete((*light));
    //}
    
    for (nArray<neLight*>::iterator pLight  = this->m_lightsArray.Begin();
                                    pLight != this->m_lightsArray.End();
                                    pLight++)
    {
        (*pLight)->Release();
    }

    this->m_lightsArray.Reset();
}

//------------------------------------------------------------------------------
/**
    destroy all the entities in this cell's categories
*/
void
nSpatialCell::DestroyAllCatEntities()
{
   /// @TODO.
    /// The problem is that a spatial element can be in more than one cell
    /// and if we delete it in one cell, the application will crash when 
    /// trying to delete it from another one.
    /// To solve it we have to make the spatial element inherits from nReferenced,
    /// and use the 'Release' method instead of 'n_delete'.
    /// When we have the entities system, the spatial module will manage entities,
    /// instead of nSpatialElements. Of course, all these entities will have a
    /// spatial component, where the spatial module will catch all the necessary
    /// spatial information. The spatial component will be more or less like 
    /// the nSpatialElement.
}

//------------------------------------------------------------------------------
/**
    set the space the cell belongs to
*/
void 
//nSpatialCell::SetParentSpace(nSpatialSpace *space)
nSpatialCell::SetParentSpace(ncSpatialSpace *space)
{
    this->m_parentSpace = space;
}

//------------------------------------------------------------------------------
/**
    get the space the cell belongs to
*/
//nSpatialSpace *
ncSpatialSpace *
nSpatialCell::GetParentSpace()
{
    return this->m_parentSpace;
}

//------------------------------------------------------------------------------
/**
    adds an element to the cell
*/
//void 
//nSpatialCell::AddSpatialElement(nSpatialElement *element)
//{
//    n_assert2(element, "miquelangel.rujula");
//
//    nSpatialLight *spatialLight;
//    switch(element->GetType())
//    {
//        case nSpatialElement::N_SPATIAL_PORTAL:
//            this->m_portalsArray.Append((nSpatialPortal*)element);
//            break;
//
//        case nSpatialElement::N_SPATIAL_LIGHT:
//            spatialLight = (nSpatialLight*)element;
//            this->m_lightsArray.Append(spatialLight);
//            if (!spatialLight->AffectsOnlyACell())
//            {
//                // its a space range light
//                this->m_parentSpace->GetSpaceRangeLightsArray()->Append(spatialLight);
//            }
//            break;
//
//        case nSpatialElement::N_SPATIAL_CAMERA:
//            // do nothing. Just don't insert it in any cell's array
//            break;
//        
//        default:
//            this->m_elementsArray.Append(element);
//            break;
//
//    }
//
//    element->SetCell(this);
//}

//------------------------------------------------------------------------------
/**
    adds an entity to the cell
*/
void 
nSpatialCell::AddEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");
    
    //ncSpatial *spatialComp = entity->GetComponent(nType<ncSpatial>());
    //n_assert2(spatialCom, "Error: entity hasn't spatial component!");

    //if ((spatialComp = entity->GetComponent(nType<ncSpatial>())) != 0)
    //{
    //    this->m_entitiesArray.Append(entity);
    //    spatialComp->SetCell(this);
    //}
    //else if((portalComp = entity->GetComponent(nType<ncSpatialPortal>())) != 0)
    //{
    //    this->m_portalsArray.Append(entity);
    //    portalComp->SetCell(this);
    //}
    //else if((lightComp = entity->GetComponent(nType<ncSpatialLight>())) != 0)
    //{
    //    this->m_lightsArray.Append(entity);
    //    if (!lightComp->AffectsOnlyACell())
    //    {
    //        // it's a space range light
    //        this->m_parentSpace->GetSpaceRangeLightsArray()->Append(entity);
    //    }
    //    lightComp->SetCell(this);
    //}

    ncSpatial *spatialComp = entity->GetComponent(nType<ncSpatial>());
    n_assert2(spatialComp, "Error: entity hasn't spatial component!");

    //ncSpatialLight *lightComp = 0;
    neLight *lightEntity = 0;
    switch(spatialComp->GetType())
    {
        case ncSpatial::N_SPATIAL_PORTAL:
            this->m_portalsArray.Append(static_cast<nePortal*>(entity));
            break;

        case ncSpatial::N_SPATIAL_LIGHT:
            lightEntity = static_cast<neLight*>(entity);
            this->m_lightsArray.Append(lightEntity);
            //lightComp = static_cast<ncSpatialLight*>(spatialComp);
            //if (!lightComp->AffectsOnlyACell())
            if (!lightEntity->AffectsOnlyACell())
            {
                // its a space range light
                this->m_parentSpace->GetSpaceRangeLightsArray()->Append(lightEntity);
            }
            break;

        case ncSpatial::N_SPATIAL_CAMERA:
            // do nothing. Just don't insert it in any cell's array
            break;
        
        default:
            this->m_entitiesArray.Append(entity);
            break;

    }

    spatialComp->SetCell(this);
}

//------------------------------------------------------------------------------
/**
    adds an element to the cell in a category
*/
//bool 
//nSpatialCell::AddSpatialElement(nSpatialElement *element, int category)
//{
//    nArray<nSpatialElement*> *catArray = 0;
//
//    // find the category
//    if(!this->m_categories->Find(category, catArray))
//    {
//        return false;
//    }
//
//    n_assert2(catArray, "Error: can't insert spatial element. Wrong category id.");
//
//    // append the element to the category
//    catArray->Append(element);
//
//    
//    //// remove the element from the old cell
//    //nSpatialCell *oldCell = element->GetCell();
//    //if (oldCell)
//    //{
//    //    //nSpatialServer::Instance()->MoveSpatialElement(element, this);
//    //    if (!oldCell->RemoveSpatialElement(element, category))
//    //    {
//    //        return false;
//    //    }
//    //}
//    //
//    //// set the cell of this element
//    //element->SetCell(this);
//
//    return true;
//}
//------------------------------------------------------------------------------
/**
    adds an entity to the cell in a category
*/
bool 
nSpatialCell::AddEntity(nEntityObject *entity, int category)
{
    nArray<nEntityObject*> *catArray = 0;

    // find the category
    if(!this->m_categories->Find(category, catArray))
    {
        return false;
    }

    n_assert2(catArray, "Error: can't insert spatial element. Wrong category id.");

    // append the entity to the category
    catArray->Append(entity);

    
    //// remove the element from the old cell
    //nSpatialCell *oldCell = element->GetCell();
    //if (oldCell)
    //{
    //    //nSpatialServer::Instance()->MoveSpatialElement(element, this);
    //    if (!oldCell->RemoveSpatialElement(element, category))
    //    {
    //        return false;
    //    }
    //}
    //
    //// set the cell of this element
    //element->SetCell(this);

    return true;
}

//------------------------------------------------------------------------------
/**
    adds a portal to the cell
*/
void 
//nSpatialCell::AddSpatialPortal(nSpatialPortal *portal)
nSpatialCell::AddPortal(nePortal *portal)
{
    this->m_portalsArray.Append(portal);
    //portal->SetCell(this);
    portal->SetCell(this);
}

//------------------------------------------------------------------------------
/**
    adds a light to the cell
*/
void 
//nSpatialCell::AddSpatialLight(nSpatialLight *light)
nSpatialCell::AddLight(neLight *light)
{
    this->m_lightsArray.Append(light);
    //light->SetCell(this);
    light->SetCell(this);
}

//------------------------------------------------------------------------------
/**
    removes an element from the cell and doesn't destroy it
*/
//bool 
//nSpatialCell::RemoveSpatialElement(nSpatialElement *element)
//{
//    n_assert2(element, "miquelangel.rujula");
//
//    int index = -1;
//    switch(element->GetType())
//    {
//        case nSpatialElement::N_SPATIAL_PORTAL:
//            index = this->m_portalsArray.FindIndex((nSpatialPortal*)element);
//            this->m_portalsArray.EraseQuick(index);
//            break;
//
//        case nSpatialElement::N_SPATIAL_LIGHT:
//            index = this->m_lightsArray.FindIndex((nSpatialLight*)element);
//            this->m_lightsArray.EraseQuick(index);
//            break;
//
//        case nSpatialElement::N_SPATIAL_CAMERA:
//            // do nothing, because cameras are not inserted in any cell's array
//            break;
//
//        default:
//            index = this->m_elementsArray.FindIndex(element);
//            this->m_elementsArray.EraseQuick(index);
//            break;
//
//    }
//    
//    // element shouldn't be destroyed
//    n_assert2(element, "miquelangel.rujula");
//    element->SetCell(NULL);
//    return true;
//}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell and doesn't destroy it
*/
bool 
nSpatialCell::RemoveEntity(nEntityObject *entity)
{
    //ncSpatial *spatialComp = 0;
    //ncSpatialPortal *portalComp = 0;
    //ncSpatialLight *lightComp = 0;
    //int index = -1;

    //if ((spatialComp = entity->GetComponent(nType<ncSpatial>())) != 0)
    //{
    //    index = this->m_entitiesArray.FindIndex(entity);
    //    if (index == -1)
    //    {
    //        return false;
    //    }
    //    this->m_entitiesArray.EraseQuick(index);
    //    spatialComp->SetCell(0);
    //}
    //else if((portalComp = entity->GetComponent(nType<ncSpatialPortal>())) != 0)
    //{
    //    index = this->m_portalsArray.FindIndex(entity);
    //    if (index == -1)
    //    {
    //        return false;
    //    }
    //    this->m_portalsArray.EraseQuick(index);
    //    portalComp->SetCell(0);
    //}
    //else if((lightComp = entity->GetComponent(nType<ncSpatialLight>())) != 0)
    //{
    //    index = this->m_lightsArray.FindIndex(entity);
    //    if (index == -1)
    //    {
    //        return false;
    //    }
    //    this->m_lightsArray.EraseQuick(index);
    //    lightComp->SetCell(0);
    //}
    
    n_assert2(entity, "miquelangel.rujula");

    ncSpatial *spatialComp = entity->GetComponent(nType<ncSpatial>());
    n_assert2(spatialComp, "Error: entity hasn't spatial component!");

    int index = -1;
    switch(spatialComp->GetType())
    {
        case ncSpatial::N_SPATIAL_PORTAL:
            index = this->m_portalsArray.FindIndex(static_cast<nePortal*>(entity));
            if (index == -1)
            {
                return false;
            }
            this->m_portalsArray.EraseQuick(index);
            break;

        case ncSpatial::N_SPATIAL_LIGHT:
            index = this->m_lightsArray.FindIndex(static_cast<neLight*>(entity));
            if (index == -1)
            {
                return false;
            }
            this->m_lightsArray.EraseQuick(index);
            break;

        case ncSpatial::N_SPATIAL_CAMERA:
            // do nothing, because cameras are not inserted in any cell's array
            break;

        default:
            index = this->m_entitiesArray.FindIndex(entity);
            if (index == -1)
            {
                return false;
            }
            this->m_entitiesArray.EraseQuick(index);
            break;

    }
    
    // entity shouldn't be destroyed
    n_assert2(entity, "miquelangel.rujula");
    //element->SetCell(0);
    spatialComp->SetCell(0);

    return true;
}

////------------------------------------------------------------------------------
///**
//    removes an element from the cell and a category without destroying it
//*/
//bool 
//nSpatialCell::RemoveSpatialElement(nSpatialElement *element, int category)
//{
//    nArray<nSpatialElement*> *catArray = 0;
//
//    // find the category
//    this->m_categories->Find(category, catArray);
//
//    if (catArray)
//    {
//        // find the element in the category
//        int index = catArray->FindIndex(element);
//        if (index > -1)
//        {
//            catArray->EraseQuick(index);
//            return true;
//        }
//    }
//    
//    return false;
//}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell and a category without destroying it
*/
bool 
nSpatialCell::RemoveEntity(nEntityObject *entity, int category)
{
    nArray<nEntityObject*> *catArray = 0;

    // find the category
    this->m_categories->Find(category, catArray);

    if (catArray)
    {
        // find the entity in the category
        int index = catArray->FindIndex(entity);
        if (index > -1)
        {
            catArray->EraseQuick(index);
            return true;
        }
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    removes an element from the cell and destroys it
*/
//bool
//nSpatialCell::DestroySpatialElement(nSpatialElement *element)
//{
//    n_assert2(element, "miquelangel.rujula");
//
//    // UGLY HACK: this animsroot maybe must be passed by parameter!!!!
//    //nSceneNode *animsRoot = (nSceneNode *)nKernelServer::Instance()->Lookup("/usr/animators");
//    //if (animsRoot)
//    //{
//    //    animsRoot->RenderContextDestroyed(element->GetRenderContext());
//    //}
//    
//    int index = -1;
//    switch(element->GetType())
//    {
//        case nSpatialElement::N_SPATIAL_PORTAL:
//            index = this->m_portalsArray.FindIndex((nSpatialPortal*)element);
//            this->m_portalsArray.Erase(index);
//            n_delete(element);
//            break;
//
//        case nSpatialElement::N_SPATIAL_LIGHT:
//            index = this->m_lightsArray.FindIndex((nSpatialLight*)element);
//            this->m_lightsArray.Erase(index);
//            n_delete(element);
//            break;
//
//        case nSpatialElement::N_SPATIAL_CAMERA:
//            // in this case, we'll not destroy the spatial camera here, we'll
//            // only remove it from the cell. The spatial server is who has to
//            // destroy it.
//            index = this->m_elementsArray.FindIndex(element);
//            this->m_elementsArray.EraseQuick(index);
//            element->SetCell(NULL);
//            break;
//
//        default:
//            index = this->m_elementsArray.FindIndex(element);
//            this->m_elementsArray.Erase(index);
//            //the entity destroys its elements
//            //n_delete(element); 
//            break;
//    }
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell and destroys it
*/
bool
nSpatialCell::DestroyEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");

    // UGLY HACK: this animsroot maybe must be passed by parameter!!!!
    //nSceneNode *animsRoot = (nSceneNode *)nKernelServer::Instance()->Lookup("/usr/animators");
    //if (animsRoot)
    //{
    //    animsRoot->RenderContextDestroyed(element->GetRenderContext());
    //}
    
    ncSpatial *spatialComp = entity->GetComponent(nType<ncSpatial>());
    int index = -1;
    switch(spatialComp->GetType())
    {
        case ncSpatial::N_SPATIAL_PORTAL:
            index = this->m_portalsArray.FindIndex(static_cast<nePortal*>(entity));
            if (index == -1)
            {
                return false;
            }
            this->m_portalsArray.EraseQuick(index);
            break;

        case ncSpatial::N_SPATIAL_LIGHT:
            index = this->m_lightsArray.FindIndex(static_cast<neLight*>(entity));
            if (index == -1)
            {
                return false;
            }
            this->m_lightsArray.EraseQuick(index);
            break;

        case ncSpatial::N_SPATIAL_CAMERA:
            // do nothing, because cameras are not inserted in any cell's array
            //cameraComp->SetCell(NULL);
            return true;
            //break;

        default:
            index = this->m_entitiesArray.FindIndex(entity);
            if (index == -1)
            {
                return false;
            }
            this->m_entitiesArray.EraseQuick(index);
            break;

    }

    n_assert2(entity, "miquelangel.rujula");

    return true;
}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell and a category and destroys it
*/
bool 
nSpatialCell::DestroyEntity(nEntityObject *entity, int category)
{
    nArray<nEntityObject*> *catArray = 0;

    // find the category
    this->m_categories->Find(category, catArray);

    if (catArray)
    {
        // find the entity in the category
        int index = catArray->FindIndex(entity);
        if (index > -1)
        {
            catArray->Erase(index);
            return true;
        }
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    removes an element from the cell and a category and destroys it
*/
//bool 
//nSpatialCell::DestroySpatialElement(nSpatialElement *element, int category)
//{
//    nArray<nSpatialElement*> *catArray = 0;
//
//    // find the category
//    this->m_categories->Find(category, catArray);
//
//    if (catArray)
//    {
//        // find the element in the category
//        int index = catArray->FindIndex(element);
//        if (index > -1)
//        {
//            catArray->Erase(index);
//            return true;
//        }
//    }
//    
//    return false;
//}

//------------------------------------------------------------------------------
/**
    set the bounding box of the cell
*/
void 
nSpatialCell::SetBBox(bbox3 &box)
{
    this->m_bbox = box;
}

//------------------------------------------------------------------------------
/**
    get the bounding box of the cell
*/
bbox3 
nSpatialCell::GetBBox()
{
    return this->m_bbox;
}

//------------------------------------------------------------------------------
/**
    set cell's transformation matrix
*/
void
nSpatialCell::SetTransformMatrix(matrix44 &matrix)
{
    this->m_transformationMatrix = matrix;
}

//------------------------------------------------------------------------------
/**
    get cell's transformation matrix
*/
matrix44 *
nSpatialCell::GetTransformMatrix()
{
    return &this->m_transformationMatrix; 
}

//------------------------------------------------------------------------------
/**
    get the number of elements in the cell
*/
//int
//nSpatialCell::GetNumElements() const
//{
//    //return this->m_elementsArray.Size() + 
//    return this->m_entitiesArray.Size() + 
//           this->m_portalsArray.Size()  +
//           this->m_lightsArray.Size();
//}

//------------------------------------------------------------------------------
/**
    get the number of entities in the cell
*/
int
nSpatialCell::GetNumEntities() const
{
    return this->m_entitiesArray.Size() + 
           this->m_portalsArray.Size()  +
           this->m_lightsArray.Size();
}

//------------------------------------------------------------------------------
/**
    get elements array
*/
//nArray<nSpatialElement*> *
//nSpatialCell::GetElementsArray()
//{
//    return &this->m_elementsArray;
//}

//------------------------------------------------------------------------------
/**
    get entities array
*/
nArray<nEntityObject*> *
nSpatialCell::GetEntitiesArray()
{
    return &this->m_entitiesArray;
}

//------------------------------------------------------------------------------
/**
    get portals array
*/
//nArray<nSpatialPortal*> *
//nSpatialCell::GetPortalsArray()
//{
//    return &this->m_portalsArray;
//}

//------------------------------------------------------------------------------
/**
    get portals array
*/
nArray<nePortal*> *
nSpatialCell::GetPortalsArray()
{
    return &this->m_portalsArray;
}

//------------------------------------------------------------------------------
/**
    get lights array
*/
//nArray<nSpatialLight*> *
nArray<neLight*> *
nSpatialCell::GetLightsArray()
{
    return &this->m_lightsArray;
}

//------------------------------------------------------------------------------
/**
    get visible entities array
*/
//nArray<nSpatialElement*> *
nArray<nEntityObject*> *
nSpatialCell::GetVisEntitiesArray()
{
    return &this->m_visEntitiesArray;
}

//------------------------------------------------------------------------------
/**
    add an element to the visible elements array
*/
//void 
//nSpatialCell::AppendVisElement(nSpatialElement* element)
//{
//    this->m_visElementsArray.Append(element);
//}

//------------------------------------------------------------------------------
/**
    add an entity to the visible entities array
*/
void 
nSpatialCell::AppendVisEntity(nEntityObject *entity)
{
    this->m_visEntitiesArray.Append(entity);
}

//------------------------------------------------------------------------------
/**
    set cell's identifier
*/
void 
nSpatialCell::SetId(int id)
{
    this->m_cellId = id;
}

//------------------------------------------------------------------------------
/**
    get cell's identifier
*/
int 
nSpatialCell::GetId() const
{
    return this->m_cellId;
}

//------------------------------------------------------------------------------
/**
    create a new category
*/
void  
nSpatialCell::CreateCategory(int catId)
{
    //this->m_categories->Add(catId, n_new(nArray<nSpatialElement*>(32, 16)));
    this->m_categories->Add(catId, n_new(nArray<nEntityObject*>(32, 16)));
}

//------------------------------------------------------------------------------
/**
    remove a category
*/
bool 
nSpatialCell::RemoveCategory(int catId)
{
    //nArray<nSpatialElement*> **pCategory;
    nArray<nEntityObject*> **pCategory;
    if (!this->m_categories->FindPtr(catId, pCategory))
    {
        return false;
    }
    
    this->m_categories->Rem(catId);

    return true;
}

//------------------------------------------------------------------------------
/**
    set frame id
*/
void 
nSpatialCell::SetFrameId(int id)
{
    this->m_frameId = id;
}

//------------------------------------------------------------------------------
/**
    get frame id
*/
int 
nSpatialCell::GetFrameId()
{
    return this->m_frameId;
}

//------------------------------------------------------------------------------
/**
    return true if this cell has been determined visible in this frame
*/
bool
nSpatialCell::IsVisible()
{
    return (this->m_frameId == nSpatialServer::Instance()->GetFrameId());
}

//------------------------------------------------------------------------------
/**
    set the AI's polygon container. Be careful, deletes the previous one!
*/
void 
nSpatialCell::SetPolygonContainer(nPolygonContainer *polygonContainer)
{
    n_assert2(polygonContainer, "miquelangel.rujula");

    // free the current polygons container
    n_delete(this->m_polygonContainer);
    // assign the new polygons container
    this->m_polygonContainer = polygonContainer;
}


//------------------------------------------------------------------------------
/**
*/
nPolygonContainer *
nSpatialCell::GetPolygonContainer()
{
    return this->m_polygonContainer;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialCell::ReadStaticInfo(const TiXmlElement *cellElem)
{
    n_assert2(cellElem, "miquelangel.rujula");

    char smallCharBuff[5];

    // cell's type
    TiXmlElement *cellTypeElem = cellElem->FirstChildElement("Type");
    if (!cellTypeElem)
    {
        return false;
    }
    this->m_cellType = atoi(cellTypeElem->Attribute("Value"));

    // cell's id
    TiXmlElement *cellIdElem = cellElem->FirstChildElement("Id");
    if (!cellIdElem)
    {
        return false;
    }
    this->m_cellId = atoi(cellIdElem->Attribute("Value"));

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
    n_assert2(bboxElem, "miquelangel.rujula");
    bbox3 box;
    box.vmin.x = (float)atof(bboxElem->Attribute("VminX"));
    box.vmin.y = (float)atof(bboxElem->Attribute("VminY"));
    box.vmin.z = (float)atof(bboxElem->Attribute("VminZ"));
    box.vmax.x = (float)atof(bboxElem->Attribute("VmaxX"));
    box.vmax.y = (float)atof(bboxElem->Attribute("VmaxY"));
    box.vmax.z = (float)atof(bboxElem->Attribute("VmaxZ"));
    this->SetBBox(box);



    //// NOTE: entities won't be saved into the cells. Instead, entities will know which cell they belong to
    //// catch the information of the spatial elements contained in cellElem
    //if (!this->ReadStaticElements(cellElem))
    //{
    //    return false;
    //}

    //// catch the information of the spatial portals contained in cellElem
    //if (!this->ReadPortals(cellElem))
    //{
    //    return false;
    //}

    return true;
}

////------------------------------------------------------------------------------
///**
//    read all the static spatial elements in the given TiXmlElement
//*/
//bool 
//nSpatialCell::ReadStaticElements(const TiXmlElement *cellElem)
//{
//    n_assert2(cellElem, "miquelangel.rujula");
//
//    // cell's spatial elements
//    TiXmlElement *spatialElementElem = cellElem->FirstChildElement("SpatialElement");
//    while(spatialElementElem)
//    {
//        nSpatialElement *newSpatialElement = n_new(nSpatialElement);
//        if (!newSpatialElement->ReadXmlElement(spatialElementElem))
//        {
//            return false;
//        }
//
//        // add the new spatial element to this cell
//        this->AddSpatialElement(newSpatialElement);
//
//        // get next spatial element
//        spatialElementElem = spatialElementElem->NextSiblingElement("SpatialElement");
//    }
//
//    return true;
//}

////------------------------------------------------------------------------------
///**
//    NOTE: the cell has to know which space it belongs to, before calling
//    this method!
//*/
//bool 
//nSpatialCell::ReadPortals(const TiXmlElement *cellElem)
//{
//    n_assert2( cellElem, "miquelangel.rujula");
//    
//    if (!this->m_parentSpace)
//    {
//        return false;
//    }
//
//    // cell's spatial portals
//    TiXmlElement *spatialPortalElem = cellElem->FirstChildElement("SpatialPortal");
//    while(spatialPortalElem)
//    {
//        nSpatialPortal *newSpatialPortal = n_new(nSpatialPortal);
//        if (!newSpatialPortal->ReadXmlElement(spatialPortalElem))
//        {
//            return false;
//        }
//        
//        this->m_parentSpace->AddUnsolvedPortal(newSpatialPortal);
//
//        // add the new spatial portal to this cell
//        this->AddSpatialPortal(newSpatialPortal);
//
//        // get next spatial portal
//        spatialPortalElem = spatialPortalElem->NextSiblingElement("SpatialPortal");
//    }
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
    NOTE: the cell has to know which space it belongs to, before calling
    this method!
*/
//bool 
//nSpatialCell::ReadPortals(const TiXmlElement *cellElem)
//{
//    n_assert2( cellElem, "miquelangel.rujula");
//
//    nString portalIdStr;
//    //char portalIdStr[256];
//
//    if (!this->m_parentSpace)
//    {
//        return false;
//    }
//
//    nEntityObjectId portalId = -1;
//
//    // cell's portals
//    TiXmlElement *portalElem = cellElem->FirstChildElement("Portal");
//    while(portalElem)
//    {
//        portalIdStr = portalElem->Attribute("Id");
//        portalId = portalIdStr.AsInt();
//        nEntityServer::Instance()->FindEntityObject(portalId);
//
//
//
//
//
//
//
//        this->m_parentSpace->AddUnsolvedPortal(newSpatialPortal);
//
//        // add the new spatial portal to this cell
//        this->AddSpatialPortal(newSpatialPortal);
//
//        // get next spatial portal
//        spatialPortalElem = spatialPortalElem->NextSiblingElement("SpatialPortal");
//    }
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialCell::WriteStaticInfo(TiXmlElement *cellElem)
{
    n_assert2(cellElem, "miquelangel.rujula");

    char smallCharBuff[5];
    char longCharBuff[50];

    // cell's type
    TiXmlElement cellTypeElem("Type");
    cellTypeElem.SetAttribute("Value", this->m_cellType);
    cellElem->InsertEndChild(cellTypeElem);

    // cell's id
    TiXmlElement cellIdElem("Id");
    cellIdElem.SetAttribute("Value", this->m_cellId);
    cellElem->InsertEndChild(cellIdElem);

    // cell's transformation matrix
    TiXmlElement cellMatrixElem("TransformMatrix");
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {            
            sprintf(smallCharBuff, "M%d%d", i, j);
            sprintf(longCharBuff, "%f", this->GetTransformMatrix()->m[i][j]);
            cellMatrixElem.SetAttribute(smallCharBuff, longCharBuff);
        }
    }
    cellElem->InsertEndChild(cellMatrixElem);

    // cell's bounding box
    TiXmlElement bboxElem("BBox");
    sprintf(longCharBuff, "%f", this->GetBBox().vmin.x);
    bboxElem.SetAttribute("VminX", longCharBuff);
    sprintf(longCharBuff, "%f", this->GetBBox().vmin.y);
    bboxElem.SetAttribute("VminY", longCharBuff);
    sprintf(longCharBuff, "%f", this->GetBBox().vmin.z);
    bboxElem.SetAttribute("VminZ", longCharBuff);
    sprintf(longCharBuff, "%f", this->GetBBox().vmax.x);
    bboxElem.SetAttribute("VmaxX", longCharBuff);
    sprintf(longCharBuff, "%f", this->GetBBox().vmax.y);
    bboxElem.SetAttribute("VmaxY", longCharBuff);
    sprintf(longCharBuff, "%f", this->GetBBox().vmax.z);
    bboxElem.SetAttribute("VmaxZ", longCharBuff);

    cellElem->InsertEndChild(bboxElem);

    //// store the static entities in this cell
    //if (!this->AddStaticEntities(cellElem))
    //{
    //    return false;
    //}

    return true;
}

//------------------------------------------------------------------------------
/**
    Write the dynamic information of the cell.
    It writes the dynamic spatial elements it contains in the correponding
    folder and file, depending on the layer where the elements are.

    It catches the first dynamic element in the elements array. Then, searches
    all the elements that are in the same layer than the first dynamic one, and 
    writes them in the same file, named 'CellId' (where 'Id' is the cells id),
    also creating a folder for that layer if wasn't created yet. While the 
    algorithm traverses the array, it erases all the static elements and those
    that are already written.

    Finally, it also fills up the parameter array 'layers' with the names of 
    the persisted layers.
*/
//bool 
//nSpatialCell::WriteDynamicInfo(nArray<nString> &persistedLayers)
//{
//    nFileServer2 *fileServer = nFileServer2::Instance();
//
//    nArray<nSpatialElement*> tempArray = this->m_elementsArray;
//    int currentLayerId = -1;
//    bool getLayer;
//    
//    // while we have spatial elements to persist
//    while(tempArray.Size() > 0)
//    {
//        TiXmlDocument cellDoc;
//        TiXmlElement cellElem("Cell");
//        bool fileCreated = false;
//        nString layerDirName(this->m_parentSpace->GetSpaceDirectory());
//        getLayer = true;
//
//        // iterate over the remaining elements in the array
//        nArray<nSpatialElement*>::iterator pSpatialElement = tempArray.Begin();
//        while(pSpatialElement != tempArray.End())
//        {
//            if((*pSpatialElement)->IsDynamic())
//            {
//                if (getLayer)
//                {
//                    // compose layer's directory path
//                    currentLayerId = (*pSpatialElement)->GetLayer();
//                    layerDirName.Append("layer");
//                    layerDirName.AppendInt(currentLayerId);
//                    
//                    // check if the directory already exists
//                    if (!fileServer->DirectoryExists(layerDirName.Get()))
//                    {
//                        // if the layer's directory doesn't exist yet, create it
//                        fileServer->MakePath(layerDirName.Get());
//                    }
//        
//                    // append the layer id
//                    if (persistedLayers.FindIndex(layerDirName) == -1)
//                    {
//                        persistedLayers.Append(layerDirName);
//                    }
//
//                    // directory created. Now, create cell's tinyxml document
//                    nString cellFileName("/Cell");
//                    cellFileName.AppendInt(this->m_cellId);
//                    
//                    layerDirName.Append(cellFileName);
//                    layerDirName.Append(".xml");
//
//                    nString pathName(fileServer->ManglePath(layerDirName.Get()));
//                    cellDoc.SetValue(pathName.Get());
//                    
//                    // cell id
//                    cellElem.SetAttribute("Id", this->m_cellId);
//                    
//                    fileCreated = true;
//                    getLayer = false;
//                }
//
//                if ((*pSpatialElement)->GetLayer() == currentLayerId)
//                {
//                    TiXmlElement spatialElementElem("SpatialElement");
//        
//                    // fill the TiXmlElement corresponding to the element with its info
//                    if (!(*pSpatialElement)->FillXmlElement(&spatialElementElem))
//                    {
//                        return false;
//                    }
//                    
//                    // append the spatial element's information to the cell's document
//                    cellElem.InsertEndChild(spatialElementElem);
//
//                    // remove the element from the temporal array
//                    tempArray.EraseQuick(pSpatialElement);
//
//                    // don't increase the pointer to the spatial element, because
//                    // when we have erased the actual one the following one was
//                    // moved to its position. Then, the 'next one' is referenced
//                    // by 'pSpatialElement'
//                    continue;
//                }
//            }
//            else
//            {
//                // remove the element from the temporal array
//                tempArray.EraseQuick(pSpatialElement);
//
//                // don't increase the pointer to the spatial element, because
//                // when we have erased the actual one the following one was
//                // moved to its position. Then, the 'next one' is referenced
//                // by 'pSpatialElement'
//                continue;
//            }
//
//            pSpatialElement++;
//        }
//
//        if (fileCreated)
//        {
//            cellDoc.InsertEndChild(cellElem);
//            cellDoc.SaveFile();
//        }
//    }
//
//    // save the file
//    return true;
//}

////------------------------------------------------------------------------------
///**
//*/
//bool
//nSpatialCell::AddStaticElements(TiXmlElement *cellElem)
//{
//    n_assert2(cellElem, "miquelangel.rujula");
//
//    // spatial elements of the current cell
//    for (nArray<nSpatialElement*>::iterator pSpatialElement  = this->m_elementsArray.Begin();
//                                            pSpatialElement != this->m_elementsArray.End();
//                                            pSpatialElement++)
//    {
//        // check if the element is static
//        if (!(*pSpatialElement)->IsDynamic())
//        {
//            TiXmlElement spatialElementElem("SpatialElement");
//            
//            // fill the TiXmlElement corresponding to the element with its info
//            if (!(*pSpatialElement)->FillXmlElement(&spatialElementElem))
//            {
//                return false;
//            }
//
//            cellElem->InsertEndChild(spatialElementElem);
//        }
//    }
//
//    // spatial portals of the current cell
//    for (nArray<nSpatialPortal*>::iterator pSpatialPortal  = this->m_portalsArray.Begin();
//                                           pSpatialPortal != this->m_portalsArray.End();
//                                           pSpatialPortal++)
//    {
//        // check if the element is static
//        if (!(*pSpatialPortal)->IsDynamic())
//        {
//            TiXmlElement spatialPortalElem("SpatialPortal");
//            
//            // fill the TiXmlElement corresponding to the portal with its info
//            if (!(*pSpatialPortal)->FillXmlElement(&spatialPortalElem))
//            {
//                return false;
//            }
//
//            cellElem->InsertEndChild(spatialPortalElem);
//        }
//    }
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
*/
//bool
//nSpatialCell::AddStaticEntities(TiXmlElement *cellElem)
//{
//    n_assert2(cellElem, "miquelangel.rujula");
//
//    ncSpatial *spatialComp = 0;
//    char entityID[256];
//
//    // current cell's entities
//    for (nArray<nEntityObject*>::iterator pEntity  = this->m_entitiesArray.Begin();
//                                          pEntity != this->m_entitiesArray.End();
//                                          pEntity++)
//    {
//        spatialComp = (*pEntity)->GetComponent(nType<ncSpatial>());
//
//        // check if the entity is static
//        if (!spatialComp->IsDynamic())
//        {
//            TiXmlElement entityElem("Entity");
//            sprintf(entityID, "%x", (*pEntity)->GetId());
//
//            entityElem.SetAttribute("Id", entityID);
//
//            cellElem->InsertEndChild(entityElem);
//        }
//    }
//
//    // current cell's portals
//    for (nArray<nePortal*>::iterator pPortal  = this->m_portalsArray.Begin();
//                                     pPortal != this->m_portalsArray.End();
//                                     pPortal++)
//    {
//        TiXmlElement portalElem("Portal");
//        
//        sprintf(entityID, "%x", (*pPortal)->GetId());
//        portalElem.SetAttribute("Id", entityID);
//
//        cellElem->InsertEndChild(portalElem);
//    }
//
//    // current cell's lights
//    for (nArray<neLight*>::iterator pLight  = this->m_lightsArray.Begin();
//                                    pLight != this->m_lightsArray.End();
//                                    pLight++)
//    {
//        TiXmlElement lightElem("Light");
//        
//        sprintf(entityID, "%x", (*pLight)->GetId());
//        lightElem.SetAttribute("Id", entityID);
//
//        cellElem->InsertEndChild(lightElem);
//    }
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
    visibility visitor processing for a basic spatial cell.
*/
void 
//nSpatialCell::Accept(nVisibilityVisitor &/*visitor*/, int /*recursiondepth*/)
nSpatialCell::Accept(nVisibleFrustumVisitor &/*visitor*/, int /*recursiondepth*/)
{
    this->SetFrameId(nSpatialServer::Instance()->GetFrameId());
    //visitor.Visit(this, recursiondepth);
}

//------------------------------------------------------------------------------
/**
    spatial visitor processing for a basic spatial cell.
*/
/*void 
nSpatialCell::Accept(nSpatialVisitor &visitor, int recursiondepth)
{
    visitor.Visit(this, recursiondepth);
}
*/

