#include "precompiled/pchngrass_conjurer.h"
#include "nvegetation/nvegetationbuilder.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "ngeomipmap/ncterraingmmcell.h"
#include "nspatial/ncspatialquadtree.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nvegetation/ncterrainvegetationcell.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "ngrassgrowth/ngrowthmaterial.h"
#include "util/nrandomlogic.h"
#include "ngrassgrowth/ngrowthseeds.h"
#include "ngeomipmap/nfloatmap.h"
#include "nasset/nentityassetbuilder.h"
#include "tools/nmeshbuilder.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "resource/nresourceserver.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nobjecteditorstate.h"

const float builderSubdivisionSize = 0.1f;


//------------------------------------------------------------------------------
/**
*/
nVegetationBuilder::nVegetationBuilder() :
    outdoorClass(0),
    outdoor(0),
    growthSeed(0)
{
}

//------------------------------------------------------------------------------
/**
*/
nVegetationBuilder::~nVegetationBuilder()
{
    //this->cells.SetSize;
}

//------------------------------------------------------------------------------
/**
*/
bool
nVegetationBuilder::Begin(nEntityClass* outdoorClass)
{
    this->outdoor = nSpatialServer::Instance()->GetOutdoorEntity();

    if  ( ( ! outdoor ) || outdoor->GetEntityClass() != outdoorClass )
    {
        // need a instance
        return false;
    }
    this->outdoorClass = outdoorClass;
    this->makeCellList();
    //this->LoadResources();

    /// make the directories for conjurer data
    nFileServer2::Instance()->MakePath( nEntityAssetBuilder::GetTerrainPath( outdoorClass , true ) );

    return true;

}

