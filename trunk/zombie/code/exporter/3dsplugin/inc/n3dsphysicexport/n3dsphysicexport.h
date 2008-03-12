#ifndef N3DSPHYSICEXPORT_H
#define N3DSPHYSICEXPORT_H
//------------------------------------------------------------------------------
/**
    @class n3dsPhysicExport
    @ingroup n3dsMaxExporterKernel
    
    Is the object that export all physics object to nebula engine.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/narray.h"
#include "util/nstring.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class INode;
class nRoot;
class nPhysicsWorld;
class nPhyGeomSpace;
class nPhysicsGeom;
class nEntityClass;
class nEntityObject;
class ncPhysicsObj;
class n3dsSkeleton;
class nObjectInstancer;
class nIndoorBuilder;
class bbox3;

//------------------------------------------------------------------------------
class n3dsPhysicExport
{
public:
    /// Constructor
    n3dsPhysicExport();
    /// Destructor
    ~n3dsPhysicExport();
    /// Init and exportInfo();
    void Init();
    /// add a max node to physics
    void CreatePhysic( INode * const node );

    /// get parent class for a brush
    bool GetBrushClass( nString & parentName );

    /// fill the brush class with physic information
    void ExportBrushClass( nEntityClass * entityClass );
    /// fill the brush object with physic information
    void ExportBrushObject( nEntityObject * entityObject );
    /// fill a object array with the physic objects
    void ExportIndoorObjects(nEntityClass* indoorClass, nIndoorBuilder * builder );

    /// fill the brush class with physic information
    void ExportSkeletonClass( nEntityClass * entityClass, n3dsSkeleton* pSkeleton );
    /// fill the brush class with physic information
    void ExportSkeletonObject( nEntityObject * entityObject );

private:
    /// geometry of the collision object
    enum CollisionType{
        T_BOX = 1,
        T_SPHERE,
        T_CYLINDER,
        T_TRIMESH,
        T_REALCYLINDER,
        T_COMPOSITE,
        T_LAST
    };

    /// get the collision type of a max node
    static CollisionType GetCollisionType(INode * const node );
    /// set the attribute flag of geometry
    void SetGeomAttributes( INode * const node,  nPhysicsGeom * const geom) const;
    /// set collision attributes of object
    void SetCollisionAttributes( INode * const node, ncPhysicsObj * const obj ) const;
    /// check the name of a physic category
    TSTR CheckPhysicCategoryName( TSTR name ) const;
    /// check that all geometries have the same physic category
    void CheckAllPhysicCategories( const nArray< INode * > * const geometries ) const;

    enum BrushType{
        BRUSH_NONE,
        BRUSH_SIMPLE = 1,
        BRUSH_COMPOSITE,
        BRUSH_LAST
    };

    /// get the brush type to export
    BrushType GetBrushType() const;

    // create a physic geometry using a max node
    static void CreateGeometries( INode * const node, const nString & physicPath, nArray<nPhysicsGeom*> & geoms );
    /// create a physic geometry box using a max node
    static nPhysicsGeom* CreateGeomBox( INode * const node );
    /// create a physic geometry sphere using a max node
    static nPhysicsGeom* CreateGeomSphere( INode * const node );
    /// create a physic geometry capsule using a max node
    static nPhysicsGeom* CreateGeomCapsule( INode * const node );
    /// create a physic geometry trimesh using a max node
    static nPhysicsGeom* CreateGeomTrimesh( INode * const node, const nString & physicPath, bool isSimple = false );
    /// create the geometries than aproximate a real cylinder
    static void CreateGeomCylinder( INode * const node, const nString & physicPath, nArray<nPhysicsGeom*> & geoms );

    /// export a simple brush object
    void ExportSimpleBrushObject( nEntityObject * entityObject );
    /// export a composite brush object
    void ExportCompositeBrushObject( nEntityObject * entityObject );

    /// export a composite character object
    void ExportCompositeSkeletonObject( nEntityObject * entityObject );
    /// find geometry-joint offset
    vector3 FindOffset( INode* iNode, int boneIdx );

    /// create a nePhySimple object with the node information
    nEntityObject * CreatePhySimpleObject( INode * const node );
    /// create a nePhyComposite object with the node information
    nEntityObject * CreatePhyCompositeObject( INode * const node );
    /// create a nePhyComposite object with a cylinder node
    nEntityObject * CreatePhyCylinderObject( INode * const node );

    /// export a physic chunk
    template<class TYPE>
    void ExportPhysicChunk( nEntityClass * entityClass, nEntityObject * entityObject, TYPE * phyObject );

    /// array of max nodes containing physic spaces
    nArray<INode*> physicSpaces;
    /// array of max nodes containing collision objects
    nArray<INode*> collisionObjects;
    /// node of physics objects
    nRoot * physicRoot;
    /// node of physics objects of collisions
    nRoot * physicCollision;
    /// physics world node
    nPhysicsWorld * physicWorld;
    bool exportSpaces;

    nEntityClass * entityClass;
    nString physicPath;

    /// skeleton
    n3dsSkeleton* skeleton;
};

//------------------------------------------------------------------------------
/**
    @param entityObject entity object instance to export
    @param phyObject the physic component
*/
template<class TYPE>
void
n3dsPhysicExport::ExportPhysicChunk( nEntityClass * entityClass, nEntityObject * entityObject, TYPE * phyObject )
{
    bool valid = ( entityObject != 0 && phyObject != 0 && entityClass != 0 );
    n_assert( valid );
    if( ! valid )
    {
        return;
    }

    // save chunk
    ncLoaderClass * loaderClass = entityClass->GetComponent<ncLoaderClass>();
    n_assert( loaderClass );
    if( loaderClass )
    {
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);
        if( ps )
        {
            // change command to 'THIS'
            nCmd * cmd = ps->GetCmd( entityObject, 'THIS' );
            n_assert( cmd );
            if( cmd )
            {
                nString path;
                path = loaderClass->GetResourceFile();
                path.Append( "/physics/chunk.n2" );

                nFileServer2::Instance()->MakePath( path.ExtractDirName().Get() );

                if( ps->BeginObjectWithCmd( entityObject, cmd, path.Get() ) ) 
                {
                    phyObject->SaveChunk( ps );
                    ps->EndObject( true );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
#endif//N3DSPHYSICEXPORT_H
