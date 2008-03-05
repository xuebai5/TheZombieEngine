/*-----------------------------------------------------------------------------
    @file ngeomipmapnode_main.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    @brief Terrain rendering methods

    (C) 2004 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "zombieentity/ncloaderclass.h"
#include "entity/nentityclass.h"
#include "entity/nentityclassserver.h"
#include "gfx2/nmesh2.h"
#include "ngeomipmap/ngmmvertexbuilder.h"
#include "ngeomipmap/ngmmindexbuilder.h"
#include "ngeomipmap/ngmmmeshstorage.h"

//---------------------------------------------------------------------------
nNebulaScriptClass(nGeoMipMapResourceLoader, "nresourceloader");

//------------------------------------------------------------------------------
static const char * const SEPARATOR = "!";
nGeoMipMapResourceLoader * nGeoMipMapResourceLoader::singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nGeoMipMapResourceLoader::nGeoMipMapResourceLoader()
{
    if (!this->singleton)
    {
        this->singleton = this;
    }
}

//------------------------------------------------------------------------------
/**
*/
nGeoMipMapResourceLoader::~nGeoMipMapResourceLoader()
{
    if (this->singleton == this)
    {
        this->singleton = 0;
    }
}


//------------------------------------------------------------------------------
/**
*/
nGeoMipMapResourceLoader * 
nGeoMipMapResourceLoader::Instance()
{
    return singleton;
}

//------------------------------------------------------------------------------
/**
*/
nString &
nGeoMipMapResourceLoader::GenerateResourceLoaderString( nString & str, nEntityClass * terrainClass, int bx, int bz, int indexKey, int numVertices, int numIndices )
{
    char resourceName[N_MAXPATH];

    n_assert(terrainClass);
    N_IFDEF_ASSERTS(ncTerrainGMMClass * gfxTerrain = terrainClass->GetComponent<ncTerrainGMMClass>() );
    n_assert(gfxTerrain);
    n_assert(bx >= 0 && bx < gfxTerrain->GetNumBlocks());
    n_assert(bz >= 0 && bz < gfxTerrain->GetNumBlocks());
    n_assert(indexKey >= 0);
    
    sprintf(resourceName, "%s%s%d%s%d%s%d%s%d%s%d", 
        terrainClass->nClass::GetName(),
        SEPARATOR,
        bx,
        SEPARATOR,
        bz,
        SEPARATOR,
        indexKey,
        SEPARATOR,
        numVertices,
        SEPARATOR,
        numIndices );

    str.Set(resourceName);

    return str;
}

//------------------------------------------------------------------------------
/**
*/
nEntityClass *
nGeoMipMapResourceLoader::ParseResourceLoaderString( const char * res, int & x, int & z, int & indexKey, int & numVertices, int & numIndices )
{
    n_assert(res);

    nString stringParser(res);

    const char * terrainClassStr = stringParser.GetFirstToken(SEPARATOR);
    n_assert(terrainClassStr);
    x = atoi(stringParser.GetNextToken(SEPARATOR));
    z = atoi(stringParser.GetNextToken(SEPARATOR));
    indexKey = atoi(stringParser.GetNextToken(SEPARATOR));
    numVertices = atoi(stringParser.GetNextToken(SEPARATOR));
    numIndices = atoi(stringParser.GetNextToken(SEPARATOR));

    nEntityClass * terrainClass = static_cast<nEntityClass *> (nEntityClassServer::Instance()->GetEntityClass(terrainClassStr));
    n_assert(terrainClass);

    return terrainClass;
}

