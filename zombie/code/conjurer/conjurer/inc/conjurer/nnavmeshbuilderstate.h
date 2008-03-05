#ifndef N_NAVMESHBUILDERSTATE_H
#define N_NAVMESHBUILDERSTATE_H
//------------------------------------------------------------------------------
/**
    @class nNavMeshBuilderState
    @ingroup NebulaConjurerEditor

    Editor state for building and saving navigation meshes.

    (C) 2005 Conjurer Services, S.A.
*/
#include "conjurer/neditorstate.h"
#include "kernel/ncmdprotonativecpp.h"
#include <ctime>

class nNavMesh;
class nNavMeshBuilder;

//------------------------------------------------------------------------------
class nNavMeshBuilderState : public nEditorState
{
public:
    /// constructor
    nNavMeshBuilderState();
    /// destructor
    virtual ~nNavMeshBuilderState();
    /// called when state is created
    virtual void OnCreate(nApplication* app);

    /// @name Script interface
    //@{

    /// build the navigation mesh, discarding the previous one
    bool BuildNavMesh();
    /// build the navigation mesh for an indoor, discarding the previous one
    bool BuildIndoorNavMesh(nEntityObject*);
    /// build the navigation mesh for a brush, discarding the previous one
    bool BuildBrushNavMesh(nEntityObject*);
    /// load a navigation mesh from a file
    bool LoadNavMesh(const char*);
    /// save the current navigation mesh to file
    bool SaveNavMesh(const char*);
    /// save the current outdoor's navigation mesh to file
    bool SaveOutdoorNavMesh(const char*);
    /// remove all external links from all navigation graphs
    void ClearExternalLinks();
    /// generate external links to connect superimposed navigation graphs
    void GenerateExternalLinks();

    /// Until which height above floor level an obstacle can rise without being considered as such
    void SetMinObstacleHeight(float);
    float GetMinObstacleHeight();
    /// Until which height above floor level an obstacle is considered as such
    void SetCharacterHeight(float);
    float GetCharacterHeight();
    /// Enable/disable the use of triable meshes as obstacles
    void SetTriangleMeshesAsObstacles(bool);
    bool GetTriangleMeshesAsObstacles();
    /// Minimum slope angle to create another navigation node
    void SetMinHardSlope(float);
    float GetMinHardSlope();
    /// Maximum walkable slope angle
    void SetMaxWalkableSlope(float);
    float GetMaxWalkableSlope();
    /// Sea level
    void SetSeaLevel(float);
    float GetSeaLevel();
    /// Use of terrain holes to hole the navmesh
    void SetTerrainHolesEnabled(bool);
    bool GetTerrainHolesEnabled();
    /// Enable/disable the merging of polygons
    void SetMergePolygons(bool);
    bool GetMergePolygons();
    /// Enable/disable fitting of polygons to obstacles
    void SetFitToObstacles(bool);
    bool GetFitToObstacles();
    /// Minimum size for obstacles
    void SetMinObstacleSize(float);
    float GetMinObstacleSize();
    /// Enable/disable the generation of portals
    void SetGeneratePortals(bool);
    bool GetGeneratePortals();

    //@}

private:
    bool BuildSpaceNavMesh( nPhySpace* space, nNavMesh* navMesh );
    void LogStartTime();
    void LogEndTime();

    nNavMeshBuilder* navBuilder;
    nNavMesh* navMesh;
    int subspaceLevel;
    time_t startTime;
};
//------------------------------------------------------------------------------
#endif
