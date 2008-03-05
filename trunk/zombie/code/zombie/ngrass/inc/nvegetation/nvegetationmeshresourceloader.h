#ifndef N_VEGETATIONMESHRESOURCELOADER_H
#define N_VEGETATIONMESHRESOURCELOADER_H

/*-----------------------------------------------------------------------------
    @file nVegetationMeshResourceLoader.h"
    @class nVegetationMeshResourceLoader
    @ingroup NebulaGrass
    @author Cristobal Castillo Domingo

    @brief nVegetationMeshResourceLoader
    Only one instance of nVegetationMeshResourceLoader need be created per application.
    This create a nMesh2 resource from ngrowthseeds
    (C) 2004 Conjurer Services, S.A.

*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "resource/nresourceloader.h"
class nMesh2;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    Class nVegetationMeshResourceLoader
*/
class nVegetationMeshResourceLoader : public nResourceLoader
{
public:
    /// constructor
    nVegetationMeshResourceLoader();
    /// destructor
    virtual ~nVegetationMeshResourceLoader();
    /// persistence of nebual
    virtual bool SaveCmds(nPersistServer* ps);
    /// instance
    static nVegetationMeshResourceLoader  * Instance();
    /// Primary load method, the one called by an nResource.
    bool Load(const char *cmdString, nResource *callingResource);

        //EMpty
    
private:
    static nVegetationMeshResourceLoader * singleton;
    /// transform
    bool CopyAndTransformVertices(nMesh2* mesh,float* vbDst, int idxBegin, float *vbSrc ,int numVertices, const matrix44& matrix);
};

#endif // N_VEGETATIONMESHRESOURCELOADER_H
//------------------------------------------------------------------------------