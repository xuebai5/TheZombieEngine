#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nnavmeshbuilderstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nnavmeshbuilderstate.h"
#include "nnavmeshbuilder/nnavmeshbuilder.h"
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmesh.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsworld.h"
#include "nphysics/nphygeomspace.h"
#include "nphysics/ncphyterrain.h"
#include "nphysics/ncphyindoor.h"
#include "resource/nresourceserver.h"
#include "conjurer/nconjurerapp.h"
#include "nspatial/ncspatialquadtree.h"
#include "ncrnswaypoint/ncrnswaypoint.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "zombieentity/nctransform.h"
#include "kernel/nlogclass.h"

nNebulaScriptClass(nNavMeshBuilderState, "neditorstate");

//------------------------------------------------------------------------------
/**
*/
nNavMeshBuilderState::nNavMeshBuilderState() :
    navBuilder(NULL),
    navMesh(NULL),
    subspaceLevel( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNavMeshBuilderState::~nNavMeshBuilderState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nNavMeshBuilderState::OnCreate(nApplication* app)
{
    nEditorState::OnCreate(app);

    // Create the shared mesh used by this state (to work with it) and the viewport debug module (to draw it)
    this->navMesh = static_cast<nNavMesh*>( nResourceServer::Instance()->NewResource("nnavmesh", "workingnavmesh", nResource::Other) );
}

//------------------------------------------------------------------------------
/**
*/
bool
nNavMeshBuilderState::BuildNavMesh()
{
    bool success( false );

    nEntityObject* outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    if ( outdoor )
    {
        this->LogStartTime();

        // Clear previous navigation mesh
        nNavMesh* navMesh = outdoor->GetComponentSafe<ncNavMesh>()->GetNavMesh();
        navMesh->Unload();

        // Create the builder
        navBuilder = n_new(nNavMeshBuilder);
        navBuilder->SetNavMesh(navMesh);

        // Build the mesh
        nPhySpace* space = outdoor->GetComponentSafe<ncPhyTerrain>()->GetTerrainSpace();
        if ( space )
        {
            navBuilder->SetOutdoorSpace(space);
            success = navBuilder->GenerateMesh();
        }

        // Delete the builder
        n_delete(navBuilder);

        this->LogEndTime();
    }

    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool
nNavMeshBuilderState::BuildIndoorNavMesh( nEntityObject* indoor )
{
    bool success( false );

    n_assert( indoor );
    if ( indoor )
    {
        ncNavMesh* navComp = indoor->GetComponentSafe<ncNavMesh>();
        ncPhyIndoor* phyComp = indoor->GetComponentSafe<ncPhyIndoor>();
        if ( navComp && phyComp )
        {
            this->LogStartTime();

            // Clear previous navigation mesh
            nNavMesh* navMesh = navComp->GetNavMesh();
            navMesh->Unload();

            // Create the builder
            navBuilder = n_new( nNavMeshBuilder );
            navBuilder->SetNavMesh( navMesh );

            // Build the mesh
            nPhySpace* space = phyComp->GetIndoorSpace();
            if ( space )
            {
                success = this->BuildSpaceNavMesh( space, navMesh );
            }

            // Delete the builder
            n_delete( navBuilder );

            this->LogEndTime();
        }
    }

    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool
nNavMeshBuilderState::BuildBrushNavMesh( nEntityObject* brush )
{
    bool success( false );

    n_assert( brush );
    if ( brush )
    {
        ncNavMesh* navComp = brush->GetComponentSafe<ncNavMesh>();
        ncPhyCompositeObj* phyComp = brush->GetComponentSafe<ncPhyCompositeObj>();
        if ( navComp && phyComp )
        {
            this->LogStartTime();

            // Clear previous navigation mesh
            nNavMesh* navMesh = navComp->GetNavMesh();
            navMesh->Unload();

            // Create the builder
            navBuilder = n_new( nNavMeshBuilder );
            navBuilder->SetNavMesh( navMesh );

            // Build the mesh
            navBuilder->SetBrushSpace( phyComp );
            success = navBuilder->GenerateMesh();

            // Delete the builder
            n_delete( navBuilder );

            this->LogEndTime();
        }
    }

    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool
nNavMeshBuilderState::BuildSpaceNavMesh( nPhySpace* space, nNavMesh* navMesh )
{
    bool success( true );
    ++this->subspaceLevel;

    // Generate navmeshes for all subspaces
    bool childSpaces( false );
    for ( int index(0); index < space->GetNumGeometries(); ++index )
    {
        nPhysicsGeom* geom( space->GetGeometry(index) );
        if ( geom->IsSpace() )
        {
            NLOG_REL( navbuild, (NLOGUSER | 0, "Beginning space %d/%d (subspace level %d)",
                index+1, space->GetNumGeometries(), this->subspaceLevel) );
            success &= this->BuildSpaceNavMesh( static_cast< nPhySpace* >( geom ), navMesh );
            childSpaces = true;
            NLOG_REL( navbuild, (NLOGUSER | 0, "Finished space %d/%d (subspace level %d)",
                index+1, space->GetNumGeometries(), this->subspaceLevel) );
        }
    }

    // If it's a leaf space, generate and append the navmesh for this space
    if ( !childSpaces )
    {
        nNavMesh* tmpMesh( static_cast<nNavMesh*>( nKernelServer::Instance()->New("nnavmesh") ) );
        n_assert( tmpMesh );
        if ( tmpMesh )
        {
            nNavMeshBuilder* tmpBuilder = n_new( nNavMeshBuilder );

            // Generate mesh
            tmpBuilder->SetNavMesh( tmpMesh );
            tmpBuilder->SetIndoorSpace( space );
            success &= tmpBuilder->GenerateMesh();

            // Add space mesh to indoor mesh
            navMesh->AddMesh( tmpMesh );
            tmpMesh->ClearMesh();

            n_delete( tmpBuilder );
            tmpMesh->Release();
        }
    }

    --this->subspaceLevel;
    return success;
}

//------------------------------------------------------------------------------
/**
*/
void
nNavMeshBuilderState::LogStartTime()
{
    this->startTime = time( NULL );
    tm* localTime = localtime( &startTime );
    NLOG_REL( navbuild, (NLOGUSER | 0, "Navmesh generation started at %s", asctime(localTime)) );
}

//------------------------------------------------------------------------------
/**
*/
void
nNavMeshBuilderState::LogEndTime()
{
    time_t endTime = time( NULL );
    tm* localTime = localtime( &endTime );
    NLOG_REL( navbuild, (NLOGUSER | 0, "Navmesh generation ended at %s", asctime(localTime)) );

    double timeElapsed = difftime(endTime, this->startTime);
    double hoursElapsed = timeElapsed / 3600.0f;
    NLOG_REL( navbuild, (NLOGUSER | 0, "Time elapsed: %.0f seconds (%.1f hours)", timeElapsed, hoursElapsed) );
}

//------------------------------------------------------------------------------
/**
*/
bool
nNavMeshBuilderState::LoadNavMesh(const char* filename)
{
    // Clear the mesh
    this->navMesh->Unload();

    // Load the mesh
    this->navMesh->SetFilename(filename);
    return this->navMesh->Load();
}

//------------------------------------------------------------------------------
/**
*/
bool
nNavMeshBuilderState::SaveNavMesh(const char* filename)
{
    // Create the builder
    navBuilder = n_new(nNavMeshBuilder);
    navBuilder->SetNavMesh(navMesh);
    bool success = false;

    // Save the mesh
    success = navBuilder->SaveNavMesh(filename);

    // Delete the builder
    n_delete(navBuilder);
    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool
nNavMeshBuilderState::SaveOutdoorNavMesh(const char* filename)
{
    // Get the outdoor's navigation mesh
    nEntityObject* outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
    if ( !outdoor )
    {
        return false;
    }
    nNavMesh* navMesh = outdoor->GetComponentSafe<ncNavMesh>()->GetNavMesh();
    if ( !navMesh )
    {
        return false;
    }

    // Save the navigation mesh to disk, using the builder to allow saving the mesh to another file
    navBuilder = n_new(nNavMeshBuilder);
    navBuilder->SetNavMesh(navMesh);
    bool success = false;
    success = navBuilder->SaveNavMesh(filename);
    n_delete(navBuilder);
    return success;
}

//------------------------------------------------------------------------------
/**
*/
void
nNavMeshBuilderState::GenerateExternalLinks()
{
    this->ClearExternalLinks();

    // Look for any waypoint that's on a navigation mesh node and link both together
    for ( nEntityObject* entity( nEntityObjectServer::Instance()->GetFirstEntityObject() ); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        ncRnsWaypoint* waypoint( entity->GetComponent<ncRnsWaypoint>() );
        if ( waypoint )
        {
            waypoint->GenerateExternalLinks();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nNavMeshBuilderState::ClearExternalLinks()
{
    // Look for all nav nodes and remove their external links
    for ( nEntityObject* entity( nEntityObjectServer::Instance()->GetFirstEntityObject() ); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        ncNavNode* node( entity->GetComponent<ncNavNode>() );
        if ( node )
        {
            node->ClearExternalLinks( ncNavNode::DO_NOT_REMOVE_SELF_FROM_TARGET );
        }
    }
}

//------------------------------------------------------------------------------
/**
    SetMinObstacleHeight
*/
void nNavMeshBuilderState::SetMinObstacleHeight( float height )
{
    nNavMeshBuilder::SetMinObstacleHeight( height );
}

//------------------------------------------------------------------------------
/**
    GetMinObstacleHeight
*/
float nNavMeshBuilderState::GetMinObstacleHeight()
{
    return nNavMeshBuilder::GetMinObstacleHeight();
}

//------------------------------------------------------------------------------
/**
    SetCharacterHeight
*/
void nNavMeshBuilderState::SetCharacterHeight( float height )
{
    nNavMeshBuilder::SetCharacterHeight( height );
}

//------------------------------------------------------------------------------
/**
    GetCharacterHeight
*/
float nNavMeshBuilderState::GetCharacterHeight()
{
    return nNavMeshBuilder::GetCharacterHeight();
}

//------------------------------------------------------------------------------
/**
    SetTriangleMeshesAsObstacles
*/
void nNavMeshBuilderState::SetTriangleMeshesAsObstacles( bool enable )
{
    nNavMeshBuilder::SetTriangleMeshesAsObstacles( enable );
}

//------------------------------------------------------------------------------
/**
    GetTriangleMeshesAsObstacles
*/
bool nNavMeshBuilderState::GetTriangleMeshesAsObstacles()
{
    return nNavMeshBuilder::GetTriangleMeshesAsObstacles();
}

//------------------------------------------------------------------------------
/**
    SetMinHardSlope
*/
void nNavMeshBuilderState::SetMinHardSlope( float angle )
{
    nNavMeshBuilder::SetMinHardSlope( angle * N_PI / 180.f );
}

//------------------------------------------------------------------------------
/**
    GetMinHardSlope
*/
float nNavMeshBuilderState::GetMinHardSlope()
{
    return nNavMeshBuilder::GetMinHardSlope() * 180.f / N_PI;
}

//------------------------------------------------------------------------------
/**
    SetMaxWalkableSlope
*/
void nNavMeshBuilderState::SetMaxWalkableSlope( float angle )
{
    nNavMeshBuilder::SetMaxWalkableSlope( angle * N_PI / 180.f );
}

//------------------------------------------------------------------------------
/**
    GetMaxWalkableSlope
*/
float nNavMeshBuilderState::GetMaxWalkableSlope()
{
    return nNavMeshBuilder::GetMaxWalkableSlope() * 180.f / N_PI;
}

//------------------------------------------------------------------------------
/**
    SetSeaLevel
*/
void nNavMeshBuilderState::SetSeaLevel( float height )
{
    nNavMeshBuilder::SetSeaLevel( height );
}

//------------------------------------------------------------------------------
/**
    GetSeaLevel
*/
float nNavMeshBuilderState::GetSeaLevel()
{
    return nNavMeshBuilder::GetSeaLevel();
}

//------------------------------------------------------------------------------
/**
    Enable/disable the use of terrain holes to hole the navmesh
*/
void nNavMeshBuilderState::SetTerrainHolesEnabled( bool enable )
{
    nNavMeshBuilder::SetTerrainHolesEnabled( enable );
}

//------------------------------------------------------------------------------
/**
    Tell if the terrain holes are used to hole the navmesh
*/
bool nNavMeshBuilderState::GetTerrainHolesEnabled()
{
    return nNavMeshBuilder::GetTerrainHolesEnabled();
}

//------------------------------------------------------------------------------
/**
    Enable/disable the merging of polygons in the generated navmesh
*/
void nNavMeshBuilderState::SetMergePolygons( bool enable )
{
    nNavMeshBuilder::SetMergePolygons( enable );
}

//------------------------------------------------------------------------------
/**
    Tell if polygons in the generated navmesh will be merged
*/
bool nNavMeshBuilderState::GetMergePolygons()
{
    return nNavMeshBuilder::GetMergePolygons();
}

//------------------------------------------------------------------------------
/**
    Enable/disable the fitting of polygons to obstacles' boundary
*/
void nNavMeshBuilderState::SetFitToObstacles( bool enable )
{
    nNavMeshBuilder::SetFitToObstacles( enable );
}

//------------------------------------------------------------------------------
/**
    Tell if polygons are fit to obstacles' boundary
*/
bool nNavMeshBuilderState::GetFitToObstacles()
{
    return nNavMeshBuilder::GetFitToObstacles();
}

//------------------------------------------------------------------------------
/**
    Set the minimum size for obstacles
*/
void nNavMeshBuilderState::SetMinObstacleSize( float size )
{
    nNavMeshBuilder::SetMinObstacleSize( size );
}

//------------------------------------------------------------------------------
/**
    Get the minimum size for obstacles
*/
float nNavMeshBuilderState::GetMinObstacleSize()
{
    return nNavMeshBuilder::GetMinObstacleSize();
}

//------------------------------------------------------------------------------
/**
    Enable/disable the generation of portals
*/
void nNavMeshBuilderState::SetGeneratePortals( bool enable )
{
    nNavMeshBuilder::SetGeneratePortals( enable );
}

//------------------------------------------------------------------------------
/**
    Tell if the generation of portals is enabled
*/
bool nNavMeshBuilderState::GetGeneratePortals()
{
    return nNavMeshBuilder::GetGeneratePortals();
}
