//------------------------------------------------------------------------------
//  n3dsphysicexport.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchn3dsmaxexport.h"

#pragma warning( push, 3 )
#include "Max.h"
#include "simpobj.h"
#include "iparamb2.h"
#include "icustattribcontainer.h"
#include "custattrib.h"
#pragma warning( pop )


#include "n3dsphysicexport/n3dsphysicexport.h"
#include "n3dsexporters/n3dscustomattributes.h"
#include "n3dsexporters/n3dssystemcoordinates.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsscenelist.h"
#include "n3dsexporters/n3dslog.h"

#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphygeombox.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/nphygeomcylinder.h"
#include "nphysics/nphygeomtrimesh.h"

//------------------------------------------------------------------------------
const double max_error(0.0001);
const float min_flt_persistable(0.00001f);

//------------------------------------------------------------------------------
static struct PhysicCategory {
    const char * name;
    nPhysicsGeom::Category type;
} physicCategories [] = {
    { "STATIC", nPhysicsGeom::Static },
    { "RAMP", nPhysicsGeom::Ramp },
    { "STAIRS", nPhysicsGeom::Stairs },
    // default value for category is Static, for not founded categories
    { 0, nPhysicsGeom::Static }
};

//------------------------------------------------------------------------------
/**
*/
n3dsPhysicExport::n3dsPhysicExport():
    exportSpaces( true ),
    entityClass( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
n3dsPhysicExport::~n3dsPhysicExport()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
*/
void
n3dsPhysicExport::CreatePhysic( INode * const node )
{    
    int value = -1;
    node->GetUserPropInt("physicType", value );
    switch( value )
    {
    case 0:
        this->physicSpaces.PushBack( node );
        break;

    case 1:
        if ( T_LAST != GetCollisionType(node) )
        {
            this->collisionObjects.PushBack( node );
        }
        break;

    default:
        n_assert2_always( "Incorrect type of physic object" );
    }
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @returns the collision type
*/
n3dsPhysicExport::CollisionType
n3dsPhysicExport::GetCollisionType( INode *const node)
{
    Object * obj = node->GetObjectRef();
    Class_ID classid = obj->ClassID();

    TSTR name;
    node->GetUserPropString( "physicGeom", name );

    if( classid == Class_ID( BOXOBJ_CLASS_ID, 0 ) )
    {
        if( 0 != strcmp( "box", name ) )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Bad Box, physic object \"%s\" " , node->GetName() ) );
            return T_LAST;
        }
        return T_BOX;
    }
    else if( classid == Class_ID( SPHERE_CLASS_ID, 0 ) )
    {
        if( 0 != strcmp( "sphere", name ) )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Bad Sphere, physic object \"%s\" " , node->GetName() ) );
            return T_LAST;
        }
        return T_SPHERE;
    }
    else if( classid == Class_ID( DUMMY_CLASS_ID, 0 ) )
    {
        if( 0 != strcmp( "composite", name ) )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Bad Composite, physic object \"%s\" " , node->GetName() ) );
            return T_LAST;
        }
        return T_COMPOSITE;
    }
    else if( classid == Class_ID( CYLINDER_CLASS_ID, 0 ) )
    {
        if( 0 != strcmp( "realcylinder", name ) )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Bad Cylinder, physic object \"%s\" " , node->GetName() ) );
            return T_LAST;
        }
        return T_REALCYLINDER;
    }
    else if( 0 == strcmp( "cylinder", name ) )
    {
        return T_CYLINDER;
    }
    else if( 0 == strcmp( "mesh", name ) )
    {
        return T_TRIMESH;
    }
    else
    {
        N3DSERROR( physicExport , ( 0 , "ERROR: Incorrect type of physic object \"%s\" " , node->GetName() ) );
        //n_assert2_always( "Error in Collision Type" ); 
        return T_LAST;
    }
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @param phyGeom physics geometry
*/
void
n3dsPhysicExport::SetGeomAttributes( INode * const node, nPhysicsGeom * const phyGeom ) const
{
    int wallFlag = 0, groundFlag = 0, ceilingFlag = 0, walkableFlag = 0;

    n3dsCustomAttributes::GetParamInt( node, "CollParams", "wall", wallFlag );
    if( wallFlag ){
        phyGeom->AddAttributes( nPhysicsGeom::wall );
    }

    n3dsCustomAttributes::GetParamInt( node, "CollParams", "ground", groundFlag );
    if( groundFlag ){
        phyGeom->AddAttributes( nPhysicsGeom::ground );
    }

    n3dsCustomAttributes::GetParamInt( node, "CollParams", "ceiling", ceilingFlag );
    if( ceilingFlag ){
        phyGeom->AddAttributes( nPhysicsGeom::ceiling );
    }

    n3dsCustomAttributes::GetParamInt( node, "CollParams", "walkable", walkableFlag );
    if( walkableFlag ){
        phyGeom->AddAttributes( nPhysicsGeom::walkable );
    }

    TSTR material;

    if( n3dsCustomAttributes::GetParamString( node, "CollParams", "material", material ) )
    {
        phyGeom->SetGameMaterial( material.data() );
    }
}

