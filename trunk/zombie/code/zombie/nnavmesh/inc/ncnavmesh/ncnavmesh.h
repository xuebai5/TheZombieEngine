#ifndef NC_NAVMESH_H
#define NC_NAVMESH_H

//------------------------------------------------------------------------------
/**
    @class ncNavMesh
    @ingroup NebulaNavmeshSystem

    Navigation mesh component object.

    This components provides an entity with a navigation mesh, allowing to load
    it from disk.

   (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"

class ncNavMeshClass;
class nNavMesh;

//------------------------------------------------------------------------------
class ncNavMesh : public nComponentObject
{

    NCOMPONENT_DECLARE(ncNavMesh,nComponentObject);

public:
    /// Default constructor
    ncNavMesh();
    /// Destructor
    ~ncNavMesh();
    /// Component initialization
    void InitInstance( nObject::InitInstanceMsg initType );
    /// Get the navigation mesh
    nNavMesh* GetNavMesh() const;
    /// Load the navigation mesh from disk, replacing the old one
    bool Load() const;
    /// Save the navigation mesh to disk
    bool Save( bool binary = true, bool compressed = true ) const;
    /// Erase the navigation mesh from disk
    void Erase() const;
    /// Bind the nav mesh with any waypoints that overlap it
    void GenerateExternalLinks();

private:
    /// Get the full path of the navigation mesh file
    void GetNavMeshPath( nString& path, bool binary, bool compressed ) const;
    /// Find the full path of the stored navigation mesh file
    bool FindNavMeshFile( nString& path ) const;

    /// The navigation mesh
    nRef<nNavMesh> navMesh;

};

//-----------------------------------------------------------------------------
/**
    Get the navigation mesh
*/
inline
nNavMesh* ncNavMesh::GetNavMesh() const
{
    return this->navMesh;
}

#endif
