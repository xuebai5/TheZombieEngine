#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nspatialoctreespacebuilder.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "nspatial/nspatialoctreespacebuilder.h"
#include "tools/nmeshbuilder.h"
#include "nscene/nshapenode.h"
#include "kernel/nkernelserver.h"
#include "entity/nentity.h"
#include "entity/nentityobjectserver.h"
#include "entity/nentityclassserver.h"
#include "nscene/ntransformnode.h"
#include "nscene/ncsceneclass.h"
#include "nscene/ncscene.h"
#include "nscene/ncsceneindexed.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialoctree.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialportal.h"
#include "zombieentity/ncloader.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nSpatialOctreeSpaceBuilder::nSpatialOctreeSpaceBuilder()
{

}

//------------------------------------------------------------------------------
/**
    destructor
*/
nSpatialOctreeSpaceBuilder::~nSpatialOctreeSpaceBuilder()
{
}

//------------------------------------------------------------------------------
/**
    set the octree space to build. Useful if you want to continue building it
*/
void 
nSpatialOctreeSpaceBuilder::SetOctreeSpace(ncSpatialOctree* octreeSpace)
{
    this->m_octreeSpace = octreeSpace->GetEntityObject();
}

//------------------------------------------------------------------------------
/**
    returns the octree space. Call it after construction.
*/
ncSpatialOctree*
nSpatialOctreeSpaceBuilder::GetOctreeSpace()
{
    return this->m_octreeSpace->GetComponentSafe<ncSpatialOctree>();
}

//------------------------------------------------------------------------------
/**
    build an octree space from an indoor space
*/
void 
nSpatialOctreeSpaceBuilder::BuildOctreeSpace(ncSpatialIndoor* indoor)
{
    // create an octree space
    this->m_octreeSpace = nEntityObjectServer::Instance()->NewEntityObject("neoctreespace");
    ncSpatialOctree * octreeSpace = this->m_octreeSpace->GetComponentSafe<ncSpatialOctree>();

    octreeSpace->InitializeSpace();

    // get all the indoor brushes from the indoor
    nArray<nEntityObject*> indoorBrushes;
    indoor->GetIndoorBrushes(indoorBrushes);
    nEntityObject *clone = 0;
    //ncLoader *loader = 0;
    // insert all the indoor brushes into the octree space
    for (nArray<nEntityObject*>::iterator pIbrush  = indoorBrushes.Begin();
                                          pIbrush != indoorBrushes.End();
                                          pIbrush++)
    {
        clone = static_cast<nEntityObject*>((*pIbrush)->Clone());
    #if 0
        loader = clone->GetComponent<ncLoader>();
        n_assert2(loader, "miquelangel.rujula: indoor brush has no loader component!");
        loader->LoadComponents();
    #endif
        octreeSpace->AddEntity(clone);
    }

    // get all the portals from the indoor
    nArray<ncSpatialPortal*> portals;
    indoor->GetPortals(portals);
    // insert all the portals into the octree space
    for (nArray<ncSpatialPortal*>::iterator pPortal  = portals.Begin();
                                            pPortal != portals.End();
                                            pPortal++)
    {
        clone = static_cast<nEntityObject*>((*pPortal)->GetEntityObject()->Clone());
    #if 0
        loader = clone->GetComponent<ncLoader>();
        n_assert2(loader, "miquelangel.rujula: indoor brush has no loader component!");
        loader->LoadComponents();
    #endif
        octreeSpace->AddEntity(clone);
    }

    // add the indoor's facade (if any)
    nEntityObject *facade = indoor->GetFacade();
    if (facade)
    {
        clone = static_cast<nEntityObject*>(facade->Clone());
    #if 0
        loader = clone->GetComponentSafe<ncLoader>();
        loader->LoadComponents();
    #endif
        octreeSpace->AddEntity(clone);
    }

    // set the octree space's bounding box
    bbox3 sceneBox;
    sceneBox.extend(1000.0f, 1000.0f, 1000.0f);
    sceneBox.extend(-1000.0f, -1000.0f, -1000.0f);
    octreeSpace->SetSpaceSize(sceneBox);

    // determine the octree space's cells
    octreeSpace->DetermineCells();
}

