#ifndef N_PHYGEOMTRIMESH_H
#define N_PHYGEOMTRIMESH_H
//-----------------------------------------------------------------------------
/**
    @class nPhyGeomTriMesh
    @ingroup NebulaPhysicsSystem
    @brief A triangle mesh (TriMesh) represents an arbitrary collection of triangles

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics TriMesh Geometry
    
    @cppclass nPhyGeomTriMesh
    
    @superclass nPhysicsGeom

    @classinfo Represents an arbitrary collection of triangles.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"
#include "nphysics/nphymeshload.h"

//-----------------------------------------------------------------------------

class nPhyGeomTriMesh : public nPhysicsGeom 
{
public:
    /// constructor
    nPhyGeomTriMesh();

    /// destructor
    virtual ~nPhyGeomTriMesh();

    /// creates the geometry
    void Create();

    /// gets the vertex by index
    phyreal* GetVertex( int index );

    /// gets the triangle index by index
    int* GetTriangleIndex( int index );

    /// updates the triangle mesh
    void Update();

#ifndef NGAME
    /// draws the geometry
    void Draw( nGfxServer2* server );
#endif

    /// returns the name of the last file loaded
    const nString& GetFileName() const;

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// get number of vertexes
    int GetNumberVertexes() const;

    /// get number of indexes
    int GetNumberIndexes() const;

    /// sets a external buffer for vertexes
    void SetBufferVertexes( int number, phyreal* buffer );

    /// returns the buffer address for vertexes
    const phyreal* GetBufferVertexes() const;

    /// sets a external buffer for indexes
    void SetBufferIndexes( int number, int* buffer );

    /// returns the buffer address for indexes
    const int* GetBufferIndexes() const;

    /// returns the number of contacts
    int GetNumContacts() const;

    /// begin:scripting
    
    /// loads a description of a trimesh
    void SetFile(const nString&);

    /// end:scripting

    /// scales the geometry
    void Scale( const phyreal factor );

    /// releases the data of all meshes
    static void ReleaseMeshesData();

public:

    /// releases a counter from  the mesh data
    void ReleaseCounter( const int key );

    /// create Space for vertexes
    void CreateSpaceVertexs( int number );

    /// create Space for triangles
    void CreateSpaceTriangleIndexes( int number );

    /// create Space for normals
    void CreateSpaceNormals( int number );

    /// creates the structure to store the triangle mesh data
    void CreateStructureData();

    /// stores ID os the data structure
    trimeshdata dataID;

    /// stores the vertexes
    phyreal* vertexes;

    /// stores the normals
    phyreal* normals;

    /// stores the number of vertexes
    int numberVertexes;

    /// stores the indexes to create triangles
    int* indexes;

    /// stores the number of indexes
    int numberIndexes;

    /// composite object to load the meshes
    static nPhyMeshLoad meshLoader;

    /// releases data
    void ReleaseData();

    /// last file loaded
    nString fileName;

    /// stores if the vertexes buffer is external
    bool setExternalBufferVertexes : 1;
    
    /// stores if the indexes buffer is external
    bool setExternalBufferIndexes : 1;

    /// type to store mesh data
    struct MeshData {
        phyreal* vertexes;

        phyreal* normals;

        int numberVertexes;

        int* indexes;

        int numberIndexes;

        int counter;

        MeshData() : vertexes(0), normals(0), numberVertexes(0),indexes(0),numberIndexes(0),counter(0) {}
    };

    /// stores mesh data
    static nKeyArray< MeshData > meshesData;

    /// gets the mesh data
    const bool GetMeshData( const nString& filename );

    /// fixes indexes data of a mesh (NOTE: Temporary patch)
    void fixIndexes( const nString& filename );
};

//-----------------------------------------------------------------------------
/**
    Get number of vertexes.

    @return number of vertexes

    history:
        - 10-Oct-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
int nPhyGeomTriMesh::GetNumberVertexes() const
{
    return this->numberVertexes;
}

//-----------------------------------------------------------------------------
/**
    Get number of indexes.

    @return number of indexes

    history:
        - 10-Oct-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
int nPhyGeomTriMesh::GetNumberIndexes() const
{
    return this->numberIndexes;
}

//-----------------------------------------------------------------------------
/**
    Returns the buffer address for vertexes.

    @return buffer address

    history:
        - 08-Oct-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
const phyreal* nPhyGeomTriMesh::GetBufferVertexes() const
{
    return this->vertexes;
}

//-----------------------------------------------------------------------------
/**
    Returns the buffer address for indexes.

    @return buffer address

    history:
        - 08-Oct-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
const int* nPhyGeomTriMesh::GetBufferIndexes() const
{
    return this->indexes;
}

//-----------------------------------------------------------------------------
/**
    Returns the number of contacts.

    @return num of contacts

    history:
        - 08-Oct-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
int nPhyGeomTriMesh::GetNumContacts() const
{
    return 6;
}

#endif
