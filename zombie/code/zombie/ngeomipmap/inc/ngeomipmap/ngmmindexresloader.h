#ifndef N_GMMINDEXRESLOADER_H
#define N_GMMINDEXRESLOADER_H
//------------------------------------------------------------------------------
/**
    @file ngmmindexresloader.h
    @class nGMMIndexResLoader
    @ingroup NebulaTerrain
    
    @author Mateu Batle Sastre

    @brief Loads geomipmap index information into an nMesh2

    (c) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "resource/nresourceloader.h"

//------------------------------------------------------------------------------
class nResource;

//------------------------------------------------------------------------------
/**
*/
class nGMMIndexResLoader : public nResourceLoader
{
public:
    /// constructor
    nGMMIndexResLoader();
    /// destructor
    virtual ~nGMMIndexResLoader();

    /// instance
    static nGMMIndexResLoader * Instance();

    /// Primary load method, the one called by an nResource.
    bool Load(const char *cmdString, nResource *callingResource);

    /// generate the resource loader string
    static nString GenerateResourceString( const char * terrainClassName, int lod );
    /// parse the resource loader string
    static nEntityClass * ParseResourceString( const char * res, int & lod );

private:

    static nGMMIndexResLoader * singleton;

};

//------------------------------------------------------------------------------
#endif  // N_GMMINDEXRESLOADER_H
