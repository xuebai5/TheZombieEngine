#include "precompiled/pchn3dsmaxexport.h"
#pragma warning( push, 3 )
#include "Max.h"
#include "modstack.h"
#include "IGame/IGame.h"
#include "IGame/IGameModifier.h"
#include "inodelayerproperties.h"
#include "ilayerproperties.h"
#pragma warning( pop )
//------------------------------------------------------------------------------
#include "n3dsgraphicsexport/n3dsgraphicsexport.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsscenelist.h"
#include "n3dsexporters/n3dsSystemcoordinates.h"
#include "n3dsexporters/n3dsmeshshape.h"
#include "n3dsexporters/n3dsskinshape.h"
#include "n3dsexporters/n3dsblendshape.h"
#include "n3dsexporters/n3dsblendskinshape.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsvisibilityexport/n3dsvisibilityexport.h"
#include "n3dsmaterial/n3dsmateriallist.h"
//------------------------------------------------------------------------------
#include "nasset/nentityassetbuilder.h"
#include "kernel/ndependencyserver.h"
#include "nspatial/nindoorbuilder.h"
//------------------------------------------------------------------------------
#include "entity/nentityobjectserver.h"
#include "entity/nentityclass.h"
#include "zombieentity/ncloaderclass.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialportal.h"
#include "nscene/ncsceneindexed.h"
#include "nscene/ncsceneclass.h"
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/**
*/
n3dsGraphicsExport::n3dsGraphicsExport() :
    mapCellIdToNohContainer(0)
    //rootAnimators(0)
{
    rootNode = static_cast<nSceneNode*>(  nKernelServer::ks->New("ntransformnode",  "/lib/scene/") );
}

//------------------------------------------------------------------------------
/**
*/
n3dsGraphicsExport::~n3dsGraphicsExport()
{
    if (rootNode) rootNode->Release();
    if (mapCellIdToNohContainer) n_delete(mapCellIdToNohContainer);
}

//------------------------------------------------------------------------------
/**
    Create the scene for class
*/
void 
n3dsGraphicsExport::ExportBrushClass( nEntityClass* entityClass, nArray<n3dsAssetInfo> assetPathArray )
{
    //nEntityClass * entityClass;
    n_assert( assetPathArray.Size() > 0 );
    const n3dsLOD& lod = n3dsExportServer::Instance()->GetSettings().lod;
    n3dsExportServer::Instance()->GetMtlList()->InitializedData();

    for ( int idx = 0; idx < assetPathArray.Size() ; ++idx )
    {   
        // create folder for meshes
        nEntityAssetBuilder::MakeMeshPathForAsset( assetPathArray[idx].assetPath );

        assetPathArray[idx].noh = nEntityAssetBuilder::GetSceneNOHForAsset( assetPathArray[idx].assetPath );
        assetPathArray[idx].node = static_cast<nSceneNode*>( nKernelServer::ks->New("ntransformnode" , assetPathArray[idx].noh.Get() ) );
        /// no delete the local scene
    }
    
    n3dsSceneList list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::graphics ) );

    index.ShowProgressBar( "Export graphics: ");
    
    bbox3 box; // The  BBOX for entity class
    box.begin_extend();
    bool isShadowEmitter = false;
    for ( ; index != list.End() ; ++index)
    {
        IGameNode* node = (*index).GetNode();
        if ( !(node->IsNodeHidden() ) )
        {
            n3dsAssetInfo& currentAsset = assetPathArray[ lod.GetLODof( node ) ];
            node->GetIGameObject()->InitializeData();
            // push the asset's scenenode
            nKernelServer::ks->PushCwd( currentAsset.node );
            n3dsMeshShape *myShape= n_new(n3dsMeshShape(node , currentAsset.assetPath ));
            myShape->CreateMesh();
            box.extend( myShape->GetBBox() );
            isShadowEmitter |= myShape->IsShadowEmitter();
            n_delete(myShape);
            nKernelServer::ks->PopCwd();
        }
    }

    //export plugs
    n3dsSceneList::iterator indexPlugs( list.Begin( n3dsObject::plugs ) );
    indexPlugs.ShowProgressBar( "Export plugs: ");

    ncSceneClass* sceneClass = entityClass->GetComponentSafe<ncSceneClass>();
    if(( sceneClass ) && ( list.Count(n3dsObject::plugs) > 0) )
    {
        sceneClass->BeginPlugs(list.Count(n3dsObject::plugs));

        for ( int i=0; indexPlugs != list.End() ; ++indexPlugs, i++)
        {
            IGameNode* node = (*indexPlugs).GetNode();

            GMatrix worldGMatrix = node->GetWorldTM(0);
            matrix44d worldMatrix = n3dsExportServer::Instance()->GetSystemCoordinates()->MaxToNebulaMatrix(worldGMatrix);

            quaternion quat = worldMatrix.get_quaternion();
            vector3 pos = worldMatrix.pos_component();

            char* plugName = node->GetName();

            sceneClass->AddPlug( i, plugName, pos, quat);
        }

        sceneClass->EndPlugs();
    }

    for ( int idx = 0; idx < assetPathArray.Size() ; ++idx )
    {
        // Set the same bounding box for spatial element
        //ncSpatialClass* spatComp = entityClassArray[idx]->GetComponent<ncSpatialClass>();
        //spatComp->SetOriginalBBox( box );

        // Save the asset
        SaveSceneForAsset( assetPathArray[ idx ].assetPath );
    }

    ncSpatialClass* spatComp = entityClass->GetComponent<ncSpatialClass>();
    spatComp->SetOriginalBBox( box );
    spatComp->SetShadowCaster( isShadowEmitter );
    n3dsExportServer::Instance()->GetMtlList()->SaveResourceFile();
}


