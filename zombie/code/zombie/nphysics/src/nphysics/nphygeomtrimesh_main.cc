//-----------------------------------------------------------------------------
//  nphygeomtrimesh_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomtrimesh.h"
#include "nphysics/nphysicsserver.h"

#include "util/nkeyarray.h"

#include "zombieentity/nctransform.h"

#define __CHECK_INDEXES__
//-----------------------------------------------------------------------------

nPhyMeshLoad nPhyGeomTriMesh::meshLoader;

nKeyArray<nPhyGeomTriMesh::MeshData> nPhyGeomTriMesh::meshesData(1,1);

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyGeomTriMesh, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 29-Sep-2004   David Reyes    created
*/
nPhyGeomTriMesh::nPhyGeomTriMesh() : 
    dataID(NoValidID),
    vertexes(0),
    indexes(0),
    numberVertexes(0),
    numberIndexes(0),
    fileName(""),
    setExternalBufferVertexes(true),
    setExternalBufferIndexes(true),
    normals(0)
{
    this->type = TriangleMesh;

    this->Create();

    meshLoader.SetIndexType( nPhyMeshLoad::Index32 );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 29-Sep-2004   David Reyes    created
        - 22-Nov-2004   David Reyes    freeing memory
*/
nPhyGeomTriMesh::~nPhyGeomTriMesh()
{
    if( this->dataID != NoValidID )
    {
        phyDestroyTriMeshData( this->dataID );

        if( !this->setExternalBufferVertexes )
        {
            n_delete_array( this->vertexes  );
            this->vertexes = 0;
        }

        this->indexes = 0;

        this->normals = 0;
    }

    this->ReleaseCounter( nTag( this->fileName ).KeyMap() );
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    @param name name of the new geomtry

    history:
        - 29-Sep-2004   David Reyes    created
*/
void nPhyGeomTriMesh::Create()
{
    this->CreateStructureData();

    n_assert2( this->Id() == NoValidID , "Attempting to create an already created geometry" );

    n_assert2( this->dataID != NoValidID , "There isn't data" );

    this->geomID = phyCreateTriMesh( this->dataID );

    n_assert2( this->Id() != NoValidID , "Failing to create the geometry" );

#ifndef NGAME
    this->DrawShape(true);
#endif

    nPhysicsGeom::Create();
}


//-----------------------------------------------------------------------------
/**
    Creates the structure to store the triangle mesh data

    history:
        - 29-Sep-2004   David Reyes    created
*/
void nPhyGeomTriMesh::CreateStructureData()
{
    n_assert2( this->dataID == NoValidID , "Attempting to create an already created data" ); 

    this->dataID = phyCreateTriMeshData();

    n_assert2( this->dataID != NoValidID , "Failing to create the trimesh data structure" ); 
}


//-----------------------------------------------------------------------------
/**
    Create Space for vertexes

    @param number number of vertexes

    history:
        - 29-Sep-2004   David Reyes    created
*/
void nPhyGeomTriMesh::CreateSpaceVertexs( int number )
{
    n_assert2( !this->vertexes , "Error there's already space for vertexes" );

    this->vertexes = n_new_array( phyreal, number * 3);

    n_assert2( this->vertexes , "Not enough memory" );

    this->numberVertexes = number;
}

//-----------------------------------------------------------------------------
/**
    Create Space for indexes

    @param number number of indexes

    history:
        - 29-Sep-2004   David Reyes    created
*/
void nPhyGeomTriMesh::CreateSpaceTriangleIndexes( int number )
{
    n_assert2( !this->indexes , "Error there's already space for indexes" );

    this->indexes = n_new_array( int, number );

    n_assert2( this->indexes , "Not enough memory" );

    this->numberIndexes = number;
}

//-----------------------------------------------------------------------------
/**
    Create Space for normals

    @param number number of normals

    history:
        - 02-Feb-2006   David Reyes    created
*/
void nPhyGeomTriMesh::CreateSpaceNormals( int number )
{
    n_assert2( !this->normals , "Error there's already space for normals" );

    this->normals = n_new_array( phyreal, number );

    n_assert2( this->normals , "Not enough memory" );
}

//-----------------------------------------------------------------------------
/**
    Gets the vertex by index

    @param index index to the vertex
    @return vertex to the corresponding index

    history:
        - 29-Sep-2004   David Reyes    created
*/
phyreal* nPhyGeomTriMesh::GetVertex( int index )
{
    n_assert2( this->vertexes , "There isn't vertexes" );
    
    n_assert2( index < this->numberVertexes , "Index out of bounds" );

    return &this->vertexes[ index * 3 ];

}

//-----------------------------------------------------------------------------
/**
    Gets the triangle index by index

    @param index index to the triangle indexes
    @return pointer to the first index of three

    history:
        - 29-Sep-2004   David Reyes    created
*/
int* nPhyGeomTriMesh::GetTriangleIndex( int index )
{
    n_assert2( this->indexes , "There isn't indexes" );

    n_assert2( index < this->numberIndexes  , "Index out of bounds" );
    
    return &this->indexes[ index * 3 ];
}

//-----------------------------------------------------------------------------
/**
    Updates the triangle mesh

    history:
        - 29-Sep-2004   David Reyes    created
*/
void nPhyGeomTriMesh::Update()
{    
    phyDataBuildTriMesh( 
        this->dataID,
        this->vertexes,
        sizeof( phyreal )* 3,
        this->numberVertexes,
        this->indexes,
        this->numberIndexes,
        sizeof( int ),
        this->normals );

    phyTriMeshData( this->Id(), this->dataID );
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the geometry

    @param server   graphics server

    history:
        - 29-Sep-2004   David Reyes    created
*/
void nPhyGeomTriMesh::Draw( nGfxServer2* server )
{
    nPhysicsGeom::Draw( server );
 
    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyTriMesh) )
        return;

    if( !this->numberIndexes )
        return;

    if( this->DrawShape() )
    {
            
        vector3 vertexes[4];

        vector3 position;

        this->GetAbsolutePosition( position );

        quaternion orien;

        this->GetAbsoluteOrientation( orien );

        matrix33 orientation;
        
        orientation = orien;

        server->BeginLines();

        int Size( numberIndexes / 3 );

        vector4 color(phy_color_trianglemesh);

        if( !this->IsEnabled() )
        {
            color = phy_color_trianglemesh_disabled;
        }

        for( int i(0); i < Size; ++i )
        {
            int* index(this->GetTriangleIndex( i ));

            

            for( int inner(0); inner < 3; ++inner )
            {
                int indexnum(*(index+ inner));
                phyreal* coord(this->GetVertex( indexnum ));

                vertexes[inner].x = coord[0];
                vertexes[inner].y = coord[1];
                vertexes[inner].z = coord[2];                 

                vertexes[ inner ] = orientation * vertexes[inner];

                vertexes[inner].x += position.x;
                vertexes[inner].y += position.y;
                vertexes[inner].z += position.z;                 

            }

            vertexes[3] = vertexes[0];

            server->DrawLines3d( vertexes, 4, color );

            if( this->normals )
            {
                if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyNormals) )
                {
                    continue;
                }
                vector3 Normal(0,0,0);

                for( int inner(1); inner < 3; ++inner )
                {
                    vertexes[0] += vertexes[inner];
                }
 
                Normal.x = this->normals[ i*3 ];
                Normal.y = this->normals[ i*3 + 1 ];
                Normal.z = this->normals[ i*3 + 2 ];

                vertexes[0] = vertexes[0] / phyreal(3);

                vertexes[1] = vertexes[ 0 ] + (Normal*phy_normals_lenght);

                server->DrawLines3d( vertexes, 2, phy_color_normals );
            }
        }

        server->EndLines();
    }

}
#endif

