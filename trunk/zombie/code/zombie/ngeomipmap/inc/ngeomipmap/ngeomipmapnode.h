#ifndef N_GEOMIPMAPNODE_H
#define N_GEOMIPMAPNODE_H
//------------------------------------------------------------------------------
/**
    @file ngeomipmapnode.h
    @class nGeoMipMapNode
    @ingroup NebulaTerrain
    
    @author Mateu Batle Sastre

    @brief nGeoMipMapNode is scene node used to render a terrain tile using the
    geometrical mipmapping algorithm.

    Only one nGeoMipMapTerrain node is needed to render all the tiles of a terrain.
    The information of what tile, and LOD of the tile, etc. is provided through
    the render context.

    See also @ref N2ScriptInterface_ngeomipmapnode and @ref nGeoMipMapNode

    The following information must be precalculated offline, since it is not 
    changing from frame to frame:
    - minD2 for each LOD
    - bounding box of the cell

    (c) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nscene/ngeometrynode.h"
#include "gfx2/ndynamicmesh.h"
#include "ngeomipmap/ngeomipmap.h"
#include "ngeomipmap/ngmmlowmeshcacheentry.h"
#include "ngeomipmap/ngmmholeindicescacheentry.h"
#include "ngeomipmap/ngeomipmapviewport.h"
#include "ngeomipmap/ncache.h"
#include "util/nringbuffer.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
class nDynamicMesh;
class nPersistServer;
class ncTerrainGMMClass;
class ncTerrainGMM;
class nMesh2;
class nGMMDynamicMeshStorage;

//------------------------------------------------------------------------------
/**
    Class nGeoMipMapNode is the scene node located in the scene graph which
    has the responsability of doing the actual rendering of the terrain.
    It is a geometry node.
*/
class nGeoMipMapNode : public nGeometryNode
{
public:
    /// constructor
    nGeoMipMapNode();
    /// destructor
    virtual ~nGeoMipMapNode();
    /// called on initialization
    void InitInstance(nObject::InitInstanceMsg);

    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// Preload resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// call on new created entity
    void EntityCreated(nEntityObject* entityObject);
    /// Set shader pararameters shared by all cells & shaders
    void SetShaderParamsShared(nShaderParams & shaderParams) const;
    /// Set shader parameters for terrain detailed pass
    void SetShaderParamsGMM0(nShaderParams & shaderParams, ncTerrainGMMCell* cell);

    /// set terrain component 
    void SetTerrainGMM(ncTerrainGMM *);

    /// method called when node attached to the scene
    virtual void Attach(nSceneGraph *sceneGraph, nEntityObject *entityObject);
    /// Pre-instancing geometry call
    virtual bool Apply(nSceneGraph* sceneGraph);    
    /// update internal state and render
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject *entityObject);
    /// perfor post-instancing rendering of geometry
    virtual bool Flush(nSceneGraph* sceneGraph);

#ifndef NGAME
    /// Render the grass edition map
    void SetRenderGrassEdition(bool render);
    /// Get if the render grass edition is active;
    bool GetRenderGrassEdition();
#endif

protected:

    /// restore on lost device, restore resources
    void RestoreOnLostDevice(void);

    /// Attach a cell
    int AttachCell(nSceneGraph *sceneGraph, ncTerrainGMMCell* cell, nGeoMipMapViewport * gmmvp);
    /// Attach cell to scenegraph depending on the subMaterialLOD
    void AttachSceneGraph(nSceneGraph *sceneGraph, nEntityObject *entityObject, nGeoMipMapViewport * gmmvp, int sumMaterialLOD);

    /// Render the mesh, calls to gfx server
    void RenderMesh(ncTerrainGMMCell* cell, nMesh2 * vmesh, nMesh2 * imesh, int group);
    /// rendering of terrain materials
    bool RenderMaterial(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell);

    /// Render a terrain cell
    void RenderCell(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell, nGeoMipMapViewport * gmmvp, bool firstlevel);
    /// Scan cache, and render any cell visible which is already calculated
    void RenderVisibleCellsInCache(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell);
    /// Render a leaf cell through the dynamic mesh
    void RenderCellDynamicMesh(ncTerrainGMMCell* cell);
    /// Render a cell and cache it
    void RenderCellAndCache(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell);
    /// log the contents of the cache
    void LogCache();

    /// Find viewport data from scenegraph
    nGeoMipMapViewport * FindViewport(nSceneGraph* sceneGraph, nEntityObject * entityObject);

    /// reset statistics information
    void ResetStatistics();
    /// add statistical information
    void AddStatisticsCell(int numCalls, int numPrimitives, int lod);
    /// show statistics
    void ShowStatistics();

    void PreloadCache(const nGeoMipMapViewport * vpSettings);

    /// Map data source
    ncTerrainGMMClass * terrainGMMClass;
    ncTerrainGMM * terrainGMM;

    // dynamic mesh
    nDynamicMesh dynMesh;
    /// mesh storage used for dynamic meshes
    nGMMDynamicMeshStorage * dynMeshStorage;

    nRef<nTexture2> refErrorTexture;

    /// stats info
    int numPrimitives;
    int numCalls;
    struct {
        int numPrimitives;
        int numCells;
    } infoLOD[MaxLODLevels];

    // Per viewport settings
    nGeoMipMapViewport vp[8];

    nProfiler profAttach;
    nProfiler profRender;
    nProfiler profRenderFill;
    nProfiler profRenderLOD;
    nProfiler profRenderMaterial;

    int passFlagGMM0;
    int passFlagGMM1;

    // index to ncTerrainGMMCell to optimize access
    int ncTerrainGMMCellIndex;
    // index to ncSpatialQuadtreeCell to optimize access
    int ncSpatialQuadtreeCellIndex;
    // frame id from nApplication to optimize acces
    int frameId;

#ifndef NGAME
    bool renderGrassEdition;
    int passFlagGMGE;
    int passFlagGMLM;
    int passFlagGMSD;
    /// rendering of grass edition  maps
    bool RenderGrassEditionMaterial(nSceneGraph* sceneGraph, ncTerrainGMMCell* cell);
#endif

};

//------------------------------------------------------------------------------
#endif