//------------------------------------------------------------------------------
/**
*/
bool
nVegetationBuilder::End(bool /*save*/)
{

    /// Load all resource of cells
    /*
    if ( save )
    {
        for ( int idx = 0 ; idx < this->GetCellsList().Size()  ; ++idx)
        {
            ncTerrainVegetationCell* cell = this->GetCellsList()[idx]->GetComponentSafe<ncTerrainVegetationCell>();
            cell->SaveEditionResources();
        }
    }
    */

    this->UnloadResources();
    this->cellList.Clear();
    //this->outdoorClass->GetComponentSafe<ncTerrainVegetationClass>()->SaveTerrainVegetationResources();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
const nArray<nEntityObject*> &
nVegetationBuilder::GetCellsList() const
{
    return this->cellList;
}



//------------------------------------------------------------------------------
/**
    @brief fill the cell arrray 
*/
void
nVegetationBuilder::makeCellList()
{
    n_assert(this->outdoor);
    nArray<ncSpatialQuadtreeCell*> &list= * (this->outdoor->GetComponentSafe<ncSpatialQuadtree>()->GetLeafCells()) ;

    for ( int idx = 0; idx < list.Size() ; ++idx )
    {
        ncSpatialQuadtreeCell* cell = list[idx];
        this->cellList.Set( idx ,  cell->GetEntityObject() );
    }
}

//------------------------------------------------------------------------------
/**
    load edition resources
*/
bool
nVegetationBuilder::LoadResources()
{
    /// Load all resource of cells
    for ( int idx = 0 ; idx < this->GetCellsList().Size()  ; ++idx)
    {
        ncTerrainVegetationCell* cell = this->GetCellsList()[idx]->GetComponentSafe<ncTerrainVegetationCell>();
        if  ( ! cell->LoadEditionResources() )  
        { 
            return false;
        } 
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload edition resources only if it not change
*/
void
nVegetationBuilder::UnloadResources()
{
        /// Load all resource of cells
    for ( int idx = 0 ; idx < this->GetCellsList().Size()  ; ++idx)
    {
        ncTerrainVegetationCell* cell = this->GetCellsList()[idx]->GetComponent<ncTerrainVegetationCell>();
        if ( cell )
        {
            nByteMap* byteMap =cell->GetGrowthMap();
            if ( byteMap && (! byteMap->IsDirty() ) )
            {
                cell->UnloadEditionResources();
            }
        }
    }
}


//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationBuilder::UpdateHeight(nEntityClass* outdoorClass)
{
    this->outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    if  ( ( ! outdoor ) || outdoor->GetEntityClass() != outdoorClass )
    {
        // need a instance
        return false;
    }
    ncTerrainVegetationClass* vegClass  = outdoorClass->GetComponent<ncTerrainVegetationClass>();
    ncTerrainGMMClass*        gmmClass  = outdoorClass->GetComponentSafe<ncTerrainGMMClass>();
    nFloatMap*                heightMap = gmmClass->GetHeightMap();
    
    int numBlocks  = outdoorClass->GetComponentSafe< ncTerrainGMMClass>()->GetNumBlocks();

    for ( int z1 = 0 ; z1 < numBlocks ; ++z1)
    {
        for ( int x1 = 0; x1 < numBlocks ; ++ x1 )
        {
            nGrowthSeeds *growthSeed = vegClass->GetGrowthSeeds( x1  , z1 );
            if ( growthSeed )
            {
                const nGrowthSeeds::GroupListSeed& oldListSeed = growthSeed->GetSeeds();
                nGrowthSeeds::BuildGroupListSeed  newListSeed( oldListSeed.Size() );

                for ( int idxGroup = 0; idxGroup < oldListSeed.Size() ; ++idxGroup )
                {
                    const int listSize = oldListSeed[idxGroup].Size();
                    for ( int idxList = 0; idxList < listSize ; ++idxList)
                    {
                        nGrowthSeeds::Seed seed = oldListSeed[idxGroup][idxList];
                        vector3 pos  = growthSeed->GetPosition(seed);
                        vector3 normal;
                        heightMap->GetHeightNormal(  pos.x,  pos.z,  pos.y , normal);
                        normal.norm();
                        growthSeed->SetPosition(seed , pos );
                        growthSeed->SetNormal(seed ,  normal);
                        newListSeed[idxGroup].Append(seed);
                    }
                }
                growthSeed->CreateList(newListSeed);
                growthSeed->Save();
            }
        }
    }

    vegClass->UnloadSceneResources();
    vegClass->LoadSceneResources();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationBuilder::UpdateMeshes(nEntityClass *outdoorClass)
{
    /// For update meshes not change size
    return CreateMeshes(outdoorClass , true);
}

//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationBuilder::CreateMeshes(nEntityClass *outdoorClass , bool checkSameSize )
{
    ncTerrainVegetationClass* vegClass = outdoorClass->GetComponent<ncTerrainVegetationClass>();
    nArray<nString> meshNameList;
    nString meshPath = nEntityAssetBuilder::GetMeshesPath( outdoorClass );
    nFileServer2::Instance()->MakePath( meshPath );

    vegClass->UnloadSceneResources();
    vegClass->UnloadResources(false);

    const int numMat = vegClass->GetNumberOfGrowthMaterials();
    for ( int idx = 0 ; idx < numMat ; ++idx )
    {
        nGrowthMaterial*  material= vegClass->GetGrowthMaterialById(idx);
        const int numBrush = material->GetNumberOfGrowthBrushes();
        for ( int idxBrush = 0 ; idxBrush < numBrush ; ++idxBrush )
        {
            nGrowthBrush*  brush = material->GetGrowthBrush( idxBrush );
            brush->SetMeshId( meshNameList.Size() );


            nString currentAssetPath = nEntityAssetBuilder::GetDefaultAssetPath( brush->GetAssetName() );
            nString currentMeshPath = nEntityAssetBuilder::GetMaterialPathForAsset( currentAssetPath );

            nArray<nString> fileList = nFileServer2::Instance()->ListFiles( currentMeshPath );

            /// search mesh file
            int idxFileList = 0;
            for ( idxFileList = 0 ; 
                idxFileList < fileList.Size() &&  ! fileList[idxFileList].CheckExtension("nvx2") ;
                ++idxFileList )
            {
                ;
            }

            if ( idxFileList < fileList.Size() )
            {
                float terrainColorFactor = brush->GetModulateByTerrain() ? 1.0f : 0.0f;
                nMeshBuilder meshBuilder(1024,1024);
                meshBuilder.Load( nFileServer2::Instance() , fileList[idxFileList].Get() );
                nString newMeshName( meshPath  + "mesh_" + meshNameList.Size() + ".nvx2" );
                // Make transformation 
                meshBuilder.SetTerrainColorFactor( terrainColorFactor );
                meshBuilder.Save( nFileServer2::Instance(), newMeshName.Get() );
                meshNameList.Append(  newMeshName);
            }
        }
    }

    bool result = false;
    if ( ! checkSameSize  || meshNameList.Size() == vegClass->GetGrowthMeshLibrarySize() ) 
    {
        vegClass->SetGrowthMeshLibrary( meshNameList);
        outdoorClass->SetObjectDirty();
        result = true;
    } 
    vegClass->LoadResources();
    vegClass->LoadSceneResources();
    return result;
}


//------------------------------------------------------------------------------
/**
    First step Collect all messhe from material growth and put it the vegetation class
    Second step calculate the growthSeed by cell
*/
bool 
nVegetationBuilder::Build(Mode mode)
{
    if ( ! this->outdoorClass )
    {
        return false;
    }
    int oldRandomSeed = n_getseed( );  // get the seed for restore it when end the build
    n_setseed( 1 ); //Always same generate
    
    //Fisrst step
    ncTerrainVegetationClass* vegClass = this->outdoorClass->GetComponent<ncTerrainVegetationClass>();
    const int numMat = vegClass->GetNumberOfGrowthMaterials();
    for ( int idx = 0 ; idx < numMat ; ++idx )
    {
        nGrowthMaterial*  material= vegClass->GetGrowthMaterialById(idx);
        material->BeginBuild();
    }
    this->CreateMeshes(this->outdoorClass, false);

    //Second step
    ncTerrainGMMClass*        gmmClass = this->outdoorClass->GetComponentSafe<ncTerrainGMMClass>();
    int size =  static_cast<int>( gmmClass->GetBlockSideSizeScaled() / builderSubdivisionSize );


    this->growthSeed  =  static_cast<nGrowthSeeds*>(nResourceServer::Instance()->NewResource("ngrowthseeds", 0 , nResource::Other));
    n_assert( growthSeed );

    //GetCellsList(nArray<nEntityObject*>& buildCellList, Mode mode )

    nArray<nEntityObject*> buildCellList;
    this->GetCellsList( buildCellList, mode );

    MakeRandomAccess(size , randomAccess);
    for ( int idx = 0 ; idx < buildCellList.Size()  ; ++idx)
    {
        nEntityObject*  currentCell = buildCellList[idx];
        this->BuildByCell(  currentCell  , size);
    }
    randomAccess.Reallocate( 1,0);


    for ( int idx = 0 ; idx < numMat ; ++idx )
    {
        nGrowthMaterial*  material= vegClass->GetGrowthMaterialById(idx);
        material->EndBuild();
    }
    n_setseed( oldRandomSeed );

    this->growthSeed->Release();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
nVegetationBuilder::BuildByCell(nEntityObject* currentCell, int size)
{
    currentCell->SetObjectDirty();
    
    //ncTerrainVegetationCell* cell = currentCell->GetComponentSafe<ncTerrainVegetationCell>();
    ncTerrainGMMCell*         gmmCell  = currentCell->GetComponentSafe<ncTerrainGMMCell>();
    ncTerrainVegetationCell*  vegCell  = currentCell->GetComponentSafe<ncTerrainVegetationCell>();
    ncTerrainVegetationClass* vegClass = this->outdoorClass->GetComponent<ncTerrainVegetationClass>();
    ncTerrainMaterialClass*   matClass = outdoorClass->GetComponentSafe<ncTerrainMaterialClass>();

    nTerrainCellInfo* cellInfo = matClass->GetTerrainCellInfo( gmmCell->GetCellX() , gmmCell->GetCellZ() );
    //nGrowthSeeds *growthSeed  = vegClass->GetGrowthSeeds(  gmmCell->GetCellX() , gmmCell->GetCellZ());
    nByteMap*  byteMap = vegCell->GetValidGrowthMap();
    this->growthSeed->SetFilename( vegClass->GetGrowthSeedFileName( gmmCell->GetCellX() , gmmCell->GetCellZ() ) );


    const bbox3& bbox = gmmCell->GetBBox();
    growthSeed->SetCellSize( bbox.vmin , bbox.vmax );

  
    //const bbox3& bbox = gmmCell->GetBBox();
    int numBlocks      = this->getNumBlocks();

    // The random construct is the same for each Block
    n_setseed( numBlocks* gmmCell->GetCellZ() + gmmCell->GetCellX()  );

    int div = vegClass->GetMaxSubCellIndexRelative();
    nGrowthSeeds::BuildGroupListSeed listSeed( div*div );

    //byteMap->CalculateAccumulator();
    bool isEmpty = byteMap->GetAccumulator() == 0;

    if ( ! isEmpty)
    {
        BuildListSeed( size , currentCell , listSeed );
        cellInfo->AddFlags(nTerrainCellInfo::HasGrass);
        /// Make and save it   
        this->growthSeed->CreateList(listSeed);
        this->growthSeed->Save();
    }  else
    {
        cellInfo->ClearFlags(nTerrainCellInfo::HasGrass);
        if ( nFileServer2::Instance()->FileExists( this->growthSeed->GetFilename() ) )
        {
            nFileServer2::Instance()->DeleteFile( this->growthSeed->GetFilename() );
        }
      
    }

    byteMap->CleanUserDirty();
}

//------------------------------------------------------------------------------
/**
*/
void 
nVegetationBuilder::BuildListSeed(int size, nEntityObject* currentCell, nGrowthSeeds::BuildGroupListSeed& listSeed)
{
    ncTerrainVegetationCell*  vegCell  = currentCell->GetComponentSafe<ncTerrainVegetationCell>();
    ncTerrainVegetationClass* vegClass = this->outdoorClass->GetComponent<ncTerrainVegetationClass>();
    ncTerrainGMMCell*         gmmCell  = currentCell->GetComponentSafe<ncTerrainGMMCell>();
    nArray2<nuint8> data(size,size);
    /*
    nArray<Index> randomAccess;
    MakeRandomAccess(size , randomAccess);
    */

    for ( int z =  0 ;  z < size ; ++z )
    {
        for ( int x = 0; x < size ; ++ x )
        {
            data.At(x,z) = 0;
        }
    }

    // Reset probability for all materials growth, Same generation for a same cell
    // And not all cell is generate always
    int numBlocks  = this->getNumBlocks();
    int randomSeed = numBlocks* gmmCell->GetCellZ() + gmmCell->GetCellX();
    const int numMat = vegClass->GetNumberOfGrowthMaterials();
    for ( int idx = 0 ; idx < numMat ; ++idx )
    {
        nGrowthMaterial*  material= vegClass->GetGrowthMaterialById(idx);
        material->SetRandomSeed(idx + randomSeed);
    }


    nByteMap*  byteMap = vegCell->GetValidGrowthMap();
    for ( int idx= 0; idx < randomAccess.Size () ; ++idx)
    {
        int x = randomAccess[idx].x;
        int z = randomAccess[idx].y;
        
        if ( data.At(x,z) == 0 ) // not use
        {           
            // Firs search the material id in byteMap 
            int mapX = x * byteMap->GetSize() / size; // not change the order 
            int mapZ = z * byteMap->GetSize() / size; // not change the order 
            int matId = byteMap->GetValue( mapX , mapZ );
            if ( matId > 0 )
            {
                nGrowthMaterial*  growtMaterial = vegClass->GetGrowthMaterialById(matId);
                nGrowthBrush* brush = growtMaterial->GetRandomBrush();
                if ( ! brush ) 
                {
                    data.At(x,z) = 254 ;  // SET empty brush
                } else
                {
                    
                    if ( TestBrush(brush , x , z , data) )
                    {
                        MarkInvalids( brush, x , z , data );
                        CreateSeed( currentCell, listSeed, this->growthSeed, brush, x ,z, size );
                    } else
                    {
                        growtMaterial->DiscardLastRandomBrush(); 
                    }

                }
            }
        }
    }

}

//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationBuilder::TestBrush(nGrowthBrush* brush, int x, int z , nArray2<nuint8>& data)
{
    float dis = brush->GetMinDistanceOtherBrush();
    dis /= builderSubdivisionSize;
    int intDist = static_cast<int>(dis);
    int mitDist = intDist;

    // Test if all texels aroun of x z is 0

    for ( int subX = x - mitDist; subX < x + mitDist ; ++subX) 
    {
        for ( int subZ = z - mitDist; subZ < z + mitDist; ++subZ)
        {
            if ( data.ValidIndex(subX,subZ) )
            {
                if ( data.At(subX,subZ) != 0 )
                {
                    return false;
                }
            } else
            {
                // This prevent to create object near of borders
                return false;
            }
        }
    }

    return true;
}
//------------------------------------------------------------------------------
/**
*/
void 
nVegetationBuilder::MarkInvalids(nGrowthBrush* brush, int x, int z , nArray2<nuint8>& data)
{
    float dis = brush->GetMinDistanceOtherBrush();
    dis /= builderSubdivisionSize;
    int intDist = static_cast<int>(dis);
    int mitDist = intDist;

    // Test if all texels around of x z is 0

    for ( int subX = x - mitDist; subX < x + mitDist ; ++subX) 
    {
        for ( int subZ = z - mitDist; subZ < z + mitDist; ++subZ)
        {
            if ( data.ValidIndex(subX,subZ) )
            {
                if ( data.At(subX,subZ) == 0 )
                {
                    data.At(subX,subZ) = 255;   
                }
            } 
        }
    }

     data.At(x,z) = static_cast<nuint8>(brush->GetMeshId() + 1); // Mark this
}

//------------------------------------------------------------------------------
/**
*/
void 
nVegetationBuilder::CreateSeed(nEntityObject* currentCell,
                               nGrowthSeeds::BuildGroupListSeed& listSeed,
                               nGrowthSeeds* growthSeeds,
                               nGrowthBrush* brush, 
                               int x, int z , int size)
{

    //Components
    ncTerrainVegetationCell*  vegCell  = currentCell->GetComponentSafe<ncTerrainVegetationCell>();
    ncTerrainGMMCell*         gmmCell  = currentCell->GetComponentSafe<ncTerrainGMMCell>();
    ncTerrainGMMClass*        gmmClass = this->outdoorClass->GetComponentSafe<ncTerrainGMMClass>();

    const bbox3& bbox = gmmCell->GetBBox();
    nFloatMap* heightMap = gmmClass->GetHeightMap();

    vector3 pos;
    pos.x   = bbox.vmin.x + ( bbox.vmax.x -bbox.vmin.x)*x/size;
    pos.z   = bbox.vmin.z + ( bbox.vmax.z -bbox.vmin.z)*z/size;
    float rot = n_rand_real_in_range (  0.0f , N_TWOPI);
    float scale  = n_rand_real_in_range (  brush->GetMinScale() , brush->GetMaxScale() );

    nGrowthSeeds::Seed  seed;
    growthSeeds->SetMeshId( seed , brush->GetMeshId() );
    growthSeeds->SetPosition( seed , pos );
    growthSeeds->SetRotationY( seed , rot);
    growthSeeds->SetScale( seed , scale);

    pos = growthSeeds->GetPosition( seed );


    // Test the height
    vector3 normal;
    if (   heightMap->GetHeightNormal(  pos.x,  pos.z,  pos.y, normal) ) 
    {
        int subx,subz,idx;
        growthSeeds->SetPosition( seed , pos );
        growthSeeds->SetNormal(seed ,  normal);
        normal.norm();
        vegCell->getSubCellIndex( pos.x, pos.z , subx , subz);
        idx = vegCell->getSubGroupIdx( subx, subz );
        listSeed[idx].Append(seed);
    }
}



//------------------------------------------------------------------------------
/**
*/
int
nVegetationBuilder::getNumBlocks()
{
    return this->outdoorClass->GetComponentSafe< ncTerrainGMMClass>()->GetNumBlocks();
}
//------------------------------------------------------------------------------
/**
*/
void
nVegetationBuilder::MakeRandomAccess(int numDiv, nArray<Index>& list)
{
    list.Reallocate(numDiv*numDiv ,0);
    for ( nuint16 y =  0 ;  y < numDiv ; ++y)
    {
        for ( nuint16 x = 0; x < numDiv ; ++ x )
        {
            Index& i=list.At( y*numDiv + x);
            i.x = x;
            i.y = y;
            i.rand = n_rand_func();
        }
    }
    list.QSort( nVegetationBuilder::IndexSorter);
}



//------------------------------------------------------------------------------
/**
*/
int 
__cdecl 
nVegetationBuilder::IndexSorter(const void* elm0, const void* elm1)
{
    const Index* e0 = static_cast<const Index*>(elm0);
    const Index* e1 = static_cast<const Index*>(elm1);
    return e0->rand - e1->rand;
}


//------------------------------------------------------------------------------
/**
*/
void 
nVegetationBuilder::GetCellsList(nArray<nEntityObject*>& buildCellList, Mode mode )
{
    buildCellList.Clear();
    if ( mode != nVegetationBuilder::Selected)
    {
        buildCellList.Reallocate( this->GetCellsList().Size() , 16);
        for ( int idx = 0 ; idx < this->GetCellsList().Size()  ; ++idx)
        {
            nEntityObject*  currentCell = this->GetCellsList()[idx];
            ncTerrainVegetationCell*  vegCell  = currentCell->GetComponentSafe<ncTerrainVegetationCell>();
            nByteMap*  byteMap = vegCell->GetValidGrowthMap();

            if ( mode == nVegetationBuilder::All || byteMap->IsUserDirty() )
            {
                buildCellList.Append( currentCell );
            }
        }

    } else
    {
        nObjectEditorState * obj = static_cast<nObjectEditorState*>(nConjurerApp::Instance()->FindState("object"));
        buildCellList.Reallocate( obj->GetSelectionCount() , 16);

        for ( int idx = 0 ; idx < obj->GetSelectionCount(); ++idx)
        {
            nEntityObject* currentCell = obj->GetSelectedEntity( idx );
            if ( currentCell && currentCell->GetComponent<ncTerrainVegetationCell>() )
            {
                buildCellList.Append( currentCell );
            }
        }
    }
}