//-----------------------------------------------------------------------------
/**
    Loads a description of a trimesh

    @param filename   file where the data is

    history:
        - 05-Oct-2004   David Reyes    created
*/
void nPhyGeomTriMesh::SetFile( const nString& filename )
{
    this->GetMeshData( filename );

    this->Update();

    this->fileName = filename;
}

//-----------------------------------------------------------------------------
/**
    Releases data

    history:
        - 06-Oct-2004   David Reyes    created
*/
void nPhyGeomTriMesh::ReleaseData()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Returns the name of the last file loaded.

    history:
        - 06-Oct-2004   David Reyes    created
*/
const nString& nPhyGeomTriMesh::GetFileName() const
{
    return fileName;
}

//-----------------------------------------------------------------------------
/**
    Sets a external buffer for vertexes.

    @param number   number of vertexes in the buffer
    @param buffer   data

    history:
        - 08-Oct-2004   David Reyes    created
*/
void nPhyGeomTriMesh::SetBufferVertexes( int number, phyreal* buffer )
{
    n_assert2( number >= 0 , "Error negative numbers aren't accepted" );
    
    this->vertexes = buffer;
    
    this->numberVertexes = number;

    this->setExternalBufferVertexes = true;
}

//-----------------------------------------------------------------------------
/**
    Sets a external buffer for vertexes.

    @param number   number of vertexes in the buffer
    @param buffer   data

    history:
        - 08-Oct-2004   David Reyes    created
*/
void nPhyGeomTriMesh::SetBufferIndexes( int number, int* buffer )
{
    n_assert2( number >= 0 , "Error negative numbers aren't accepted" );
    
    this->indexes = buffer;
    
    this->numberIndexes = number;

    this->setExternalBufferIndexes = true;
}