//------------------------------------------------------------------------------
/**
    Fills the mesh with the proper terrain block data according to the cmdString.

    @param cmdString Name of the resource
    @param callingResource ptr to the nResource (nMesh2)
    @return bool indicating success/failure
*/
bool 
nGeoMipMapResourceLoader::Load(const char *cmdString, nResource *callingResource)
{
    n_assert(cmdString);
    n_assert(callingResource);

    // check the command string
    int startX, startZ, indexKey, numVertices, numIndices;
    nEntityClass * terrainClass = ParseResourceLoaderString( cmdString, startX, startZ, indexKey, numVertices, numIndices );
    n_assert(terrainClass);

    /// get the mesh resource
    nMesh2* mesh = static_cast<nMesh2*>(callingResource);
    n_assert(mesh);

    /// set the mesh parameters
    n_assert(numVertices > 0 || numIndices > 0);
    mesh->SetVertexComponents( nMesh2::Coord | nMesh2::Normal );
    //mesh->SetUsage( nMesh2::WriteOnly | nMesh2::NeedsVertexShader );
    mesh->SetUsage( nMesh2::NeedsVertexShader );
    mesh->SetNumVertices( numVertices );
    mesh->SetNumIndices( numIndices );
    mesh->SetNumGroups(1);
    mesh->CreateEmpty();

    /// use a heightmap mesh builder with proper parameters
    //nHeightMapMeshBuilder meshBuilder;
    nGMMMeshStorage * meshStorage = n_new(nGMMMeshStorage);
    meshStorage->AddRef();
    meshStorage->SetupMesh(mesh);

    ncTerrainGMMClass * terrainComp = terrainClass->GetComponent<ncTerrainGMMClass>();
    n_assert(terrainComp);
    //meshBuilder.Setup(terrainComp);

    // fill the vertex buffer
    //int blockSize = terrainComp->GetBlockSize();
    //meshBuilder.SetMesh(mesh);
    //meshBuilder.BeginTerrainMesh();

    meshStorage->BeginTerrainMesh();
    if (mesh->GetNumVertices() > 0)
    {
        nGMMVertexBuilder * vertexBuilder = terrainComp->GetVertexBuilder();
        vertexBuilder->SetupStorage(meshStorage);

        vertexBuilder->FillVerticesBlock(startX, startZ, 0);
        //meshBuilder.FillVertexCoord(startX * (blockSize - 1), startZ * (blockSize - 1));
        if (mesh->GetNumIndices() > 0) 
        {
            n_assert_always();
            //int lod;
            //bool crackN, crackE, crackS, crackW;
            //nHeightMapMeshBuilder::ParseIndexGroupKey(indexKey, lod, crackN, crackE, crackS, crackW);
            //meshBuilder.FillIndexTriangleListLOD(lod, crackN, crackE, crackS, crackW, &nHeightMapMeshBuilder::GetVertexIndex);

            //nGMMIndexBuilder * indexBuilder = terrainComp->GetIndexBuilder();
            //indexBuilder->ParseIndexGroup(0, group )
        }
        vertexBuilder->SetupStorage(0);
    }
    else
    {
        nGMMIndexBuilder * indexBuilder = terrainComp->GetIndexBuilder();
        indexBuilder->SetupStorage(meshStorage);

        indexBuilder->FillIndicesHoledBlock(startX, startZ, 0, 0, 0, 0, 0);

        //int lod;
        //bool crackN, crackE, crackS, crackW;

        //for(int key = 0;key < mesh->GetNumGroups();key++)
        //{
        //    meshBuilder.ParseIndexGroupKey(key, lod, crackN, crackE, crackS, crackW);
        //    meshBuilder.BeginTerrainGroup(0, meshBuilder.CountIndices(lod));
        //    meshBuilder.FillIndexTriangleListLOD(lod, lod + int(crackN), lod + int(crackE), lod + int(crackS), lod + int(crackW), &nHeightMapMeshBuilder::GetVertexIndex);
        //    meshBuilder.EndTerrainGroup();
        //}
        indexBuilder->SetupStorage(0);
    }
    meshStorage->EndTerrainMesh();
    meshStorage->Release();

    //meshBuilder.EndTerrainMesh();
    //meshBuilder.Setup(0);

    return true;
}

//---------------------------------------------------------------------------
