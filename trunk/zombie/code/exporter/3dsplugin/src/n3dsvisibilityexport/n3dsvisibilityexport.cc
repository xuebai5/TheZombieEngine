#include "precompiled/pchn3dsmaxexport.h"

#pragma warning( push, 3 )
#include "max.h"
#include "igame/igame.h"
#pragma warning( pop )
//------------------------------------------------------------------------------
#include "n3dsvisibilityexport/n3dsvisibilityexport.h"
#include "n3dsvisibilityexport/n3dswrapportalhelper.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsscenelist.h"
#include "n3dsexporters/n3dstriangle.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsexporters/n3dsSystemcoordinates.h"
//------------------------------------------------------------------------------
#include "tools/nmeshbuilder.h"
#include "kernel/nroot.h"
#include "nspatial/nindoorbuilder.h"
#include "nspatial/ncspatialportal.h"
#include "nscene/ncsceneindexed.h"
#include "util/nstring.h"
//------------------------------------------------------------------------------
#include "nasset/nentityassetbuilder.h"
#include "nspatial/nocclusionbuilder.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialindoorclass.h"


#define OCCLUDER_HELPER_CLASS_ID Class_ID(0x240d76ff, 0x39a6ba3)

//------------------------------------------------------------------------------
/**
*/
n3dsVisibilityExport::n3dsVisibilityExport() :
    indoorBuilder(0),
    countCells(0),
    countPortals(0)
{

}