//------------------------------------------------------------------------------
/**
	Scales the geometry.

    @param factor scale factor
    
    history:
     - 12-May-2005   David Reyes    created
*/
void nPhyGeomTriMesh::Scale( const phyreal factor )
{
    if( factor == phyreal(1) )
    {
        return;
    }

    if( this->setExternalBufferVertexes )
    {
        this->setExternalBufferVertexes = false;

        phyreal *vertexBuffer( this->vertexes );

        this->vertexes = 0;

        this->CreateSpaceVertexs( this->GetNumberVertexes() );

        memcpy( this->vertexes, vertexBuffer, this->GetNumberVertexes() * 3 * sizeof(phyreal));
    }

    phyreal *vertex(this->vertexes);

    for( int index(0); index < this->GetNumberVertexes(); ++index )
    {
        (*vertex++) *= factor;
        (*vertex++) *= factor;
        (*vertex++) *= factor;
    }
}

//------------------------------------------------------------------------------
/**
	Gets the mesh data.

    @param filename filename containing the mesh data
    @return if any problem
    
    history:
     - 23-Feb-2006   David Reyes    created
*/
const bool nPhyGeomTriMesh::GetMeshData( const nString& filename )
{
    MeshData Data;

    if( !meshesData.Find( nTag( filename ).KeyMap(), Data ) )
    {
        // we don't have it

        this->ReleaseData();

        meshLoader.SetFilename( filename.Get() );

        meshLoader.Open( nPhysicsServer::Instance()->GetFileServer() );

        //this->CreateSpaceVertexs( meshLoader.GetNumVertices() );
        Data.vertexes = n_new_array( phyreal, meshLoader.GetNumVertices() * 3);
        Data.numberVertexes = meshLoader.GetNumVertices();

        //this->CreateSpaceTriangleIndexes( meshLoader.GetNumIndices() );
        Data.indexes = n_new_array( int, meshLoader.GetNumIndices() );
        Data.numberIndexes = meshLoader.GetNumIndices();

        phyreal *buffer(0);

        if( meshLoader.IsNormalsPresent() )
        {
            //this->CreateSpaceNormals( meshLoader.GetNumIndices() );
            Data.normals = n_new_array( phyreal, meshLoader.GetNumIndices() );

            buffer = n_new_array( phyreal, meshLoader.GetNumVertices() * 6 );

            meshLoader.ReadVertices( buffer, sizeof( phyreal ) * Data.numberVertexes  * 6 );
        }
        else
        {
            meshLoader.ReadVertices( Data.vertexes, sizeof( phyreal ) * Data.numberVertexes  * 3 );
        }

        meshLoader.ReadIndices( Data.indexes, sizeof( int ) * Data.numberIndexes );

        meshLoader.Close();

        if( meshLoader.IsNormalsPresent() )
        {
            // pre-processing data
            for( int index(0); index < Data.numberVertexes; ++index )
            {
                Data.vertexes[ index * 3 ] = buffer[ index * 6 ];
                Data.vertexes[ index * 3 + 1 ] = buffer[ index * 6 + 1 ];
                Data.vertexes[ index * 3 + 2 ] = buffer[ index * 6 + 2 ];
            }

            for( int index(0); index < Data.numberIndexes; index+=3 )
            {
                int iindex(Data.indexes[ index ]);

                Data.normals[ index ] = buffer[ iindex * 6 +3 ];
                Data.normals[ index + 1] = buffer[ iindex * 6 + 4];
                Data.normals[ index + 2] = buffer[ iindex * 6 + 5];
            }

            n_delete_array( buffer );
        }

        meshesData.Add( nTag( filename ).KeyMap(), Data );
    }

    this->vertexes = Data.vertexes;
    this->numberVertexes = Data.numberVertexes;
    this->indexes = Data.indexes;
    this->numberIndexes = Data.numberIndexes;
    this->normals = Data.normals;

    ++meshesData.GetElement( nTag( filename ).KeyMap() ).counter;

    // NOTE: Temporary, has to be removed
    this->fixIndexes( filename );

    return true;
}

