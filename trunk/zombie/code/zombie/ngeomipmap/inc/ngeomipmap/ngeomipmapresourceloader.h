#ifndef N_GEOMIPMAPRESOURCELOADER_H
#define N_GEOMIPMAPRESOURCELOADER_H
//------------------------------------------------------------------------------
/**
    @file ngeomipmapresourceloader.h
    @class nGeoMipMapResourceLoader
    @ingroup NebulaTerrain
    
    @author Mateu Batle Sastre

    @brief Loads nGeoMipMapNode height information into an nMesh2

    Only one instance of nMapResourceLoader need be created per application.  
    Its NOH path should be passed to each MapBlock::meshTriStrip with SetResourceLoader().
    The MapBlock from which meshTriStrip is initialized must be assigned with meshTriStrip->SetFilename().
    Both should be set prior to any other use of meshTriStrip.

    (c) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "resource/nresourceloader.h"

//------------------------------------------------------------------------------
class nResource;
class nMesh2;
class nFloatMap;
class nString;
class nEntityClass;

//------------------------------------------------------------------------------
/**
*/
class nGeoMipMapResourceLoader : public nResourceLoader
{
public:
    /// constructor
    nGeoMipMapResourceLoader();
    /// destructor
    virtual ~nGeoMipMapResourceLoader();

    /// instance
    static nGeoMipMapResourceLoader * Instance();

    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// Primary load method, the one called by an nResource.
    bool Load(const char *cmdString, nResource *callingResource);

    /// generate the resource loader string
    static nString & GenerateResourceLoaderString( nString & str, nEntityClass * terrainClass, int bx, int bz, int indexKey, int numVertices, int numIndices );
    /// parse the resource loader string
    static nEntityClass * ParseResourceLoaderString( const char * res, int & bx, int & bz, int & indexKey, int & numVertices, int & numIndices );

private:

    static nGeoMipMapResourceLoader * singleton;

};

//------------------------------------------------------------------------------
#endif 
