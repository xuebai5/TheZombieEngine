//-----------------------------------------------------------------------------
//  ncnavmesh_main.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnnavmesh.h"
#include "ncnavmesh/ncnavmesh.h"
#include "ncnavmesh/ncnavmeshnode.h"
#include "nnavmesh/nnavmesh.h"
#include "nnavmeshparser/nnavmeshpersist.h"
#include "resource/nresourceserver.h"

#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialindoor.h"
#include "ngeomipmap/ncterraingmmclass.h"
#ifndef __ZOMBIE_EXPORTER__
#include "ncrnswaypoint/ncrnswaypoint.h"
#endif // !__ZOMBIE_EXPORTER__

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncNavMesh,nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNavMesh)
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
/**
   Default constructor
*/
ncNavMesh::ncNavMesh()
{
    this->navMesh = static_cast<nNavMesh*>( nResourceServer::Instance()->NewResource("nnavmesh", NULL, nResource::Other) );
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
ncNavMesh::~ncNavMesh()
{
    if ( this->navMesh.isvalid() )
    {
        this->navMesh->Release();
    }
}

//-----------------------------------------------------------------------------
/**
    Load a navigation mesh from disk, replacing the old one
*/
void ncNavMesh::InitInstance( nObject::InitInstanceMsg initType )
{
    /// @todo ma.garcias Move this navmesh loading to some indoor/walkable brush loader component
    if ( initType != nObject::NewInstance && initType != nObject::ReloadedInstance && !this->GetEntityObject()->IsA("neoutdoor") )
    {
        // Load the nav mesh
        this->Load();

        // A new nav mesh has born! Let's update any waypoint that should be binded to it
        this->GenerateExternalLinks();
    }
}

//-----------------------------------------------------------------------------
/**
    Bind the nav mesh with any waypoints that overlap it
*/
void ncNavMesh::GenerateExternalLinks()
{
#ifndef __ZOMBIE_EXPORTER__
    // Get the bbox that wraps the space represented by the nav mesh
    bbox3 meshBBox;
    if ( this->GetEntityObject()->IsA("neoutdoor") )
    {
        // The outdoor is the first entity to be loaded, so there's no point
        // in wasting time looking for waypoints
        return;
    }
    else if ( this->GetComponent<ncSpatialIndoor>() )
    {
        // This is the nav mesh of an indoor, so update any waypoint that's inside it
        meshBBox = this->GetComponentSafe<ncSpatialIndoor>()->GetBBox();
    }
    else if ( this->GetComponent<ncSpatial>() )
    {
        // This is the nav mesh of a brush, so update any waypoint that's inside it
        meshBBox = this->GetComponentSafe<ncSpatial>()->GetBBox();
    }
    else
    {
        nString msg( "Found a navigation mesh contained in the unsupported entity type '" );
        msg += this->GetEntityObject()->GetClass()->GetName();
        msg += "'. \nExternal links may not be generated for this navigation mesh.";
        n_error( msg.Get() );
        return;
    }

    // Gather all those waypoints that may overlap the nav mesh
    nArray< nEntityObject* > waypoints;
    nSpatialServer::Instance()->GetEntitiesCategory( meshBBox, nSpatialTypes::CAT_WAYPOINTS,
        nSpatialTypes::SPF_OUTDOORS | nSpatialTypes::SPF_CONTAINING |
        nSpatialTypes::SPF_USE_POSITION, waypoints );
    nSpatialServer::Instance()->GetEntitiesCategory( meshBBox, nSpatialTypes::CAT_WAYPOINTS,
        nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_CONTAINING |
        nSpatialTypes::SPF_USE_POSITION, waypoints );

    // Bind the waypoints with the overlapping nav mesh nodes
    // (old external links are removed)
    for ( int i(0); i < waypoints.Size(); ++i )
    {
        ncRnsWaypoint* waypoint( waypoints[i]->GetComponent<ncRnsWaypoint>() );
        if ( waypoint )
        {
            waypoint->GenerateExternalLinks();
        }
    }
#endif // !__ZOMBIE_EXPORTER__
}

//-----------------------------------------------------------------------------
/**
    Load a navigation mesh from disk, replacing the old one

    The navigation mesh is loaded either if it's stored in binary or ascii format.
*/
bool ncNavMesh::Load() const
{
    bool success( false );

    // Clear the current mesh
    this->navMesh->Unload();

    nString path;
    if ( this->FindNavMeshFile( path ) )
    {
        // Load the new mesh
        this->navMesh->SetFilename( path.Get() );
        success = this->navMesh->Load();
    }

    if (success)
    {
        // Insert the nodes in the spatial system
        // Kept for backwards compatibility
        nNavMesh::SpaceType spaceType;
        if ( this->GetEntityObject()->IsA("neindoor") )
        {
            spaceType = nNavMesh::INDOOR;
        }
        else if ( this->GetEntityObject()->IsA("neoutdoor") )
        {
            spaceType = nNavMesh::OUTDOOR;
        }
        else
        {
            spaceType = nNavMesh::BRUSH;
        }
        this->navMesh->InsertIntoSpace( spaceType );
    }

    return success;
}

//-----------------------------------------------------------------------------
/**
    Save a navigation mesh to disk

    The navigation mesh can be saved either in binary or ascii format.
*/
bool ncNavMesh::Save( bool binary, bool compressed ) const
{
    bool success( false );

    // Create a navigation mesh file only if this component contains a navigation mesh
    if ( this->navMesh->IsValid() )
    {
        nString path;
        this->GetNavMeshPath( path, binary, compressed );
        nFileServer2::Instance()->MakePath( path.ExtractDirName().Get() );
        success = nNavMeshPersist::Save( path.Get(), this->navMesh );
    }

    return success;
}

//-----------------------------------------------------------------------------
/**
    Erase the navigation mesh from disk

    The navigation mesh file is deleted either if it's stored in binary or ascii format.
*/
void ncNavMesh::Erase() const
{
    nString path;
    if ( this->FindNavMeshFile( path ) )
    {
        nFileServer2::Instance()->DeleteFile( path.Get() );
    }
}

//-----------------------------------------------------------------------------
/**
    Get the full path of the navigation mesh file
*/
void ncNavMesh::GetNavMeshPath( nString& path, bool binary, bool compressed ) const
{
    path = "level:ai/";
    char buf[50];
    sprintf( buf, "%x", this->GetEntityObject()->GetId() );
    path += buf;
    path += "/navmesh.";
    if ( binary )
    {
        if ( compressed )
        {
            path += nNavMeshPersist::CompressedFileExtension;
        }
        else
        {
            path += nNavMeshPersist::BinaryFileExtension;
        }
    }
    else
    {
        path += nNavMeshPersist::AsciiFileExtension;
    }
}

//-----------------------------------------------------------------------------
/**
    Find the full path of the stored navigation mesh file

    It looks for either the binary or ascii versions, returning true if some of
    them exist, returning false otherwise.
*/
bool ncNavMesh::FindNavMeshFile( nString& path ) const
{
    // Look for a binary compressed file
    this->GetNavMeshPath( path, true, true );
    if ( nFileServer2::Instance()->FileExists(path.Get()) )
    {
        return true;
    }

    // Look for a binary file
    this->GetNavMeshPath( path, true, false ); 
    if ( nFileServer2::Instance()->FileExists(path.Get()) )
    {
        return true;
    }

    // Look for an ascii file
    this->GetNavMeshPath( path, false, false );
    if ( nFileServer2::Instance()->FileExists(path.Get()) )
    {
        return true;
    }

    // There isn't any navigation mesh file
    return false;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