//------------------------------------------------------------------------------
/**
	Releases the data of all meshes.

    @param filename filename containing the mesh data
    @return if any problem
    
    history:
     - 23-Feb-2006   David Reyes    created
*/
void nPhyGeomTriMesh::ReleaseMeshesData()
{
    for( int index(0); index < meshesData.Size(); ++index )
    {
        n_delete_array(meshesData.GetElementAt( index ).vertexes);
        n_delete_array(meshesData.GetElementAt( index ).indexes);
        n_delete_array(meshesData.GetElementAt( index ).normals);
    }

    meshesData.Clear();
}

//------------------------------------------------------------------------------
/**
	Releases a counter from  the mesh data.

    history:
     - 23-Feb-2006   David Reyes    created
*/
void nPhyGeomTriMesh::ReleaseCounter( const int key )
{
    if( !key )
    {
        return;
    }

    --meshesData.GetElement( key ).counter;

    if( !meshesData.GetElement( key ).counter )
    {
        n_delete_array(meshesData.GetElement( key ).vertexes);
        n_delete_array(meshesData.GetElement( key ).indexes);
        n_delete_array(meshesData.GetElement( key ).normals);

        meshesData.Rem( key );
    }
}

//------------------------------------------------------------------------------
/**
	Fixes indexes data of a mesh (NOTE: Temporary patch).

    history:
     - 31-Jul-2006   David Reyes    created
*/
void nPhyGeomTriMesh::fixIndexes( const nString& filename ) 
{
    bool reported(false);

    for( int i(0); i < this->numberIndexes; i+=3 )
    {
        if( this->indexes[i] == this->indexes[i+1] ||
            this->indexes[i] == this->indexes[i+2] ||
            this->indexes[i+1] == this->indexes[i+2] ) 
        {
            if( !reported )
            {
                NLOG( physicsLog , (1, "Indexes mesh corruption on: %s.",filename.Get() ) );                
                reported = true;
            }

            // removing triangle
            this->numberIndexes -= 3;
            memcpy( &this->indexes[i], &this->indexes[i+3], sizeof(int) * ( this->numberIndexes - i));
            i-=3;
        }
    }

}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