//------------------------------------------------------------------------------
/**
    Create the scene for class
*/
void 
n3dsGraphicsExport::InitExportIndoor(nEntityClass* /*indoorClass*/, nEntityClass* indoorBrushClass, nEntityClass* portalClass, nEntityClass* indoorFacadeClass)
{
    //n_assert2(indoorClass->IsA("neindoorclass"));
    n_assert(indoorBrushClass->IsA("neindoorbrushclass"));
    n_assert(portalClass->IsA("neportalclass"));
    n_assert(indoorFacadeClass->IsA("nemirageclass"));

    //indoorBrushClass()
    ncSpatialClass* spatComp;


    CreateSceneNode( indoorBrushClass );
    CreateSceneNode( indoorFacadeClass );
    nSceneNode* scenePortals = CreateSceneNode( portalClass );

    nKernelServer::ks->PushCwd(scenePortals);
    n_verify( nKernelServer::ks->New("ntransformnode" , "null" ) );
    nKernelServer::ks->PopCwd();

    bbox3 box;
    box.begin_extend();
    spatComp = indoorFacadeClass->GetComponent<ncSpatialClass>();
    spatComp->SetOriginalBBox( box );

}

//------------------------------------------------------------------------------
/**
    Create the scene for class
*/
void 
n3dsGraphicsExport::ExportIndoor(nEntityClass* /*indoorClass*/, nEntityClass* indoorBrushClass, nEntityClass* portalClass, nEntityClass* indoorFacadeClass, nArray<n3dsAssetInfo> assetPathArray)
{
    //n_assert(indoorClass->IsA("neindoorclass"));
    n_assert(indoorBrushClass->IsA("neindoorbrushclass"));
    n_assert(portalClass->IsA("neportalclass"));
    n_assert(indoorFacadeClass->IsA("nemirageclass"));

    n3dsExportServer::Instance()->GetMtlList()->InitializedData();
    n3dsVisibilityExport* visibility = n3dsExportServer::Instance()->GetVisibility();
    
    // create directory for meshes
    nEntityAssetBuilder::MakeMeshPath( indoorBrushClass );
    nEntityAssetBuilder::MakeMeshPath( portalClass );
    nEntityAssetBuilder::MakeMeshPath( indoorFacadeClass );

    for ( int idx = 0; idx < assetPathArray.Size() ; ++idx )
    {   
        // create folder for meshes
        nEntityAssetBuilder::MakeMeshPathForAsset( assetPathArray[idx].assetPath );
        assetPathArray[idx].node = CreateSceneNodeForAsset( assetPathArray[idx].assetPath );
    }

    // create a map table , with initial size equal to cell number
    mapCellIdToNohContainer = n_new(nMapTable<nSceneNode*>(visibility->GetNumCells() ) );

    n3dsSceneList list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::graphics ) );
    //mapCellIdToNohContainer
    

    index.ShowProgressBar( "Export graphics: ");
    
    for ( ; index != list.End() ; ++index)
    {
        IGameNode* node = (*index).GetNode();
        if ( !(node->IsNodeHidden() ) )
        {
            int id=0;
            node->GetIGameObject()->InitializeData();
            INodeLayerProperties* thisNodeProps = static_cast<INodeLayerProperties*>( node->GetMaxNode()->GetInterface(NODELAYERPROPERTIES_INTERFACE));
            nString layerName = thisNodeProps->getLayer()->getName();
            layerName.ToLower();
            layerName = layerName.GetToken('_');

            if ( layerName == "facade" )
            {
                ExportIndoorFacade( node, indoorFacadeClass , assetPathArray);
            } else
            {
                GMatrix g=node->GetWorldTM(); // PIVOT POINT TRANSFORM
                matrix44d worldMatrix = n3dsExportServer::Instance()->GetSystemCoordinates()->MaxToNebulaMatrix(g);

                if ( visibility->IsInPortal( worldMatrix.pos_component() , id ) )
                {
                    ExportIndoorPortal(node, id, portalClass);

                } else if ( visibility->IsInCell( worldMatrix.pos_component() , id ) )
                {
                    ExportIndoorBrush(node , id , indoorBrushClass);
                } else
                {
                    // Put the object in wrap for view
                    ExportIndoorFacade( node, indoorFacadeClass, assetPathArray );
                    N3DSERROR ( visibilityexport , ( 0 , "ERROR: the object is not in cell %s\n", node->GetName() ) );
                }
            }
        }
    }

    SaveScene( indoorBrushClass );
    SaveScene( portalClass );
    ncSpatialClass * indoorFacadeSpatialComp = indoorFacadeClass->GetComponentSafe<ncSpatialClass>();
    if ( indoorFacadeSpatialComp->GetOriginalBBox().IsValid() ) // has a facade
    {
        SaveScene( indoorFacadeClass );
        for ( int idx = 0; idx < assetPathArray.Size() ; ++idx )
        {
            // Save the asset
            SaveSceneForAsset( assetPathArray[idx].assetPath );
        }
    }
    n3dsExportServer::Instance()->GetMtlList()->SaveResourceFile();
}