//------------------------------------------------------------------------------
/**
    @param name to check
    @returns the name fixed
*/
TSTR
n3dsPhysicExport::CheckPhysicCategoryName( TSTR name ) const
{
    if( name.isNull() )
    {
        name = _T( "STATIC" );
    }

    int i = 0;
    while( physicCategories[ i ].name && ( 0 != stricmp( physicCategories[ i ].name, name ) ) )
    {
        ++i;
    }

    if( 0 == physicCategories[ i ].name )
    {
        N3DSERROR( physicExport , ( 0 , "ERROR: invalid physic category %s", name) );
        name = _T( "STATIC" );
    }

    return name;
}

//------------------------------------------------------------------------------
/**
    @param geometries 3dstudio objects to check
*/
void
n3dsPhysicExport::CheckAllPhysicCategories( const nArray< INode * > * const geometries ) const
{
    // check that all geometries category are the same
    TSTR firstname, name;
    (*geometries)[ 0 ]->GetUserPropString( "physicCategory", firstname );
    firstname = this->CheckPhysicCategoryName( firstname );

    for( int i=1; i < geometries->Size() ; ++i )
    {
        (*geometries)[ i ]->GetUserPropString( "physicCategory", name );
        name = this->CheckPhysicCategoryName( name );
        
        if( 0 != stricmp( firstname, name ) )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: physic category is not the same in %s for %s", 
                (*geometries)[i]->GetName(), this->entityClass->nClass::GetName() ) );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param node 3dstudio object to get from collision attributes
    @param obj physic object to set in collision attributes
*/
void
n3dsPhysicExport::SetCollisionAttributes( INode * const node, ncPhysicsObj * const obj ) const
{
    TSTR name;
    node->GetUserPropString( "physicCategory", name );

    int i = 0;
    while( physicCategories[ i ].name && ( 0 != stricmp( physicCategories[ i ].name, name ) ) )
    {
        ++i;
    }

    obj->SetCategories( physicCategories[ i ].type );
    obj->SetCollidesWith( 0 );
}