//------------------------------------------------------------------------------
/**
    search the objects in the given scene and insert them in the space
*/
int
nSpatialOctreeSpaceBuilder::CatchSpatialInfo(nTransformNode *scene)
{
    this->m_octreeSpace = nEntityObjectServer::Instance()->NewEntityObject("neoctreespace");
    ncSpatialOctree * octreeSpace = this->m_octreeSpace->GetComponentSafe<ncSpatialOctree>();
    octreeSpace->InitializeSpace();
    
    int numObjects = 0;

    // get scene's transformation
    nShapeNode* shape;
    // scenebox will contain all the scene. It is used to set the space's size.
    bbox3 scenebox;
    scenebox.begin_extend();
    
    // this is supposed to be the first object in the scene
    nTransformNode* child = (nTransformNode*) scene->GetHead();

    // check if the child node is a nTransformNode
    nClass *nodeClass = child->GetClass();
    const char *nodeType = nodeClass->GetProperName();
    n_assert2( !strcmp(nodeType, "nTransformNode"), "miquelangel.rujula" );

    matrix44 sceneTransform = scene->GetTransform();
    // set identity to the scene's transformation matrix
    scene->SetScale(vector3(1,1,1));
    scene->SetQuat(quaternion());
    scene->SetPosition(vector3());

    // either create a class for the whole scene, or a subclass for every piece
    nEntityClass *parentClass;
    nString className;
    if (this->GetResourceFile())
    {
        className = this->GetResourceFile();
        className = className.ExtractFileName();
        className.StripExtension();
        className.ToCapital();

        parentClass = static_cast<nEntityClass *> (nKernelServer::Instance()->FindClass("neindoorbrush"));
        nEntityClass *subClass = nEntityClassServer::Instance()->NewEntityClass(parentClass, className.Get());
        ncSceneClass *sceneClass = subClass->GetComponent<ncSceneClass>();
        sceneClass->SetRootNode(scene);
        ncLoaderClass *loaderClass = subClass->GetComponent<ncLoaderClass>();
        loaderClass->SetResourceFile(this->GetResourceFile());
    }
    // TEMP backwards compatibility
    else
    {
        parentClass = static_cast<nEntityClass *> (nKernelServer::Instance()->FindClass("nebrush"));
    }

    while ( child )
    {
        // Here we have a piece of the scene (child)
        bbox3 childbox;
        
        matrix44 acumTransform;
        acumTransform = child->GetTransform() * sceneTransform;
        
        // determine object's bounding box
        shape = (nShapeNode*) child->GetHead();

        bool createSpatialElement = false;

        childbox = child->GetLocalBox();
        vector3 nullVec;
        if (childbox.vmin.isequal(nullVec, 0.000001f) &&
            childbox.vmax.isequal(nullVec, 0.000001f))
        {  
            // Invalid box calculate a box
            childbox.begin_extend();
            while (shape)
            {
                // check if 'shape' is a nShapeNodenode
                nodeClass = shape->GetClass();
                nodeType = nodeClass->GetProperName();
                n_assert2( !strcmp(nodeType, "nShapeNode"), "miquelangel.rujula" );

                createSpatialElement = true;
            /// @todo bbox3 childboxtemp = shape->GetLocalBox();
                nMeshBuilder meshBuilder;
                meshBuilder.Load(nKernelServer::Instance()->GetFileServer(), shape->GetMesh());
                bbox3 childboxtemp = meshBuilder.GetGroupBBoxTestBillboard(shape->GetGroupIndex());
            /// 
                //childboxtemp.transform(acumTransform);
                childbox.extend(childboxtemp);
                shape = (nShapeNode*) shape->GetSucc();
            }
        }
        else
        {
            createSpatialElement = (shape != 0);
            //childbox.transform(acumTransform);
        }
        
        // create a brush entity
        if (createSpatialElement)
        {
            // create the entity class
            // TODO : create a single entity class for the whole geometry (improve ncSceneClass, ncScene)

            // TEMP backwards compatible
            if (!this->GetResourceFile())
            {
                className = "Object";
                className.AppendInt(numObjects);

                nEntityClass *subClass = nEntityClassServer::Instance()->NewEntityClass(parentClass, className.Get());
                ncSceneClass *sceneClass = subClass->GetComponent<ncSceneClass>();
                sceneClass->SetRootNode(child);
                nEntityClassServer::Instance()->SetEntityClassDirty(subClass, false);
            }

            // create the entity
            nEntityObject *newObject = static_cast<nEntityObject*>(nEntityObjectServer::Instance()->NewEntityObject(className.Get()));

            if (this->GetResourceFile())
            {
                ncSceneIndexed *sceneComp = newObject->GetComponent<ncSceneIndexed>();
                sceneComp->SetSubscenePath(scene->GetRelPath(child).Get());
            }

            ncScene *sceneComponent = newObject->GetComponent<ncScene>();
            sceneComponent->Load();

            // set entity's spatial bbox
            ncSpatial *spatialComponent = newObject->GetComponent<ncSpatial>();
            
            scenebox.extend(childbox);
            spatialComponent->SetBBox(childbox);
            ncSpatialClass *spatialCompClass = newObject->GetClassComponent<ncSpatialClass>();
            spatialCompClass->SetOriginalBBox(childbox);

            vector3 newScale;
            quaternion newQuat;
            vector3 newPos;

            // set the new transformation to the child node
            acumTransform.get(newScale, newQuat, newPos);
            //child->SetScale(newScale);
            //child->SetQuat(newQuat);
            //child->SetPosition(newPos);

            ncTransform *transfComp = newObject->GetComponent<ncTransform>();
            //transfComp->SetPosition(childbox.center());
            transfComp->SetPosition(newPos);
            transfComp->SetQuat(newQuat);
            transfComp->SetScale(newScale);

            // set new entity's spatial position
            //spatialComponent->SetPosition(childbox.center());

            //this->m_octreeSpace->AddSpatialElement(newElement);
            octreeSpace->AddEntity(newObject);
            numObjects++;
        }
        
        child = (nTransformNode*) child->GetSucc();
    }

    scenebox.extend(1000.0f, 1000.0f, 1000.0f);
    scenebox.extend(-1000.0f, -1000.0f, -1000.0f);
    octreeSpace->SetSpaceSize(scenebox);
    octreeSpace->DetermineCells();

    return numObjects;
}