//------------------------------------------------------------------------------
/**
*/
void 
n3dsGraphicsExport::ExportIndoorBrush(IGameNode* igame, int cellId, nEntityClass* indoorBrushClass)
{   
    // Create the instance
    nEntityObject * indoorBrush = nEntityObjectServer::Instance()->NewEntityObject( indoorBrushClass->nClass::GetName() );
    n_assert(indoorBrush);

    // Search the container node for scene    
    nString scenePath;
    nSceneNode** ptr = (*mapCellIdToNohContainer)[cellId];
    nSceneNode* container;
    if ( 0 == ptr) 
    {
        // Create the cell Container
        nString name;
        name.Format("Cell_%.8X", cellId);
        nKernelServer::ks->PushCwd( nEntityAssetBuilder::GetSceneRoot(indoorBrushClass) );
        container = static_cast<nSceneNode*>(nKernelServer::ks->New( "nscenenode" , name.Get() ) );
        nKernelServer::ks->PopCwd();
        mapCellIdToNohContainer->Add( cellId , &container);

        container->AppendCommentFormat("Max name: \"%s\"" , GetCOREInterface()->GetINodeByHandle(cellId)->GetName());
    } else
    {
        container= *ptr;
    }
    scenePath = container->GetName();
    scenePath += "/";


    // Create the scene node and export mesh
    nKernelServer::ks->PushCwd( container);
    n3dsMeshShape *myShape= n_new(n3dsMeshShape( igame , indoorBrushClass));
    
    if (! myShape->CreateMesh() )
    {
        n_delete(myShape);
        nKernelServer::ks->PopCwd();
    } else
    {
        //  Set to brush
        scenePath += myShape->GetSceneName();
        indoorBrush->GetComponentSafe<ncSceneIndexed>()->SetSubscenePath( scenePath.Get() );
        indoorBrush->GetComponentSafe<ncSpatial>()->SetOriginalBBox( myShape->GetBBox() );

        //delete the shape builder
        n_delete(myShape);
        nKernelServer::ks->PopCwd();
     
        // Insert to the world
        GMatrix g=igame->GetWorldTM(); // PIVOT POINT TRANSFORM
        matrix44d worldMatrix = n3dsExportServer::Instance()->GetSystemCoordinates()->MaxToNebulaMatrix(g);
        vector3 point = worldMatrix.pos_component() ;

        n3dsExportServer::Instance()->GetVisibility()->InsertIndoorBrush( indoorBrush , point);
    }

}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsGraphicsExport::ExportIndoorPortal(IGameNode* igame, int portalId, nEntityClass* portalClass )
{
    const n3dsVisibilityExport::Portal& portal= n3dsExportServer::Instance()->GetVisibility()->GetPortal( portalId);
    nString sceneRelativePath;
    bbox3 localBBox;
    
    if ( portal.ePortal1 )
    {
        sceneRelativePath = portal.ePortal1->GetComponentSafe<ncSceneIndexed>()->GetSubscenePath();
    } else if ( portal.ePortal2)
    {
        sceneRelativePath = portal.ePortal2->GetComponentSafe<ncSceneIndexed>()->GetSubscenePath();
    }

    /// Comprobe if portal has container
    if ( sceneRelativePath == "null")
    {
        // create it
        sceneRelativePath.Format("Portal_%.8X", portalId);
        nKernelServer::ks->PushCwd( nEntityAssetBuilder::GetSceneRoot( portalClass) );
        nKernelServer::ks->New( "nscenenode" , sceneRelativePath.Get() );
        nKernelServer::ks->PopCwd();
        if ( portal.ePortal1 )
        {
            portal.ePortal1->GetComponentSafe<ncSceneIndexed>()->SetSubscenePath( sceneRelativePath.Get() );
        } 

        if ( portal.ePortal2)
        {
            portal.ePortal2->GetComponentSafe<ncSceneIndexed>()->SetSubscenePath( sceneRelativePath.Get() );
        }
    }

    // Get the container
    nKernelServer::ks->PushCwd( nEntityAssetBuilder::GetSceneRoot( portalClass) );
    nSceneNode* container = static_cast<nSceneNode*>(nKernelServer::ks->Lookup( sceneRelativePath.Get() )) ;
    n_assert( container );
    nKernelServer::ks->PopCwd();

    // Create shape
    nKernelServer::ks->PushCwd( container );
    n3dsMeshShape *myShape= n_new(n3dsMeshShape( igame , portalClass));
    myShape->CreateMesh();
    localBBox = myShape->GetBBox(); // the bounding box
    n_delete(myShape);
    nKernelServer::ks->PopCwd();


    // update the spatial bounding box
    if ( portal.ePortal1 )
    {
        bbox3 totalBox =  portal.ePortal1->GetOriginalBBox(); // return pointer to bbox
        totalBox.extend(localBBox);
        portal.ePortal1->SetOriginalBBox( totalBox);
    } 

    // update the bounding box
    if ( portal.ePortal2 )
    {
        bbox3 totalBox =  portal.ePortal2->GetOriginalBBox(); // return pointer to bbox
        totalBox.extend(localBBox);
        portal.ePortal2->SetOriginalBBox( totalBox);
    } 
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsGraphicsExport::ExportIndoorFacade(IGameNode* node, nEntityClass* indoorFacadeClass , nArray<n3dsAssetInfo> assetPathArray )
{
    const n3dsLOD& lod = n3dsExportServer::Instance()->GetSettings().lod;
    ncSpatialClass* spatComp = indoorFacadeClass->GetComponent<ncSpatialClass>();
    bbox3 box = spatComp->GetOriginalBBox();
    n3dsAssetInfo& currentLod = assetPathArray [ lod.GetLODof( node) ];
    nKernelServer::Instance()->PushCwd( currentLod.node  );

    node->GetIGameObject()->InitializeData();
    n3dsMeshShape *myShape= n_new(n3dsMeshShape(node , currentLod.assetPath ));
    myShape->CreateMesh();
    box.extend( myShape->GetBBox() );
    n_delete(myShape);
    spatComp->SetOriginalBBox( box );

    nKernelServer::Instance()->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsGraphicsExport::ExportCharacterClass( nEntityClass* entityClass, nArray<n3dsAssetInfo> lodAssetPathArray, nArray<n3dsAssetInfo> ragAssetPathArray )
{
    n_assert( lodAssetPathArray.Size() > 0 );
    n_assert( ragAssetPathArray.Size() > 0 );

    const n3dsLOD& lod = n3dsExportServer::Instance()->GetSettings().lod;
    nArray<nMeshBuilder*> shadowMesh; // one shadow mesh for each lod
    nArray<nMeshBuilder*> ragShadowMesh; // one shadow mesh for each lod

    n3dsExportServer::Instance()->GetMtlList()->InitializedData();

    for ( int idx = 0; idx < lodAssetPathArray.Size() ; ++idx )
    {   
        // create folder for meshes
        nEntityAssetBuilder::MakeMeshPathForAsset( lodAssetPathArray[idx].assetPath );

        lodAssetPathArray[idx].noh = nEntityAssetBuilder::GetSceneNOHForAsset( lodAssetPathArray[idx].assetPath );
        lodAssetPathArray[idx].node = static_cast<nSceneNode*>( nKernelServer::ks->New("ntransformnode" , lodAssetPathArray[idx].noh.Get() ) );
        shadowMesh.At(idx)  = n_new( nMeshBuilder);
    }

    // create folder for ragdpoll meshes
    for ( int idx = 0; idx < ragAssetPathArray.Size() ; ++idx )
    {   
        nEntityAssetBuilder::MakeMeshPathForAsset( ragAssetPathArray[idx].assetPath );
        ragAssetPathArray[idx].noh = nEntityAssetBuilder::GetSceneNOHForAsset( ragAssetPathArray[idx].assetPath );
        ragAssetPathArray[idx].node = static_cast<nSceneNode*>( nKernelServer::ks->New("ntransformnode" , ragAssetPathArray[idx].noh.Get() ) );
        ragShadowMesh.At(idx)  = n_new( nMeshBuilder);
    }


    //export data
    bbox3 bbox;
    bbox.begin_extend();
    bool isShadowEmitter(false);

    n3dsSceneList list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::graphics ) );

    index.ShowProgressBar( "Export skinned meshes: ");

    for ( ; index != list.End() ; ++index)
    {
        IGameNode* node = (*index).GetNode();
        IGameMesh* mesh = static_cast< IGameMesh* > ( node->GetIGameObject() );
        if (  (!(node->IsNodeHidden() ) && (mesh->IsObjectSkinned())) )
        {
            int currentLod = lod.GetLODof( node );
            n3dsAssetInfo& currentAsset = lodAssetPathArray[ currentLod ];
            n3dsAssetInfo& currentRagAsset = ragAssetPathArray[ currentLod];
            node->GetIGameObject()->InitializeData();
            //nKernelServer::ks->PushCwd( currentAsset.node );

            if( this->IsBlendedNode(node))
            {
                n3dsBlendSkinShape* skinblendShape = n_new( n3dsBlendSkinShape( node , currentAsset, currentRagAsset, entityClass ));
                skinblendShape->CreateAnimation();
                skinblendShape->CreateMesh();
                //shadowMesh[currentLod]->Append( *skinblendShape->GetShadowMesh() );
                bbox.extend( skinblendShape->GetBBox() );
                //isShadowEmitter |= skinblendShape->IsShadowEmitter();
                n_delete(skinblendShape);
            }
            else
            {
                n3dsSkinShape* skinShape = n_new( n3dsSkinShape( node , currentAsset, currentRagAsset, entityClass ));
                if( skinShape->CreateMesh() )
                {
                    shadowMesh[currentLod]->Append( *skinShape->GetShadowMesh() );
                    if( skinShape->GetRagShadowMesh())
                    {
                        ragShadowMesh[currentLod]->Append( *skinShape->GetRagShadowMesh() );
                    }
                }
                bbox.extend( skinShape->GetBBox() );
                isShadowEmitter |= skinShape->IsShadowEmitter();
                n_delete(skinShape);
            }
            //nKernelServer::ks->PopCwd();
        }
    }
    // end export data

    // Export shadows
    for ( int idx = 0; idx < lodAssetPathArray.Size() ; ++idx )
    {
        n3dsAssetInfo& currentAsset = lodAssetPathArray[ idx ];
        nKernelServer::ks->PushCwd( currentAsset.node );
        n3dsSkinShape::SaveShadowMesh( shadowMesh.At(idx) , currentAsset.assetPath );
        nKernelServer::ks->PopCwd();
        n_delete( shadowMesh.At(idx) );
        shadowMesh.At(idx) = 0;
    }
    // Export shadows
    for ( int idx = 0; idx < ragAssetPathArray.Size() ; ++idx )
    {
        n3dsAssetInfo& currentRagAsset = ragAssetPathArray[ idx ];
        if(ragShadowMesh.At(idx))
        {
            nKernelServer::ks->PushCwd( currentRagAsset.node );
            n3dsSkinShape::SaveShadowMesh( ragShadowMesh.At(idx) , currentRagAsset.assetPath );
            nKernelServer::ks->PopCwd();
        }
        n_delete( ragShadowMesh.At(idx) );
        ragShadowMesh.At(idx) = 0;
    }
 
    //assets for lod
    for ( int idx = 0; idx < lodAssetPathArray.Size() ; ++idx )
    {
        // Save the asset
        this->SaveSceneForAsset( lodAssetPathArray[ idx ].assetPath );
    }
    //assets for ragdolls
    for ( int idx = 0; idx < ragAssetPathArray.Size() ; ++idx )
    {
        // Save the asset
        this->SaveSceneForAsset( ragAssetPathArray[ idx ].assetPath );
    }

    ncSpatialClass* spatComp = entityClass->GetComponent<ncSpatialClass>();
    if( !bbox.IsValid() )
    {
        bbox.set(vector3(0,0,0), vector3(1,1,1));
        N3DSERROR( animexport , ( 0 , "ERROR: Have you unhidden the mesh? I haven't found it. Reexport. (n3dsGraphicsExport::ExportCharacterClass)"));
    }

    spatComp->SetOriginalBBox( bbox );
    spatComp->SetShadowCaster( isShadowEmitter );

    n3dsExportServer::Instance()->GetMtlList()->SaveResourceFile();
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsGraphicsExport::IsBlendedNode( IGameNode* igame )
{
    return n3dsBlendShape::IsBlendedNode( igame );
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsGraphicsExport::SaveScene( nEntityClass* entityClass) const
{
    SaveSceneForAsset ( nEntityAssetBuilder::GetAssetPath( entityClass ) );
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsGraphicsExport::SaveSceneForAsset( const nString& assetPath ) const
{
    if ( n3dsExportServer::Instance()->GetSettings().gameLibrary )
    {
        SortScene( nEntityAssetBuilder::GetSceneRootForAsset( assetPath ) );
    }
    nEntityAssetBuilder::SaveSceneRootForAsset( assetPath  );
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode* 
n3dsGraphicsExport::CreateSceneNode(nEntityClass* entityClass) 
{
    return CreateSceneNodeForAsset( nEntityAssetBuilder::GetAssetPath( entityClass) );
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode* 
n3dsGraphicsExport::CreateSceneNodeForAsset( const nString& assetPath ) 
{
    nString sceneNOH = nEntityAssetBuilder::GetSceneNOHForAsset( assetPath );
    nSceneNode* node = static_cast<nSceneNode*>( nKernelServer::ks->New("ntransformnode" , sceneNOH.Get() ) ) ;
    n_assert(node);
    return node;
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsGraphicsExport::SortScene(nRoot * node) const
{
    node->Sort();
    nRoot *c;

    // count child objects
    for ( c= node->GetHead(); c; c=c->GetSucc() )
    {
        SortScene( c );
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