//------------------------------------------------------------------------------
/**
    @param node the max node used to create the geometry
    @param physicPath physic path where trimesh are saved, if needed
    @param geometries list of geometries where created geometry is inserted
*/
void
n3dsPhysicExport::CreateGeometries( INode * const node, const nString & physicPath, nArray<nPhysicsGeom*> & geometries )
{
    nPhysicsGeom * geom = 0;
    NLOG( physicExport , ( 4 , "CreateGeometries \"%s\" " , node->GetName() ) );

    switch( n3dsPhysicExport::GetCollisionType( node ) )
    {
    case T_BOX:
        geom = n3dsPhysicExport::CreateGeomBox( node );
        break;

    case T_SPHERE:
        geom = n3dsPhysicExport::CreateGeomSphere( node );
        break;

    case T_CYLINDER:
        geom = n3dsPhysicExport::CreateGeomCapsule( node );
        break;

    case T_TRIMESH:
        geom = n3dsPhysicExport::CreateGeomTrimesh( node, physicPath );
        break;

    case T_REALCYLINDER:
        n3dsPhysicExport::CreateGeomCylinder( node, physicPath, geometries );
        break;

    default:
        n_assert_always();
    }

    if( geom )
    {
        geometries.Append( geom );
    } else
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: Can't Create physics %s", node->GetName() ) );
    }
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @returns the physics geometry object
*/
nPhysicsGeom* 
n3dsPhysicExport::CreateGeomBox( INode * const node )
{
    NLOG( physicExport , ( 4 , "CreateGeomBox \"%s\" " , node->GetName() ) );
    SimpleObject* sobj = static_cast<SimpleObject*>( node->GetObjectRef() );
    IParamBlock* pb = sobj->pblock;

    n3dsSystemCoordinates * sysCoord = n3dsExportServer::Instance()->GetSystemCoordinates();

    // get the parameters of the box
    float length=1.0, height=1.0, width=1.0;
    transform44 tr = sysCoord->MaxToNebulaTransform( node->GetNodeTM(0) );

    // check that scale is correct
    if( ! sysCoord->HaveIndependentScale( node->GetNodeTM(0) ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The object scale isn't in the Local Transform\n" ) );
        return 0;
    }

    // look for parameters
    for( int i=0; i<pb->NumParams() ; ++i )
    {
        TSTR name = sobj->GetParameterName( i );

        if( 0 == strcmp(name, "Length" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Box" );
            length = abs( sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) ) );
        }
        else if( 0 == strcmp(name, "Height" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Box" );
            height = abs( sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) ) );
        }
        else if( 0 == strcmp(name, "Width" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Box" );
            width = abs( sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) ) );
        }
    }

    // get the scale
    vector3 scale = tr.getscale();

    // check the scale
    if( !( scale.x > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.x <= 0.0" ) );
        return 0;
    }
    if( !( scale.y > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.y <= 0.0" ) );
        return 0;
    }
    if( !( scale.z > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.z <= 0.0" ) );
        return 0;
    }

    // adjust scale
    width = width * scale.x;
    height = height * scale.y;
    length = length * scale.z;


    bool validLength = length >= min_flt_persistable;
    bool validHeight = height >= min_flt_persistable;
    bool validWidth  = width >= min_flt_persistable;
    bool planeOrBoxInLength = validLength &&(validHeight || validWidth );
    bool planeInWidthAndEhight = validHeight && validWidth;

    if (! ( planeOrBoxInLength ||  planeInWidthAndEhight ))
    {
        N3DSWARNCOND( physicExport, !validLength, ( 0 , "WARNING: Can't Create Box Geometry with lenght * scale <= 0.0" ) );
        N3DSWARNCOND( physicExport, !validHeight, ( 0 , "WARNING: Can't Create Box Geometry with height * scale <= 0.0" ) );
        N3DSWARNCOND( physicExport, !validWidth,  ( 0 , "WARNING: Can't Create Box Geometry with width * scale <= 0.0" ) );
        return 0;
    }

    // the minimun value if the box is a plane
    if (!validLength) { length = min_flt_persistable;}
    if (!validHeight) { height = min_flt_persistable;}
    if (!validWidth)  { width = min_flt_persistable;}



    // create the geometry object
    nKernelServer* ks = nKernelServer::ks;

    nPhyGeomBox * phyGeom = static_cast<nPhyGeomBox*>( ks->New("nphygeombox") );
    phyGeom->AppendCommentFormat("Max Id:    .8X",  node->GetHandle() );
    phyGeom->AppendCommentFormat("Max name: \"%s\"", node->GetName() );
    phyGeom->Enable();
    phyGeom->SetLengths( vector3( width, height, length ) );
    phyGeom->SetPosition( tr.gettranslation() );
    matrix33 m( tr.getquatrotation() );
    vector3 eulerangles = m.to_euler();
    phyGeom->SetOrientation( eulerangles.x, eulerangles.y, eulerangles.z  );

    return phyGeom;
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @returns the physics geometry object
*/
nPhysicsGeom* 
n3dsPhysicExport::CreateGeomSphere( INode * const node )
{
    NLOG( physicExport , ( 4 , "CreateGeomSphere \"%s\" " , node->GetName() ) );
    SimpleObject* sobj = (SimpleObject*)node->GetObjectRef();
    IParamBlock* pb = sobj->pblock;

    n3dsSystemCoordinates * sysCoord = n3dsExportServer::Instance()->GetSystemCoordinates();

    // get the parameters of the box
    float radius=1.0;
    transform44 tr = sysCoord->MaxToNebulaTransform( node->GetNodeTM(0) );
    vector3 scale = tr.getscale();

    // check that scale is correct
    if( (fabs( scale.x - scale.y ) > max_error) || (fabs( scale.x - scale.z ) > max_error) 
        || (fabs( scale.y - scale.z) > max_error) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale in GeomSphere is incorrect. Need to be equal in all axes." ) );
        return 0;
    }

    // look for parameters
    for( int i=0; i<pb->NumParams() ; ++i )
    {
        TSTR name = sobj->GetParameterName( i );
        if( 0 == strcmp(name, "Radius" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Sphere" );
            radius = sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) );
        }
    }


    // check the scale
    if( !( scale.x > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.x <= 0.0" ) );
        return 0;
    }

    // adjust scale
    radius = radius * scale.x;
    
    // check the dimensions
    if( !( radius >= min_flt_persistable ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: Can't Create Sphere Geometry with radius * scale <= 0.0" ) );
        return 0;
    }

    // create the geometry object
    nKernelServer* ks = nKernelServer::ks;

    nPhyGeomSphere * phyGeom = static_cast<nPhyGeomSphere*>( ks->New("nphygeomsphere") );
    phyGeom->AppendCommentFormat("Max Id:    .8X",  node->GetHandle() );
    phyGeom->AppendCommentFormat("Max name: \"%s\"", node->GetName() );
    phyGeom->Enable();
    phyGeom->SetRadius( radius );
    phyGeom->SetPosition( tr.gettranslation() );

    return phyGeom;
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @returns the physics geometry object
*/
nPhysicsGeom* 
n3dsPhysicExport::CreateGeomCapsule( INode * const node )
{
    NLOG( physicExport , ( 4 , "CreateGeomCapsule \"%s\" " , node->GetName() ) );
    SimpleObject* sobj = (SimpleObject*)node->GetObjectRef();
    IParamBlock* pb = sobj->pblock;

    n3dsSystemCoordinates * sysCoord = n3dsExportServer::Instance()->GetSystemCoordinates();

    // get the parameters of the Capsule
    float radius=1.0, height=1.0;
    transform44 tr = sysCoord->MaxToNebulaTransform( node->GetNodeTM(0) );

    // check that scale is correct
    if( ! sysCoord->HaveIndependentScale( node->GetNodeTM(0) ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The object scale isn't in the Local Transform" ) );
        return 0;
    }

    vector3 scale = tr.getscale();
    if( (fabs( scale.x - scale.z ) > max_error) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale in GeomCapsule is incorrect. Need to be equal in radius axis." ) );
        return 0;
    }

    // look for parameters
    for( int i=0; i<pb->NumParams() ; ++i )
    {
        TSTR name = sobj->GetParameterName( i );

        if( 0 == strcmp(name, "Radius" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Capsule" );
            radius = sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) );
        }
        else if( 0 == strcmp(name, "Height" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Capsule" );
            height = sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) );
        }
    }

    // check the scale
    if( !( scale.x > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.x <= 0.0" ) );
        return 0;
    }
    if( !( scale.y > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.y <= 0.0" ) );
        return 0;
    }

    // adjust scale
    radius = radius * scale.x;
    height = height * scale.y;

    // check the dimensions
    if( !( radius >= min_flt_persistable ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: Can't Create Capsule Geometry with radius * scale <= 0.0" ) );
        return 0;
    }
    if( !( height >= min_flt_persistable ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: Can't Create Capsule Geometry with height * scale <= 0.0" ) );
        return 0;
    }

    // create the geometry object
    nKernelServer* ks = nKernelServer::ks;

    nPhyGeomCylinder * phyGeom = static_cast<nPhyGeomCylinder*>( ks->New("nphygeomcylinder" ) );
    phyGeom->AppendCommentFormat("Max Id:    .8X",  node->GetHandle() );
    phyGeom->AppendCommentFormat("Max name: \"%s\"", node->GetName() );
    phyGeom->Enable();
    phyGeom->SetRadius( radius );
    if( (height - radius*2.0f) < max_error )
    {
        height = static_cast<float>( radius*2.0f + max_error );
        N3DSWARN( physicExport , 
            ( 0 , "Capsule Height less that 2*radius in %s.\nChange Height Type to Overall and fix Height.", 
                node->GetName() ) );
    }
    phyGeom->SetLength( height - radius*2.0f ); 
    phyGeom->SetPosition( tr.gettranslation() ) ;
    matrix33 m( tr.getquatrotation() );
    vector3 eulerangles = m.to_euler();
    phyGeom->SetOrientation( eulerangles.x + PI/2.0f, eulerangles.y, eulerangles.z );

    return phyGeom;
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @param physicPath where save the mesh
    @param isSimple flag to indicate if mesh is for a simple object
    @returns the physics geometry object
*/
nPhysicsGeom* 
n3dsPhysicExport::CreateGeomTrimesh( INode * const node, const nString & physicPath, bool isSimple )
{
    NLOG( physicExport , ( 4 , "CreateGeomTrimesh \"%s\" " , node->GetName() ) );
    Mesh * trimesh;
    ObjectState os = node->EvalWorldState( 0 );
    if( isSimple )
    {
        SimpleObject * sobj = (SimpleObject*)os.obj;

        sobj->BuildMesh( 0 );
        trimesh = &sobj->mesh;
        trimesh->buildNormals();
    }
    else
    {
        TriObject * tobj = (TriObject*)os.obj;

        trimesh = &(tobj->GetMesh());
    }

    n3dsSystemCoordinates * sysCoord = n3dsExportServer::Instance()->GetSystemCoordinates();

    // get the parameters of the Trimesh
    transform44 tr = sysCoord->MaxToNebulaTransform( node->GetNodeTM(0) );

    // only need the position and rotation
    // the scale is in the ObjectTM
    tr.setscale( vector3( 1.f, 1.f, 1.f ) );
    matrix44 mat = tr.getmatrix();
    mat.invert();

    transform44 trObject = sysCoord->MaxToNebulaTransform( node->GetObjectTM(0) );
    matrix44 matObject = trObject.getmatrix();

    // get transformation for normals
    transform44 trObjectN = sysCoord->MaxToNebulaTransform( node->GetObjectTM(0) );
    trObjectN.setscale( vector3( 1.f, 1.f, 1.f ) );
    trObjectN.settranslation( vector3( 0.0f, 0.0f, 0.0f ) );
    matrix44 matObjectN = trObjectN.getmatrix();

    nMeshBuilder mesh( 3 * trimesh->getNumFaces(), trimesh->getNumFaces(), 1 );
    nMeshBuilder::Vertex vertex[3];
    nMeshBuilder::Triangle triangle;

    Point3 point;
    vector3 pos;
    vector3 normal;
    const bool hasNormal = trimesh->normalsBuilt != 0;
    for( int i = 0 ; i < trimesh->getNumFaces() ; ++i )
    {
        const Face& face = trimesh->faces[i];

        if ( hasNormal )
        {
            point = trimesh->getFaceNormal( i );
            normal = sysCoord->MaxtoNebulaVertex( point );
            normal = matObjectN * normal;
            normal.norm();
        }

        for( int j=0 ; j<3 ; ++j )
        {
            point = trimesh->getVert( face.v[j] );
            pos = sysCoord->MaxtoNebulaVertex( point );
            pos = matObject * pos;
            pos = mat * pos;
            vertex[j].SetCoord( pos );

            if ( hasNormal )
            {
                vertex[j].SetNormal( normal );
            }
        }

        mesh.AddTriangle( vertex[0], vertex[1], vertex[2] );
    }

    mesh.Cleanup(0);

    // create the geometry object
    nKernelServer* ks = nKernelServer::ks;

    nString filename( physicPath );
    filename += "/meshes/";
    filename += node->GetName();
    filename += ".n3d2";

    nFileServer2::Instance()->MakePath( filename.ExtractDirName().Get() );
    
    mesh.SaveN3d2( ks->GetFileServer(), filename.Get() );

    nPhyGeomTriMesh * phyGeom = static_cast<nPhyGeomTriMesh*>( ks->New("nphygeomtrimesh" ) );
    phyGeom->AppendCommentFormat("Max Id:    .8X",  node->GetHandle() );
    phyGeom->AppendCommentFormat("Max name: \"%s\"", node->GetName() );
    phyGeom->Enable();
    phyGeom->SetPosition( tr.gettranslation() ) ;
    matrix33 m( tr.getquatrotation() );
    vector3 eulerangles = m.to_euler();
    phyGeom->SetOrientation( eulerangles.x, eulerangles.y, eulerangles.z );

    phyGeom->SetFile( filename.Get() );

    return phyGeom;
}

//------------------------------------------------------------------------------
/**
    @param node 3dsMax node
    @param physicPath where save the mesh
    @param geoms array to put in the geometries created
*/
void
n3dsPhysicExport::CreateGeomCylinder( INode * const node, const nString & physicPath, nArray<nPhysicsGeom*> & geoms )
{
    NLOG( physicExport , ( 4 , "CreateGeomCylinder \"%s\" " , node->GetName() ) );
    SimpleObject* sobj = (SimpleObject*)node->GetObjectRef();
    IParamBlock* pb = sobj->pblock;

    n3dsSystemCoordinates * sysCoord = n3dsExportServer::Instance()->GetSystemCoordinates();

    // get the parameters of the Capsule
    float radius=1.0, height=1.0;
    transform44 tr = sysCoord->MaxToNebulaTransform( node->GetNodeTM(0) );

    // check that scale is correct
    if( ! sysCoord->HaveIndependentScale( node->GetNodeTM(0) ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The object scale isn't in the Local Transform" ) );
        return;
    }

    vector3 scale = tr.getscale();
    if( (fabs( scale.x - scale.z ) > max_error) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING:The scale in GeomCylinder is incorrect. Need to be equal in radius axis." ) );
        return;
    }

    // look for parameters
    for( int i=0; i<pb->NumParams() ; ++i )
    {
        TSTR name = sobj->GetParameterName( i );

        if( 0 == strcmp(name, "Radius" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Cylinder" );
            radius = sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) );
        }
        else if( 0 == strcmp(name, "Height" ) )
        {
            n_assert2( pb->GetParameterType( i ) == TYPE_FLOAT, "Bad Type in Collision Cylinder" );
            height = sysCoord->MaxToNebulaDistance( pb->GetFloat( i ) );
        }
    }

    // check the scale
    if( !( scale.x > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.x <= 0.0" ) );
        return;
    }
    if( !( scale.y > 0.f ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: The scale is incorrect. scale.y <= 0.0" ) );
        return;
    }

    // adjust scale
    radius = radius * scale.x;
    height = height * scale.y;

    // check the dimensions
    if( !( radius >= min_flt_persistable ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: Can't Create Cylinder Geometry with radius <= 0.0" ) );
        return;
    }
    if( !( height >= min_flt_persistable ) )
    {
        N3DSWARN( physicExport , ( 0 , "WARNING: Can't Create Cylinder Geometry with height <= 0.0" ) );
        return;
    }

    // create the geometries
    nKernelServer* ks = nKernelServer::ks;
    if( (height - radius*2.0f) < max_error )
    {
        nPhysicsGeom * geom = 0;
        geom = n3dsPhysicExport::CreateGeomTrimesh( node, physicPath, true );
        n_assert( geom );
        if( geom )
        {
            geoms.Append( geom );
        }
    }
    else
    {
        // create the body using a capsule
        nPhyGeomCylinder * phyBody = static_cast<nPhyGeomCylinder*>( ks->New("nphygeomcylinder" ) );
        n_assert( phyBody );
        phyBody->AppendCommentFormat("Max Id:    .8X",  node->GetHandle() );
        phyBody->AppendCommentFormat("Max name: \"%s\"", node->GetName() );
        if( phyBody )
        {
            phyBody->Enable();
            phyBody->SetRadius( radius );

            phyBody->SetLength( height - radius*2.0f ); 
            phyBody->SetPosition( tr.gettranslation() ) ;
            matrix33 m( tr.getquatrotation() );
            vector3 eulerangles = m.to_euler();
            phyBody->SetOrientation( eulerangles.x + PI/2.0f, eulerangles.y, eulerangles.z );

            geoms.Append( phyBody );
        }

        float boxSide = ( 2.f * radius ) / sqrt( 2.f );

        // create the end A using a box
        nPhyGeomBox * phyEnd = static_cast<nPhyGeomBox*>( ks->New("nphygeombox" ) );
        n_assert( phyEnd );
        phyEnd->AppendCommentFormat("Max Id:    .8X",  node->GetHandle() );
        phyEnd->AppendCommentFormat("Max name: \"%s\"", node->GetName() );
        if( phyEnd )
        {
            phyEnd->Enable();

            phyEnd->SetLengths( vector3( boxSide, boxSide, radius ) );

            matrix44 mrot( tr.getquatrotation() );

            vector3 dir( 0, ( height - radius ) / 2.f, 0 );

            dir = mrot * dir;

            matrix33 m( tr.getquatrotation() );
            vector3 eulerangles = m.to_euler();

            dir = dir + tr.gettranslation();

            phyEnd->SetPosition( dir );
            phyEnd->SetOrientation( eulerangles.x + PI/2.0f, eulerangles.y, eulerangles.z );

            geoms.Append( phyEnd );
        }

        // create the end B using a box
        phyEnd = static_cast<nPhyGeomBox*>( ks->New("nphygeombox" ) );
        n_assert( phyEnd );
        phyEnd->AppendCommentFormat("Max Id:    .8X",  node->GetHandle() );
        phyEnd->AppendCommentFormat("Max name: \"%s\"", node->GetName() );
        if( phyEnd )
        {
            phyEnd->Enable();

            phyEnd->SetLengths( vector3( boxSide, boxSide, radius ) );

            matrix44 mrot( tr.getquatrotation() );

            vector3 dir( 0, -( height - radius ) / 2.f, 0 );

            dir = mrot * dir;

            matrix33 m( tr.getquatrotation() );
            vector3 eulerangles = m.to_euler();

            dir = dir + tr.gettranslation();

            phyEnd->SetPosition( dir );
            phyEnd->SetOrientation( eulerangles.x + PI/2.0f, eulerangles.y, eulerangles.z );

            geoms.Append( phyEnd );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsPhysicExport::Init()
{
    n3dsExportServer* const server = n3dsExportServer::Instance();
    n3dsSceneList list = server->GetMaxIdOrderScene();

    n3dsSceneList::iterator index( list.Begin( n3dsObject::physics) );
    index.ShowProgressBar( "Export physics: ");

    for ( ; index != list.End() ; ++index) 
    {
        n3dsObject& object = (*index);
        IGameNode*   node    = object.GetNode();
        IGameObject* obj     = node->GetIGameObject();
        INode*       maxnode = node->GetMaxNode();
        NLOG( physicExport , ( 3 , "Init \"%s\" " , node->GetName() ) );

        switch(obj->GetIGameType())
        {

        case IGameObject::IGAME_HELPER:
            CreatePhysic( maxnode );
            break;

        case IGameObject::IGAME_MESH:
        {
            BOOL isComposite = 0;
            maxnode->GetUserPropBool( "physicComposite", isComposite );
            if( !isComposite )
            {
                CreatePhysic( maxnode );
            }
            break;
        }

        default:
            break;
        }
        
    }

};

//------------------------------------------------------------------------------
