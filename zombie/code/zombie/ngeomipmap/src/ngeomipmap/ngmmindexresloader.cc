//------------------------------------------------------------------------------
//  ngmmindexresloader.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmindexresloader.h"
#include "ngeomipmap/ngmmindexbuilder.h"
#include "ngeomipmap/ngmmmeshstorage.h"
#include "zombieentity/ncloaderclass.h"
#include "entity/nentityclass.h"
#include "entity/nentityclassserver.h"
#include "gfx2/nmesh2.h"

//---------------------------------------------------------------------------
nNebulaClass(nGMMIndexResLoader, "nresourceloader");

//------------------------------------------------------------------------------
static const char * const SEPARATOR = "!";
nGMMIndexResLoader * nGMMIndexResLoader::singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nGMMIndexResLoader::nGMMIndexResLoader()
{
    if (!this->singleton)
    {
        this->singleton = this;
    }
}

//------------------------------------------------------------------------------
/**
*/
nGMMIndexResLoader::~nGMMIndexResLoader()
{
    if (this->singleton == this)
    {
        this->singleton = 0;
    }
}


//------------------------------------------------------------------------------
/**
*/
nGMMIndexResLoader * 
nGMMIndexResLoader::Instance()
{
    if (!nGMMIndexResLoader::singleton)
    {
        nGMMIndexResLoader::singleton = static_cast<nGMMIndexResLoader *> (kernelServer->New("ngmmindexresloader", "/sys/loaders/gmmindex"));
        n_assert(nGMMIndexResLoader::singleton);
    }

    return nGMMIndexResLoader::singleton;
}

//------------------------------------------------------------------------------
/**
*/
nString 
nGMMIndexResLoader::GenerateResourceString( const char * terrainClassName, int lod )
{
    n_assert(terrainClassName);
    n_assert(lod >= 0 && lod < MaxLODLevels);
    
    nString str;
    str.Format("%s%s%d", terrainClassName, SEPARATOR, lod );
    return str;
}

//------------------------------------------------------------------------------
/**
*/
nEntityClass *
nGMMIndexResLoader::ParseResourceString( const char * str, int & lod )
{
    n_assert(str);

    nString stringParser(str);

    const char * terrainClassStr = stringParser.GetFirstToken(SEPARATOR);
    n_assert(terrainClassStr);
    lod = atoi(stringParser.GetNextToken(SEPARATOR));

    nEntityClass * terrainClass = static_cast<nEntityClass *> (nEntityClassServer::Instance()->GetEntityClass(terrainClassStr));
    n_assert(terrainClass);

    return terrainClass;
}

//------------------------------------------------------------------------------
/**
    @param cmdString Name of the resource
    @param callingResource ptr to the nResource (nMesh2)
    @return bool indicating success/failure
*/
bool 
nGMMIndexResLoader::Load(const char *cmdString, nResource *callingResource)
{
    n_assert(cmdString);
    n_assert(callingResource);

    // check the command string
    int lod;
    nEntityClass * terrainClass = ParseResourceString( cmdString, lod );
    n_assert(terrainClass);
    ncTerrainGMMClass * gmmc = terrainClass->GetComponent<ncTerrainGMMClass>();
    n_assert(gmmc);

    // get the mesh resource
    nMesh2* mesh = static_cast<nMesh2*>(callingResource);
    n_assert(mesh);

    // create geometry storage for the builder
    nGMMMeshStorage * meshStorage = n_new(nGMMMeshStorage);
    meshStorage->SetupMesh(mesh);

    // get the index builder
    nGMMIndexBuilder * indexBuilder = gmmc->GetIndexBuilder();
    indexBuilder->SetupStorage(meshStorage);

    // set the mesh parameters
    mesh->SetUsage( nMesh2::WriteOnce | nMesh2::NeedsVertexShader );
    mesh->SetNumVertices( 0 );
    mesh->SetNumIndices( indexBuilder->CountGroups(lod) * indexBuilder->CountIndices(lod) );
    mesh->SetNumGroups( indexBuilder->CountGroups(lod) );
    mesh->CreateEmpty();

    // fill the indices
    meshStorage->BeginTerrainMesh();
    for(int i = 0;i < mesh->GetNumGroups();i++)
    {
        int lodN, lodE, lodS, lodW;
        indexBuilder->ParseIndexGroup(lod, i, lodN, lodE, lodS, lodW);
        meshStorage->BeginTerrainGroup(0, indexBuilder->CountIndices(lod));
        indexBuilder->FillIndicesBlock(lod, lodN, lodE, lodS, lodW);
        meshStorage->EndTerrainGroup();
    }
    meshStorage->EndTerrainMesh();

    // reset storage
    indexBuilder->SetupStorage(0);

    return true;
}
