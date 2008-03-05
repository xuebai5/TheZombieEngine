#include "precompiled/pchnspatial.h"
#include "nspatial/nspatialspace.h"
//#include "nspatial/nspatialelements.h"
//#include "nspatial/nspatialportal.h"
#include "kernel/ndirectory.h"
#include "entity/nentity.h"
#include "zombieentity/neportal.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialportal.h"

nNebulaScriptClass(nSpatialSpace, "nroot");

//------------------------------------------------------------------------------
/**
    constructor
*/
nSpatialSpace::nSpatialSpace() : 
m_spaceType(N_SPATIAL_SPACE)
{
    this->m_bbox.begin_extend();
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialSpace::~nSpatialSpace()
{
    n_assert2(this->m_cellsArray.Size() == 0, "miquelangel.rujula");
}

//------------------------------------------------------------------------------
/**
    Destroys all the cells contained in the space.
    Call it just before deleting the space.
*/
void 
nSpatialSpace::DestroySpace()
{
    for (nArray<nSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                         cell != this->m_cellsArray.End();
                                         cell++)
    {
        (*cell)->DestroyCell();
        n_delete((*cell));
    }

    this->m_cellsArray.Reset();
}

//------------------------------------------------------------------------------
/**
    add a cell to the space
*/
void
nSpatialSpace::AddSpatialCell(nSpatialCell *cell)
{
    n_assert2(cell, "miquelangel.rujula");

    this->m_cellsArray.Append(cell);
    cell->SetParentSpace(this);
    this->m_bbox.extend(cell->GetBBox());
}

//------------------------------------------------------------------------------
/**
    remove a cell from the space
*/
bool 
nSpatialSpace::RemoveSpatialCell(nSpatialCell *cell)
{
    int index = this->m_cellsArray.FindIndex(cell);
    this->m_cellsArray.EraseQuick(index);
    return true;
}

//------------------------------------------------------------------------------
/**
    remove a cell from the space and destroys it 
    Warning: This method erases and destroys the cell, and all inside it!!
*/
bool 
nSpatialSpace::DestroySpatialCell(nSpatialCell *cell)
{
    int index = this->m_cellsArray.FindIndex(cell);
    this->m_cellsArray.Erase(index);
    return true;
}

//------------------------------------------------------------------------------
/**
    add an element to the space, and puts it in the corresponding cell
    Rewrite it in subclasses
*/
//bool 
//nSpatialSpace::AddSpatialElement(nSpatialElement * /*element*/)
//{
//    return false;
//}

//------------------------------------------------------------------------------
/**
    add an element to the space in the corresponding cell and category, depending
    on the flags
*/
//bool 
//nSpatialSpace::AddCatSpatialElement(nSpatialElement *element, int category, int flags)
//{
//    n_assert2(element, "NULL pointer to an spatial element.");
//
//    // We have to transform the element's transformation matrix and its position to space coordinates.
//    matrix44 worldToSpaceMatrix(this->m_transformationMatrix);
//    worldToSpaceMatrix.invert_simple();
//
//    // transform the element's bounding box to space coordinates
//    bbox3 elementBox = element->GetBBox();
//    elementBox.transform(worldToSpaceMatrix);
//    element->SetBBox(elementBox);
//
//    // transform the element's position
//    vector3 spaceElementPos = element->GetPosition();
//    element->SetPosition(worldToSpaceMatrix * spaceElementPos);
//
//    // now, the element's transformation matrix and bounding box are in space coordinates
//    
//    if (flags & nSpatialServer::INS_USE_BBOX)
//    {
//        return this->AddSpatialElementByBBox(element, category, flags);
//    }
//    else if (flags & nSpatialServer::INS_USE_POSITION)
//    {
//        return this->AddSpatialElementByPos(element, category, flags);
//    }
//    else if (flags & nSpatialServer::INS_USE_MODEL)
//    {
//        return this->AddSpatialElementByModel(element, category, flags);
//    }
//
//    return false;
//}

//------------------------------------------------------------------------------
/**
    add an entity to the space in the corresponding cell and category, depending
    on the flags
*/
bool 
nSpatialSpace::AddCatEntity(nEntityObject *entity, int category, int flags)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");

    // We have to transform the entity's spatial component position to space coordinates.
    matrix44 worldToSpaceMatrix(this->m_transformationMatrix);
    worldToSpaceMatrix.invert_simple();

    ncSpatial *spatialComp = entity->GetComponent(nComponentType<ncSpatial>());

    // transform the element's bounding box to space coordinates
    //bbox3 elementBox = element->GetBBox();
    bbox3 elementBox = *spatialComp->GetBBox();
    elementBox.transform(worldToSpaceMatrix);
    //element->SetBBox(elementBox);
    spatialComp->SetBBox(elementBox);

    // transform the element's position
    //vector3 spaceElementPos = element->GetPosition();
    vector3 spaceElementPos = spatialComp->GetPosition();
    //element->SetPosition(worldToSpaceMatrix * spaceElementPos);
    spatialComp->SetPosition(worldToSpaceMatrix * spaceElementPos);

    // now, the spatial component's transformation matrix and bounding box are in space coordinates
    
    if (flags & nSpatialServer::INS_USE_BBOX)
    {
        //return this->AddSpatialElementByBBox(element, category, flags);
        return this->AddEntityByBBox(entity, category, flags);
    }
    else if (flags & nSpatialServer::INS_USE_POSITION)
    {
        //return this->AddSpatialElementByPos(element, category, flags);
        return this->AddEntityByPos(entity, category, flags);
    }
    else if (flags & nSpatialServer::INS_USE_MODEL)
    {
        //return this->AddSpatialElementByModel(element, category, flags);
        return this->AddEntityByModel(entity, category, flags);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell and category.

    Assuming that the entity's spatial component is in space coordinates (its bounding box).
*/
bool 
//nSpatialSpace::AddSpatialElementByBBox(nSpatialElement * /*element*/, 
nSpatialSpace::AddEntityByBBox(nEntityObject * /*entity*/, 
                               int /*category*/, 
                               const int /*flags*/)
{ 
    return false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell and category.

    Assuming that the entity's spatial component is in space coordinates (its model).

    Rewrite it in subclasses.
*/
bool 
//nSpatialSpace::AddSpatialElementByModel(nSpatialElement * /*element*/, 
nSpatialSpace::AddEntityByModel(nEntityObject * /*entity*/, 
                                int /*category*/, 
                                const int /*flags*/)
{ 
    return false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell if the
    INS_ONE_CELL flag is set, or in all the cells whose bounding boxes contain
    the point if INS_ALL_CELLS is set. The entity is inserted in the given 
    category.

    Assuming that the entity's spatial component is in space coordinates (its position).

    Rewrite it in subclasses.
*/
bool 
//nSpatialSpace::AddSpatialElementByPos(nSpatialElement * /*element*/, 
nSpatialSpace::AddEntityByPos(nEntityObject * /*entity*/, 
                              int /*category*/, 
                              const int /*flags*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    remove an element from the space
*/
//bool 
//nSpatialSpace::RemoveSpatialElement(nSpatialElement *element)
//{
//    n_assert2(element, "miquelangel.rujula");
//
//    nSpatialCell *cell = element->GetCell();
//    if (!cell)
//    {
//        return false;
//    }
//    return cell->RemoveSpatialElement(element);
//}

//------------------------------------------------------------------------------
/**
    remove an element from the space and destroys it
*/
//bool 
//nSpatialSpace::DestroySpatialElement(nSpatialElement *element)
//{
//    n_assert2(element, "miquelangel.rujula");
//
//    nSpatialCell *cell = element->GetCell();
//    if (!cell)
//    {
//        return false;
//    }
//    return cell->DestroySpatialElement(element);
//}

//------------------------------------------------------------------------------
/**
    move an element from a cell to another one. The element contains the origin cell
*/
//bool 
//nSpatialSpace::MoveSpatialElement(nSpatialElement *element, nSpatialCell *destCell)
//{
//    n_assert2(element, "miquelangel.rujula");
//    n_assert2(destCell, "miquelangel.rujula");
//
//    nSpatialCell *origCell = element->GetCell();
//    if (!origCell)
//    {
//        return false;
//    }
//    
//    if (!origCell->RemoveSpatialElement(element))
//    {
//        return false;
//    }
//    destCell->AddSpatialElement(element);
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
    add an entity to the space, and puts it in the corresponding cell
    Rewrite it in subclasses
*/
bool 
nSpatialSpace::AddEntity(nEntityObject* /*entity*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    remove an entity from the space
*/
bool 
nSpatialSpace::RemoveEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");

    ncSpatial *spatialComponent = entity->GetComponent(nType<ncSpatial>());
    n_assert2(spatialComponent, "Error: entity hasn't spatial component!");
    //nSpatialCell *cell = element->GetCell();
    //nSpatialCell *cell = spatialComponent->GetSpatialElement()->GetCell();
    nSpatialCell *cell = spatialComponent->GetCell();
    if (!cell)
    {
        return false;
    }
    return cell->RemoveEntity(entity);
}

//------------------------------------------------------------------------------
/**
    remove an entity from the space and destroys it
*/
bool 
nSpatialSpace::DestroyEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");

    ncSpatial *spatialComponent = entity->GetComponent(nType<ncSpatial>());
    n_assert2(spatialComponent, "Error: entity hasn't spatial component!");
    //nSpatialCell *cell = element->GetCell();
    //nSpatialCell *cell = spatialComponent->GetSpatialElement()->GetCell();
    nSpatialCell *cell = spatialComponent->GetCell();
    if (!cell)
    {
        return false;
    }
    return cell->DestroyEntity(entity);
}

//------------------------------------------------------------------------------
/**
    move an entity from a cell to another one. The entity's spatial component 
    contains the origin cell
*/
bool 
nSpatialSpace::MoveEntity(nEntityObject *entity, nSpatialCell *destCell)
{
    n_assert2(entity, "miquelangel.rujula");
    n_assert2(destCell, "miquelangel.rujula");

    ncSpatial *spatialComponent = entity->GetComponent(nType<ncSpatial>());
    n_assert2(spatialComponent, "Error: entity hasn't spatial component!");
    //nSpatialCell *origCell = element->GetCell();
    //nSpatialCell *origCell = spatialComponent->GetSpatialElement()->GetCell();
    nSpatialCell *origCell = spatialComponent->GetCell();
    if (!origCell)
    {
        return false;
    }
    
    //if (!origCell->RemoveSpatialElement(element))
    if (!origCell->RemoveEntity(entity))
    {
        return false;
    }
    //destCell->AddSpatialElement(element);
    destCell->AddEntity(entity);

    return true;
}

//------------------------------------------------------------------------------
/**
    create a new category
*/
void 
nSpatialSpace::CreateCategory(int catId)
{
    // create the new category in every cell in this space
    for (nArray<nSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                         pCell != this->m_cellsArray.End();
                                         pCell++)
    {
        (*pCell)->CreateCategory(catId);
    }
}

//------------------------------------------------------------------------------
/**
    remove a category
*/
bool 
nSpatialSpace::RemoveCategory(int catId)
{
    // remove the category from every cell in this space
    for (nArray<nSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                         pCell != this->m_cellsArray.End();
                                         pCell++)
    {
        if (!(*pCell)->RemoveCategory(catId))
        {
            // can't remove the category from the cell
            return false;
        }
    }

    // category removed from all the cells
    return true;
}

////------------------------------------------------------------------------------
///**
//    get the number of elements in the space
//*/
//int 
//nSpatialSpace::GetNumElements() const
//{
//    int numElements = 0;
//    for (nArray<nSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
//                                         cell != this->m_cellsArray.End();
//                                         cell++)
//    {
//        numElements += (*cell)->GetNumElements();
//    }
//
//    return numElements;
//}

//------------------------------------------------------------------------------
/**
    get the number of entities in the space
*/
int 
nSpatialSpace::GetNumEntities() const
{
    int numEntities= 0;
    for (nArray<nSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                         cell != this->m_cellsArray.End();
                                         cell++)
    {
        numEntities += (*cell)->GetNumEntities();
    }

    return numEntities;
}

//------------------------------------------------------------------------------
/**
    get the space range lights array
*/
//nArray<nSpatialLight*> *
nArray<neLight*> *
nSpatialSpace::GetSpaceRangeLightsArray()
{
    return &this->m_spaceRangeLights;
}

//------------------------------------------------------------------------------
/**
    Searches the cell that contains the given point.
    Rewrite it in subclasses.
*/
nSpatialCell *
nSpatialSpace::SearchCellByPoint(const vector3 &/*point*/)
{
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    bounding box, and fills the parameter array with them.
    'box' has to be in world coordinates.
    It doesn't reset the parameter array.
*/
void 
nSpatialSpace::SearchCellsIntersecting(const bbox3 &/*box*/, nArray<nSpatialCell*> * /*cells*/)
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    sphere, and fills the parameter array with them.
    'sph' has to be in world coordinates.
    It doesn't reset the parameter array.
*/
void 
nSpatialSpace::SearchCellsIntersecting(const sphere &/*sph*/, nArray<nSpatialCell*> * /*cells*/)
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches all the spatial elements whose bounding box intersects with the 
    given bounding box, and fills the parameter array with them.
    'box' has to be in world coordinates.
    It doesn't reset the parameter array.
*/
//void 
//nSpatialSpace::SearchElementsIntersecting(const bbox3 &/*box*/, nArray<nSpatialElement*> * /*elements*/)
//{
//    // do nothing. Rewrite it in subclasses.
//}

//------------------------------------------------------------------------------
/**
    Searches all the entities whose bounding box intersects with the 
    given bounding box, and fills the parameter array with them.
    'box' has to be in world coordinates.
    It doesn't reset the parameter array.
*/
void 
nSpatialSpace::SearchEntitiesIntersecting(const bbox3 &/*box*/, nArray<nEntityObject*> * /*entities*/)
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches all the spatial elements whose bounding box intersects with the 
    given sphere, and fills the parameter array with them.
    'sph' has to be in world coordinates.
    It doesn't reset the parameter array.
*/
//void 
//nSpatialSpace::SearchElementsIntersecting(const sphere &/*sph*/, nArray<nSpatialElement*> * /*elements*/)
//{
//    // do nothing. Rewrite it in subclasses.
//}

//------------------------------------------------------------------------------
/**
    Searches all the entities whose bounding box intersects with the 
    given sphere, and fills the parameter array with them.
    'sph' has to be in world coordinates.
    It doesn't reset the parameter array.
*/
void 
nSpatialSpace::SearchEntitiesIntersecting(const sphere &/*sph*/, nArray<nEntityObject*> * /*entities*/)
{
    // do nothing. Rewrite it in subclasses.
}

// --- loading / saving ---

////------------------------------------------------------------------------------
///**
//    load a space from a file.
//*/
//bool 
//nSpatialSpace::LoadSpace()
//{
//    // load the static information of this space
//    if (!this->LoadStaticInfo())
//    {
//        return false;
//    }
//
//    // load the dynamic information of this space
//    if (!this->LoadDynamicInfo())
//    {
//        return false;
//    }
//
//    return true;
//}
//
////------------------------------------------------------------------------------
///**
//*/
//bool
//nSpatialSpace::LoadStaticInfo()
//{
//    nFileServer2 *fileServer = nFileServer2::Instance();
//
//    // check whether the space's directory exists and the space file is set
//    if (!fileServer->DirectoryExists(this->m_dirName.Get()) || this->m_spaceFile.IsEmpty())
//    {
//        return false;
//    }
//
//    nString pathName(this->m_dirName);
//    pathName.Append(this->m_spaceFile);
//    nString realPathName(fileServer->ManglePath(pathName.Get()));
//    TiXmlDocument doc(realPathName.Get());
//    
//    if (doc.LoadFile())
//    {
//        // file loaded and parsed. Proceed to catch spatial info from doc
//        TiXmlElement *spaceElem = doc.FirstChildElement("Space");
//        if (!this->ReadStaticInfo(spaceElem))
//        {
//            return false;
//        }
//    }
//
//    return true;
//}
//
////------------------------------------------------------------------------------
///**
//*/
//bool
//nSpatialSpace::LoadDynamicInfo()
//{
//    nFileServer2 *fileServer = nFileServer2::Instance();
//
//    // check whether the space's directory exists
//    if (!fileServer->DirectoryExists(this->m_dirName.Get()))
//    {
//        return false;
//    }
//
//    nArray<nString> layerFolders;
//    nString pathName(this->m_dirName);
//    pathName.Append("layers.xml");
//    nString realPathName(fileServer->ManglePath(pathName.Get()));
//    TiXmlDocument doc(realPathName.Get());
//    
//    if (doc.LoadFile())
//    {
//        // file loaded and parsed. Proceed to catch layer folder names from doc
//        TiXmlElement *layerElem = doc.FirstChildElement("layerDir");
//        while(layerElem)
//        {
//            layerFolders.Append(nString(layerElem->Attribute("Value")));
//            layerElem = layerElem->NextSiblingElement("layerDir");
//        }
//    }
//
//    // read the dynamic information in every layer
//    for (nArray<nString>::iterator layerFolder  = layerFolders.Begin();
//                                   layerFolder != layerFolders.End();
//                                   layerFolder++)
//    {
//        this->ReadDynamicInfo(layerFolder);
//    }
//
//    return true;
//}

////------------------------------------------------------------------------------
///**
//*/
//bool
//nSpatialSpace::ReadStaticInfo(const TiXmlElement *spaceElem)
//{
//    n_assert2(spaceElem, "miquelangel.rujula");
//
//    const char *elemSpaceType = spaceElem->Attribute("Type");
//    nClass *thisClass = this->GetClass();
//    const char *thisSpaceType = thisClass->GetProperName();
//    
//    if (!spaceElem || strcmp(elemSpaceType, thisSpaceType))
//    {
//        return false;
//    }
//    
//     // read the bbox information
//    TiXmlElement *bboxElem = spaceElem->FirstChildElement("BBox");
//    n_assert2(bboxElem, "miquelangel.rujula");
//    bbox3 box;
//    box.vmin.x = (float)atof(bboxElem->Attribute("VminX"));
//    box.vmin.y = (float)atof(bboxElem->Attribute("VminY"));
//    box.vmin.z = (float)atof(bboxElem->Attribute("VminZ"));
//    box.vmax.x = (float)atof(bboxElem->Attribute("VmaxX"));
//    box.vmax.y = (float)atof(bboxElem->Attribute("VmaxY"));
//    box.vmax.z = (float)atof(bboxElem->Attribute("VmaxZ"));
//    this->SetBBox(box);
//
//    // catch the information corresponding to the cells contained in spaceElem
//    if (!this->ReadCellsStaticInfo(spaceElem))
//    {
//        return false;
//    }
//    
//    // solve the 'otherSideCell' pointer of the portals
//    this->SolvePortalsPointers();
//
//    return true;    
//}

////------------------------------------------------------------------------------
///**
//*/
//bool 
//nSpatialSpace::ReadCellsStaticInfo(const TiXmlElement *spaceElem)
//{
//    n_assert2(spaceElem, "miquelangel.rujula");
//
//    // cells
//    TiXmlElement *cellElem = spaceElem->FirstChildElement("Cell");
//    // a space has to have at least one cell
//    if (!cellElem)
//    {
//        return false;
//    }
//
//    while(cellElem)
//    {
//        nSpatialCell *newCell = n_new(nSpatialCell);
//        newCell->SetParentSpace(this);
//        if (!newCell->ReadStaticInfo(cellElem))
//        {
//            return false;
//        }
//
//        // add the new cell to the space
//        this->AddSpatialCell(newCell);
//
//        // get next cell
//        cellElem = cellElem->NextSiblingElement("Cell");
//    }
//
//    return true;
//}

////------------------------------------------------------------------------------
///**
//    read the dynamic information in the parameter layer folder
//*/
//bool
//nSpatialSpace::ReadDynamicInfo(nString *layerFolder)
//{
//    n_assert2(layerFolder, "miquelangel.rujula");
//
//    nFileServer2 *fileServer = nFileServer2::Instance();
//
//    // check whether the space's directory exists
//    if (!fileServer->DirectoryExists(layerFolder->Get()))
//    {
//        return false;
//    }
//
//    // open the layer's folder
//    nDirectory *directory = fileServer->NewDirectoryObject();
//    if (!directory->Open(layerFolder->Get()))
//    {
//        return false;
//    }
//
//    // go to the first file in the layer's folder
//    if (!directory->SetToFirstEntry()) // is this line necessary???????
//    {
//        return false;
//    }
//
//    // read the information for each file in the folder
//    bool cont = true;
//    while (cont)
//    {
//        // load the file
//        TiXmlDocument cellDoc(directory->GetEntryName());
//        cellDoc.LoadFile();
//
//        // read the information in the file
//        TiXmlElement *cellElem = cellDoc.FirstChildElement("Cell");
//        if (!cellElem || !this->ReadDynamicInfo(cellElem))
//        {
//            return false;
//        }
//        // go to next file, if any
//        cont = directory->SetToNextEntry();
//    }
//    
//    // free the directory
//    n_delete(directory);
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
*/
//bool
//nSpatialSpace::ReadDynamicInfo(TiXmlElement *cellElem)
//{
//    n_assert2(cellElem, "miquelangel.rujula");
//
//    // get the cell
//    int cellId = atoi(cellElem->Attribute("Id"));
//    nSpatialCell *spatialCell = this->SearchCell(cellId);
//    if (!spatialCell)
//    {
//        // the cell doesn't exist
//        return false;
//    }
//
//    // read the spatial element's information
//    TiXmlElement *spatialElementElem = cellElem->FirstChildElement("SpatialElement");
//    while(spatialElementElem)
//    {
//        nSpatialElement *spatialElement = n_new(nSpatialElement);
//        if (!spatialElement->ReadXmlElement(spatialElementElem))
//        {
//            return false;
//        }
//        spatialElement->SetDynamic(true);
//        spatialCell->AddSpatialElement(spatialElement);
//        spatialElementElem = spatialElementElem->NextSiblingElement("SpatialElement");
//    }
//
//    return true;
//}

////------------------------------------------------------------------------------
///**
//    save a space to a file.
//*/
//bool 
//nSpatialSpace::SaveSpace()
//{
//    // save the static information of this space
//    if (!this->SaveStaticInfo())
//    {
//        return false;
//    }
//
//    // save the dynamic information of this space
//    if (!this->SaveDynamicInfo())
//    {
//        return false;
//    }
//
//    return true;
//}

////------------------------------------------------------------------------------
///**
//    save the static information of this space
//*/
//bool 
//nSpatialSpace::SaveStaticInfo()
//{
//    nFileServer2 *fileServer = nFileServer2::Instance();
//
//    if (this->m_spaceFile.IsEmpty())
//    {
//        return false;
//    }
//
//    // check if the space's directory already exists
//    if (!fileServer->DirectoryExists(this->m_dirName.Get()))
//    {
//        // if the space's directory doesn't exist yet, create it
//        fileServer->MakePath(this->m_dirName.Get());
//    }
//
//    nString pathName(this->m_dirName);
//    pathName.Append(this->m_spaceFile);
//    nString realPathName(fileServer->ManglePath(pathName.Get(), false));
//    TiXmlDocument doc(realPathName.Get());
//    
//    // main element
//    TiXmlElement spaceElem("Space");
//
//    // fill the TiXmlElement corresponding to the space with space's info
//    //if (this->FillXmlElement(&spaceElem))
//    if (this->WriteStaticInfo(&spaceElem))
//    {    
//        // insert the space in the document
//        doc.InsertEndChild(spaceElem);
//
//        // save the file
//        return doc.SaveFile();
//    }
//    else
//    {
//        return false;
//    }
//}
//
////------------------------------------------------------------------------------
///**
//*/
//bool 
//nSpatialSpace::WriteStaticInfo(TiXmlElement *spaceElem)
//{
//    n_assert2(spaceElem, "miquelangel.rujula");
//
//    char longCharBuff[50];
//
//    // type of the class. Useful for checking during load
//    nClass *thisClass = this->GetClass();
//    const char *spaceType = thisClass->GetProperName();
//    spaceElem->SetAttribute("Type", spaceType);
//
//    // space's bounding box
//    TiXmlElement bboxElem("BBox");
//    sprintf(longCharBuff, "%f", this->m_bbox.vmin.x);
//    bboxElem.SetAttribute("VminX", longCharBuff);
//    sprintf(longCharBuff, "%f", this->m_bbox.vmin.y);
//    bboxElem.SetAttribute("VminY", longCharBuff);
//    sprintf(longCharBuff, "%f", this->m_bbox.vmin.z);
//    bboxElem.SetAttribute("VminZ", longCharBuff);
//    sprintf(longCharBuff, "%f", this->m_bbox.vmax.x);
//    bboxElem.SetAttribute("VmaxX", longCharBuff);
//    sprintf(longCharBuff, "%f", this->m_bbox.vmax.y);
//    bboxElem.SetAttribute("VmaxY", longCharBuff);
//    sprintf(longCharBuff, "%f", this->m_bbox.vmax.z);
//    bboxElem.SetAttribute("VmaxZ", longCharBuff);
//
//    spaceElem->InsertEndChild(bboxElem);
//    
//    // cells
//    if (!this->AddXmlElementsForCells(spaceElem))
//    {
//        return false;
//    }
//    
//    return true;
//}
//
////------------------------------------------------------------------------------
///**
//*/
//bool 
//nSpatialSpace::AddXmlElementsForCells(TiXmlElement *spaceElem)
//{
//    n_assert2(spaceElem, "miquelangel.rujula");
//
//    for (nArray<nSpatialCell*>::iterator pSpatialCell  = this->m_cellsArray.Begin();
//                                         pSpatialCell != this->m_cellsArray.End();
//                                         pSpatialCell++)
//    {
//        TiXmlElement cellElem("Cell");
//        // fill the TiXmlElement corresponding to the cell with its info
//        //if (!(*spatialCell)->FillXmlElement(&cellElem))
//        if (!(*pSpatialCell)->WriteStaticInfo(&cellElem))
//        {
//            return false;
//        }
//        spaceElem->InsertEndChild(cellElem);
//    }
//
//    return true;
//}
//
////------------------------------------------------------------------------------
///**
//    save the dynamic information of this space
//*/
//bool 
//nSpatialSpace::SaveDynamicInfo()
//{
//    nArray<nString> persistedLayers;
//
//    for (nArray<nSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
//                                         pCell != this->m_cellsArray.End();
//                                         pCell++)
//    {
//        (*pCell)->WriteDynamicInfo(persistedLayers);
//    }
//
//
//    // check if the space's directory already exists
//    if (!nFileServer2::Instance()->DirectoryExists(this->m_dirName.Get()))
//    {
//        // if the space's directory doesn't exist yet, create it
//        nFileServer2::Instance()->MakePath(this->m_dirName.Get());
//    }
//
//    // create a file to contain all the created layer folders names
//    nString pathName(this->m_dirName);
//    pathName.Append("/layers.xml");
//    nString realPathName(nFileServer2::Instance()->ManglePath(pathName.Get(), false));
//    TiXmlDocument doc(realPathName.Get());
//
//    // append the layer folders names to the document
//    for (nArray<nString>::iterator layerName  = persistedLayers.Begin();
//                                   layerName != persistedLayers.End();
//                                   layerName++)
//    {
//        TiXmlElement layerElem("layerDir");
//        layerElem.SetAttribute("Value", layerName->Get());
//        doc.InsertEndChild(layerElem);
//    }
//
//    if (persistedLayers.Size() > 0)
//    {
//        // save the document
//        return doc.SaveFile();
//    }
//
//    return true;
//}

//------------------------------------------------------------------------------
/**
    Searches a cell with a given identifier.
*/
nSpatialCell* 
nSpatialSpace::SearchCell(int cellId)
{
    n_assert2( cellId > -1, "miquelangel.rujula");

    if ( cellId == 0 )
    {
        return NULL;
    }

    for (nArray<nSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                         pCell != this->m_cellsArray.End();
                                         pCell++)
    {
        if ((*pCell)->GetId() == cellId)
        {
            return (*pCell);
        }
    }
    
    return NULL;
}

//------------------------------------------------------------------------------
/**
    add a portal that needs to solve its other side cell pointer
*/
void 
//nSpatialSpace::AddUnsolvedPortal(nSpatialPortal *portal)
nSpatialSpace::AddUnsolvedPortal(nePortal *portal)
{
    this->m_portalsArray.Append(portal);
}

//------------------------------------------------------------------------------
/**
    assign the pointer to the m_otherSide variable (cell the portal is pointing to) 
    of the portals in m_portalsArray, using cell's id contained in each portal
*/
void
nSpatialSpace::SolvePortalsPointers()
{
    nSpatialCell *cell = 0;
    //for (nArray<nSpatialPortal*>::iterator portal  = this->m_portalsArray.Begin();
    for (nArray<nePortal*>::iterator pPortal  = this->m_portalsArray.Begin();
                                     pPortal != this->m_portalsArray.End();
                                     pPortal++)
    {
        // search the cell with the id contained in the portal
        //cell = this->SearchCell((*portal)->GetOtherSideCellId());
        cell = this->SearchCell((*pPortal)->GetOtherSideCellId());
        // set the pointer to the cell
        //(*portal)->SetOtherSideCell(cell);
        (*pPortal)->SetOtherSideCell(cell);

        if (cell)
        {
            // search twin portal
            //nArray<nSpatialPortal*> *portalsArray = cell->GetPortalsArray();
            nArray<nePortal*> *portalsArray = cell->GetPortalsArray();
            //for(nArray<nSpatialPortal*>::iterator otherSidePortal  = portalsArray->Begin();
            for(nArray<nePortal*>::iterator pOtherSidePortal  = portalsArray->Begin();
                                            pOtherSidePortal != portalsArray->End();
                                            pOtherSidePortal++)
            {
                //if ((*otherSidePortal)->GetOtherSideCellId() == (*portal)->GetCell()->GetId())
                if ((*pOtherSidePortal)->GetOtherSideCellId() == (*pPortal)->GetCell()->GetId())
                {
                    //(*portal)->SetTwinPortal((*otherSidePortal));
                    (*pPortal)->SetTwinPortal((*pOtherSidePortal));
                }
            }
        }
    }
}