//------------------------------------------------------------------------------
/**
*/
n3dsVisibilityExport::~n3dsVisibilityExport()
{
    if ( indoorBuilder)
    {
        n_delete( indoorBuilder);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsVisibilityExport::Init(nEntityClass * indoorClass , nEntityClass * /*indoorBrushClass*/, nEntityClass * portalClass)
{   
    NLOG ( visibilityexport , ( 0 , "\n" ) ); // FIXME separate the fist line of the log
    indoorBuilder = n_new( nIndoorBuilder(indoorClass) );
    indoorBuilder->SetPortalsClass( portalClass );
    CreateCells();
    CreatePortals();
    indoorBuilder->SetIndoorShell( nEntityAssetBuilder::GetIndoorFacadeClassName(indoorClass) );
}


//------------------------------------------------------------------------------
/**
*/
void 
n3dsVisibilityExport::CreateCells(void)
{
    const n3dsSceneList& list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::visibility , n3dsObject::visCell ) );

    index.ShowProgressBar( "Visbility cells: ");
    countCells = 0;
    for ( ; index != list.End() ; ++index)
    {
        ++countCells; // The first cell the id is 1
        int cellId;

        IGameNode *node = (*index).GetNode();
        INode*  maxNode = node->GetMaxNode();
        cellId = maxNode->GetHandle();
        
        nMeshBuilder* wrap = CreateMeshBuilder( node );
        //this->indoorBuilder->CreateNewCell( *wrap , countCells );
        this->indoorBuilder->CreateNewCell( *wrap , cellId);
        n_delete(wrap);

        // Asociative the max node with cellId is the future thi is obsolete
        mapMaxToCell.insert( INodeToInt::value_type( maxNode, cellId) );
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsVisibilityExport::CreatePortals(void)
{
    const n3dsSceneList& list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::visibility , n3dsObject::visPortal ) );

    index.ShowProgressBar( "Visibility portals: ");
    countPortals = 0;
    for ( ; index != list.End() ; ++index)
    {
        Portal portal;
        IGameNode *node = (*index).GetNode();
        INode*  maxNode = node->GetMaxNode();
        
        if ( node->GetNodeParent() )
        {
            /// The node's parent is the clip rectangle
            GetQuad( node->GetNodeParent() , portal.points); 
        } else
        {
            N3DSERROR ( visibilityexport , ( 0 , "ERROR: Is the invalid portal \"%s\" , it not has a clip rectangle", node->GetName() ));
        }

        
        n3dsWrapPortalHelper wrapPortalHelper;
        wrapPortalHelper.ExtractFrom(maxNode);

        SetCellIdFromPortalHelper( wrapPortalHelper,  portal );
        SetDeactDistFromPortalHelper( wrapPortalHelper,  portal );
        if ( portal.id1 == portal.id2 )
        {
            // Invalids identifiers
            N3DSERROR ( visibilityexport , ( 0 , "ERROR: Is the invalid portal \"%s\" , connect same cell \"%s\"", node->GetName() , wrapPortalHelper.GetCell1() ? wrapPortalHelper.GetCell1()->GetName() : "OutDoor") );

        } else
        {
            //portal.nohScene  = CreateSceneNode( node, "Portal" , countPortals);
            SetOBBoxFromPortalHelper(  node, wrapPortalHelper,  portal );

            if ( ! indoorBuilder->InsertNewPortal(  portal.points, 
                                                     portal.id1, 
                                                     portal.id2,
                                                     portal.c1ToC2Active,
                                                     portal.c2ToC1Active,
                                                     portal.ePortal1, 
                                                     portal.ePortal2,
                                                     portal.deactDist1,
                                                     portal.deactDist2) 
                )
            {
                N3DSERROR ( visibilityexport , ( 0 , "ERROR: Is the invalid portal \"%s\" , connect invalid cell", node->GetName() ) );
            } else
            {
                bbox3 box; // The original bounding box has only tectangle clip
                box.begin_extend();
                box.extend( portal.points[0] );
                box.extend( portal.points[1] );
                box.extend( portal.points[2] );
                box.extend( portal.points[3] );

                if (portal.ePortal1)
                {
                    portal.ePortal1->GetComponentSafe<ncSceneIndexed>()->SetSubscenePath( "null" );
                    portal.ePortal1->GetComponentSafe<ncSpatial>()->SetOriginalBBox( box );
                    portal.ePortal1->GetEntityObject()->AppendCommentFormat("  Max name \"%s\"", node->GetName() );
                }
                if (portal.ePortal2)
                {
                    portal.ePortal2->GetComponentSafe<ncSceneIndexed>()->SetSubscenePath( "null" );
                    portal.ePortal2->GetComponentSafe<ncSpatial>()->SetOriginalBBox( box );
                    portal.ePortal2->GetEntityObject()->AppendCommentFormat("  Max name \"%s\"", node->GetName() );
                }
                mapIdPortalToRoot.Append( portal);
                countPortals++;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsVisibilityExport::SetCellIdFromPortalHelper(const n3dsWrapPortalHelper& wrap, Portal& portal)
{
    INodeToInt::iterator it1 = this->mapMaxToCell.find( wrap.GetCell1() );
    INodeToInt::iterator it2 = this->mapMaxToCell.find( wrap.GetCell2() );

    portal.id1 = it1 !=  mapMaxToCell.end() ?  (*it1).second : 0;
    portal.id2 = it2 !=  mapMaxToCell.end() ?  (*it2).second : 0;

    portal.c1ToC2Active = wrap.GetC1ToC2Active();
    portal.c2ToC1Active = wrap.GetC2ToC1Active();

}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsVisibilityExport::SetDeactDistFromPortalHelper(const n3dsWrapPortalHelper& wrap, Portal& portal)
{
    portal.deactDist1 = wrap.GetDeactDist1();
    portal.deactDist2 = wrap.GetDeactDist2();
}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder* 
n3dsVisibilityExport::CreateMeshBuilder(IGameNode* node)
{
    n3dsTriangle triangle;
    nMeshBuilder::Vertex vertex[3];
    n3dsSystemCoordinates* systemCoord = n3dsExportServer::Instance()->GetSystemCoordinates();

    IGameMesh* mesh = static_cast<IGameMesh*> ( node->GetIGameObject() );
    mesh->InitializeData();


    const int numFaces = mesh->GetNumberOfFaces();
    const int component = nMeshBuilder::Vertex::Component::COORD;
  
    
    nMeshBuilder* myMesh = n_new( nMeshBuilder( numFaces * 3, numFaces, 1 ));
    n_assert(myMesh);

    for (int idxFace=0; idxFace<numFaces; idxFace++)
    {
          triangle.Set(mesh,idxFace);
          triangle.GetVertex(vertex, component, node);

          systemCoord->MaxToNebulaCulling(vertex[0],vertex[1],vertex[2]);
          myMesh->AddTriangle(vertex[0],vertex[1],vertex[2]);
    }

    myMesh->Cleanup(0);

    return myMesh;
}


//------------------------------------------------------------------------------
/**
        Need the vertex is sorted
          Valid    invalid
          1---0     2---0
          |   |     |   |
          2---3     3---4
*/
void
n3dsVisibilityExport::GetQuad(IGameNode* node, vector3 points[4] )
{
    IGameMesh* mesh = static_cast<IGameMesh*> ( node->GetIGameObject() );
    mesh->InitializeData();
    const int numVerts = mesh->GetNumberOfVerts();
    const int numFaces = mesh->GetNumberOfFaces();
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    if ( 4 != numVerts || 2 != numFaces )
    {
        n_message("%s : Portal clipPolygon need only 4 vertex, has %i \n" , node->GetName() , numVerts );
    } else
    {
        int countIndex[4] = {0,0,0,0} ; // Count the number of triangle use this vert, 1 or 2
        int newIndex[4]   = {0,1,2,3} ;
        for ( int idxFace = 0; idxFace < 2 ;  idxFace++ ) //Two triangles
        {
            FaceEx* face =mesh->GetFace( idxFace );
            for ( int idxVert = 0 ; idxVert < 3 ; idxVert ++ ) // Three vertex for triangle
            {
                int idx = face->vert[ idxVert ];
                if ( idx< 0 || idx >3 )
                {
                    n_message("%s : Portal invalid clipPolygon" , node->GetName()  );
                    return;
                }
                countIndex[idx]++;
            }
        }
        
        // Valids count Index    
        //  1---2     2---1      
        //  |   |     |   |      
        //  2---1     1---2 
        //
        //  1,2,1,2
        //  2,1,2,1
        if ( countIndex[0] == countIndex[1] )
        {
            int tmp = newIndex[1]; //swap vertex
            newIndex[1] = newIndex[2];
            newIndex[2] = tmp;
            countIndex[2] = countIndex[1];
        }

        
        if ( countIndex[0] != countIndex[2] )
        {
            int tmp = newIndex[2];  // swap vertex
            newIndex[2] = newIndex[3];
            newIndex[3] = tmp;
        }


        for ( int idx = 0; idx < 4 ; ++idx)
        {
            
            Point3 vgeom = mesh->GetVertex( idx );
            points[ newIndex[idx] ] = systemCoord->MaxtoNebulaVertex(vgeom);
        }
    }
}


//------------------------------------------------------------------------------
/**
*/
bool 
n3dsVisibilityExport::IsInPortal( vector3 const& pos, int& id)
{
    id = 0;
    int count = 0;
    
    for ( int i=0; i < mapIdPortalToRoot.Size() ; ++i )
    {
        Portal& portal = mapIdPortalToRoot[i];
        if (  portal.container.contains(pos) )
        {
            ++count;
            id = i;
        } 
    }
    

    return (count == 1 );
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsVisibilityExport::IsInCell(vector3 const& pos, int& id)
{
    id = indoorBuilder->GetContainerCellId(pos);
    return id > 0;
}

//------------------------------------------------------------------------------
/**
*/
const n3dsVisibilityExport::Portal& 
n3dsVisibilityExport::GetPortal(int idx)
{
    return mapIdPortalToRoot[idx];
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsVisibilityExport::GetNumPortals()
{
    return countPortals;
}

//------------------------------------------------------------------------------
/**
*/
int 
n3dsVisibilityExport::GetNumCells()
{
    return countCells;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsVisibilityExport::InsertIndoorBrush(nEntityObject *indoorBrush, vector3 &point)
{
    return indoorBuilder->InsertNewObject( indoorBrush , point );
}


//------------------------------------------------------------------------------
/**
*/
void 
n3dsVisibilityExport::SetOBBoxFromPortalHelper(IGameNode* node,const n3dsWrapPortalHelper& wrap, Portal& portal)
{
    obbox3 box;
    n3dsSystemCoordinates* systemCoord= n3dsExportServer::Instance()->GetSystemCoordinates();

    // Calculate The size of box
    vector3 size = wrap.GetBoxSize();
    portal.container.SetSize ( size);
    
    // Calculate the orietation
    GMatrix g = node->GetWorldTM(); // PIVOT POINT TRANSFORM
    transform44 transform = systemCoord->MaxToNebulaTransform(g);
    

    portal.container.SetTransform( transform );

}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsVisibilityExport::Save()
{
    return indoorBuilder->Save();
}

//------------------------------------------------------------------------------
/**
*/
nIndoorBuilder *
n3dsVisibilityExport::GetIndoorBuilder()
{
    return this->indoorBuilder;
}
//------------------------------------------------------------------------------
/**
*/
bool 
n3dsVisibilityExport::ExportOccluders(nEntityClass* entityClass)
{
    nString assetPath = nEntityAssetBuilder::GetAssetPath(entityClass);
    ncSpatialClass* sptClass =  entityClass->GetComponent<ncSpatialClass>();
    ncSpatialIndoorClass* sptIndoorClass =  entityClass->GetComponent<ncSpatialIndoorClass>();

    const n3dsSceneList& list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::visibility , n3dsObject::visOccluder ) );
    index.ShowProgressBar( "Visibility ocluders: ");
    int count =0 ;
    nOcclusionBuilder builder;
    for ( ; index != list.End() ; ++index)
    {
        vector3 points[4];
        IGameNode *node = (*index).GetNode();
        INode*  maxNode = node->GetMaxNode();
        bool doubleSided = this->ExtractDoubleSidedFromOccluder( maxNode);
        
        if ( node->GetNodeParent() )
        {
            /// The node's parent is the clip rectangle
            this->GetQuad( node->GetNodeParent() , points); 
        }else
        {
            N3DSERROR ( visibilityexport , ( 0 , "ERROR: Is the invalid Occluder \"%s\" , it not has a clip rectangle", node->GetName() ));
        }
        builder.AddOccluder( points , doubleSided);
        count++;
    }

    bool hasOcluders = ( 0 <  count ) ;

    if ( hasOcluders )
    {
        builder.EndAndSave( assetPath.Get() );
    }

    if ( sptClass )
    {
        sptClass->SetHasOccluders( hasOcluders );
    }

    if ( sptIndoorClass )
    {
        sptIndoorClass->SetHasOccluders( hasOcluders );
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsVisibilityExport::ExtractDoubleSidedFromOccluder(INode* node)
{
    Animatable* obj = node->GetObjectRef(); // in BaseObject collision method name GetParamBlock
    //Class_ID pp = obj->ClassID();
    //Class_ID p1 = node->GetObjectRef()->ClassID();
    //Class_ID p2 = node->ClassID();

    if ( OCCLUDER_HELPER_CLASS_ID != obj->ClassID() )  
    {
        return false;
    }

    const int numBlock = obj->NumParamBlocks();

    for ( int idxBlock = 0 ; idxBlock < numBlock  ; idxBlock++)
    {
        IParamBlock2* pb2 = obj->GetParamBlock(idxBlock);
        ParamBlockDesc2* pdc= pb2->GetDesc();

        int i;
        for ( i = 0; i< pdc->count ; i++)
        {
            ParamDef pD = pdc->paramdefs[i];
            nString nameParam = pD.int_name;
            nameParam.ToLower();
            if (nameParam == "doublesided" )
            {
                int val = pb2->GetInt(  pD.ID);
                return  0 != val;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------