//------------------------------------------------------------------------------
/**
    search the objects in the given scene and insert them in the space
*/
//int
//nSpatialOctreeSpaceBuilder::CatchIndoorSpatialInfo(nTransformNode *scene)
//{
//    this->m_octreeSpace = (nSpatialOctreeSpace *)nKernelServer::Instance()->New("nspatialoctreespace", "/usr/space/octreespace");
//    this->m_octreeSpace->InitializeSpace();
//    
//    int numObjects = 0;
//
//    // scenebox will contain all the scene. It is used to set the space's size.
//    bbox3 scenebox;
//    scenebox.begin_extend();
//
//    nShapeNode* shape;
//
//    nSceneNode *parentNode = static_cast<nSceneNode*>(scene);
//    nSceneNode *childNode  = static_cast<nSceneNode*>(scene->GetHead());
//    
//    // search the first nTransformNode
//    nClass *nodeClass = childNode->GetClass();
//    const char *nodeType = nodeClass->GetProperName();
//    nString childNodeTypeString(nodeType);
//    nString transformNameString("nTransformNode");
//    //while (childNodeTypeString != transformNameString)
//    while (childNode)
//    {
//        nodeClass = childNode->GetClass();
//        nodeType = nodeClass->GetProperName();
//        nString childNodeTypeString(nodeType);
//
//        if (childNodeTypeString == transformNameString)
//        {
//            while (childNode)
//            {
//                // Here we have a piece of the scene (child)
//                bbox3 childbox;
//                
//                // determine object's bounding box
//                shape = static_cast<nShapeNode*>(childNode->GetHead());
//
//                bool createSpatialElement = false;
//
//                childbox = childNode->GetLocalBox();
//                vector3 nullVec;
//                if (childbox.vmin.isequal(nullVec, 0.000001f) &&
//                    childbox.vmax.isequal(nullVec, 0.000001f))
//                {  
//                    // Invalid box calculate a box
//                    childbox.begin_extend();
//                    while (shape)
//                    {
//                        // check if 'shape' ia a nShapeNodenode
//                        nodeClass = shape->GetClass();
//                        nodeType = nodeClass->GetProperName();
//                        n_assert2( !strcmp(nodeType, "nShapeNode"), "miquelangel.rujula" );
//
//                        createSpatialElement = true;
//                    /// @todo bbox3 childboxtemp = shape->GetLocalBox();
//                        nMeshBuilder meshBuilder;
//                        meshBuilder.Load(nKernelServer::Instance()->GetFileServer(), shape->GetMesh());
//                        bbox3 childboxtemp = meshBuilder.GetGroupBBoxTestBillboard(shape->GetGroupIndex());
//                    /// 
//                        childbox.extend(childboxtemp);
//                        shape = (nShapeNode*) shape->GetSucc();
//                    }
//                }
//                else
//                {
//                    createSpatialElement = (shape != 0);
//                }
//                
//                if (createSpatialElement)
//                {
//                    scenebox.extend(childbox);
//                    nSpatialElement *newElement = n_new(nSpatialElement);
//                    newElement->SetBBox(childbox);
//
//                    // set new element's position
//                    newElement->SetPosition(childbox.center());
//
//                    newElement->GetRenderContext()->SetRootNode(childNode);
//                    childNode->RenderContextCreated(newElement->GetRenderContext());
//
//                    // UGLY HACK
//                    // if there is some animator node that could be assigned this rendercontext, initialize now
//                    nSceneNode *animsRoot = (nSceneNode *)nKernelServer::Instance()->Lookup("/usr/animators");
//                    if (animsRoot)
//                    {
//                        animsRoot->RenderContextCreated(newElement->GetRenderContext());
//                    }
//
//                    this->m_octreeSpace->AddSpatialElement(newElement);
//                    numObjects++;
//                }
//                
//                childNode = static_cast<nSceneNode*>(childNode->GetSucc());
//            }
//
//            parentNode = static_cast<nSceneNode*>(parentNode->GetSucc());
//
//            if (!parentNode)
//            {   
//                // finish
//                this->m_octreeSpace->SetSpaceSize(scenebox);
//                this->m_octreeSpace->DetermineCells();
//
//                return numObjects;
//            }
//
//            childNode  = static_cast<nSceneNode*>(parentNode->GetHead());
//            if (!childNode)
//            {
//                parentNode = static_cast<nSceneNode*>(parentNode->GetSucc());
//                
//                if (!parentNode)
//                {
//                    // finish
//                    this->m_octreeSpace->SetSpaceSize(scenebox);
//                    this->m_octreeSpace->DetermineCells();
//
//                    return numObjects;
//                }
//
//                childNode  = static_cast<nSceneNode*>(parentNode->GetHead());
//            }
//        }
//        else
//        {
//            parentNode = childNode;
//            childNode = static_cast<nSceneNode*>(childNode->GetHead());
//
//            if (!childNode)
//            {
//                parentNode = static_cast<nSceneNode*>(parentNode->GetSucc());
//                
//                if (!parentNode)
//                {
//                    // finish
//                    this->m_octreeSpace->SetSpaceSize(scenebox);
//                    this->m_octreeSpace->DetermineCells();
//
//                    return numObjects;
//                }
//
//                childNode  = static_cast<nSceneNode*>(parentNode->GetHead());
//            }
//        }
//    }
//
//    n_message("ERROR: wrong file format! Can't build octree space.");
//    
//    return -1;
//}

