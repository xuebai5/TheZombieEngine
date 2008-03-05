#ifndef N_PHYSICSCONFIG_H
#define N_PHYSICSCONFIG_H

// Nebula Includes
#include "mathlib/matrix.h"
#include "nphysics/nphysicsaabb.h"

#define NOPHYSICS_ENGINE_SELECTED

#define PHY_JOINED_BODIES_DONT_COLLIDE

//#define PHY_ATMOSPHERE_RESISTANCE_BY_FORCE

#ifndef NGAME

extern const vector4 phy_color_boundingbox;
extern const vector4 phy_color_boundingbox_selected;
extern const vector4 phy_color_boundingbox_disabled;
extern const vector4 phy_color_physicsobj_boundingbox;
extern const vector4 phy_color_shape;
extern const vector4 phy_color_normal_contact;
extern const vector4 phy_color_ray;
extern const vector4 phy_color_trianglemesh;
extern const vector4 phy_color_trianglemesh_disabled;
extern const vector4 phy_color_areamanager;
extern const vector4 phy_color_heightmap;
extern const vector4 phy_color_space;
extern const vector4 phy_color_heightmap_disabled;
extern const vector4 phy_color_space_disabled;
extern const vector4 phy_color_terrain_hole;
extern const vector4 phy_color_world_space;
extern const vector4 phy_color_dynamic_space;
extern const vector4 phy_color_dynamic_space_disabled;
extern const vector4 phy_color_joint_axisA;
extern const vector4 phy_color_joint_axisB;
extern const vector4 phy_color_joint_axisC;
extern const vector4 phy_color_joint_anchorA;
extern const vector4 phy_color_joint_anchorB;
extern const vector4 phy_color_joint_anchorC;
extern const vector4 phy_color_joint_anchorWall;
extern const vector4 phy_color_normals;

extern const float phy_normals_lenght;

// joints anchor points radius
extern const float phy_radius_joint_anchor;

// joints axis length
extern const float phy_length_joint_axis;

#endif

#define PHYSICS_ODE

#ifdef PHYSICS_ODE

// ODE Includes
#include "ode/ode.h"

// Nebula Includes
#include "mathlib/matrix.h"

// Type of the geometry id
typedef dGeomID                 geomid;
typedef dSpaceID                spaceid;
typedef dWorldID                worldid;
typedef dBodyID                 bodyid;
typedef dReal                   phyreal;
typedef dContactGeom            contactgeom;
typedef dTriMeshDataID          trimeshdata;
typedef dVector3                phyvector;
typedef dJointGroupID           jointgroupid;
typedef dMass                   phymass;
typedef dJointID                jointid;
typedef dContact                contactdata;
typedef dSurfaceParameters      surfacedata;
typedef dJointFeedback          jointdata;

typedef void (*phyCollisionCallback) (geomid, geomid);
typedef void (*phyCollisionCallbackData) (geomid, geomid,void*);

#define phyInfinity             phyreal(dInfinity)

#define NoValidID              0

#undef NOPHYSICS_ENGINE_SELECTED

#endif // PHYSICS_ODE

namespace phy {

typedef enum {
    axisA = 0,
    axisB,
    axisC,
    axisD,
    axisE
} jointaxis;

typedef enum {
    lowerStop = 0, // low stop angle or position
    higherStop, // high stop angle or position,
    velocity, // as for linear as for angular
    maxForce, // as for force as for torque,
    fudgeFactor, // don't ask
    bounce, // how bouncy are the stops (0..1) 0 = not bouncy
    CFM, // constraint force mixing (when not at stop)
    stopERP, // error reduction parameter (when at stop)
    stopCFM, // as CFM but at stops
    suspensionERP, // suspension error reduction parameter (only TwoHinge joints)
    suspensionCFM // suspension constraint force mixing (only TwoHinge joints)
} jointparameter;

}

// Should be at the end of this file always
#ifdef NOPHYSICS_ENGINE_SELECTED
    #error There's required a physics engine
#endif

/// Conversion mesures
#define BHP2NEWTONS(bhps)   (bhps*phyreal(147000))

/// Destroys a geometry object
inline 
void phyDestroyGeometry( geomid id )
{
    dGeomDestroy( id );
}

/// Gets the space of geometry
inline 
spaceid phyGetGeometrySpace( geomid id )
{
    return dGeomGetSpace( id );
}   

/// Enables a geometry
inline 
void phyEnableGeometry( geomid id )
{
	dGeomEnable( id );
}

/// Disables a geometry
inline 
void phyDisableGeometry( geomid id )
{
	dGeomDisable( id );
}

/// Returns if a geometry is enabled
inline
bool phyIsEnabledGeometry( geomid id )
{
    if( dGeomIsEnabled( id ) )
        return true;
    return false;
}

/// Sets the position of a geometry
inline
void phySetPositionGeometry( geomid id, const vector3& newposition )
{
    dGeomSetPosition( id, newposition.x, newposition.y, newposition.z );
}

/// Sets the geometry orientation
void phySetOrientationGeometry( geomid id, const matrix33& neworientation );

/// Sets the geometry orientation
void phySetOrientationGeometry( geomid id, phyreal ax, phyreal ay, phyreal az, phyreal angle );

/// Returns the geometry position
void phyGetPositionGeometry( geomid id, vector3& position );

/// Returns the geometry orientation
void phyGetOrientationGeometry( geomid id, matrix33& orientation );

/// Sets the radius of a sphere
inline
void phySetRadiusSphere( geomid id, phyreal radius )
{
    dGeomSphereSetRadius( id, radius );
}

/// Returns the radius of a sphere
inline
phyreal phyGetRadiusSphere( geomid id )
{
    return dGeomSphereGetRadius( id );
}

/// Creates a sphere
inline
geomid phyCreateSphere( phyreal radius )
{
    return dCreateSphere( 0, radius );
}

/// Create a simple space
inline
geomid phyCreateSimpleSpace()
{
    return reinterpret_cast<geomid>(dSimpleSpaceCreate( 0 ));
}

/// Create a simple space
inline
geomid phyCreateHashSpace()
{
    return reinterpret_cast<geomid>(dHashSpaceCreate( 0 ));
}

/// Adds a geometry/space to an space
inline
void phyAddToSpace( geomid space, geomid id )
{
    dSpaceAdd( reinterpret_cast<dSpaceID>(space), id );
}

/// Removes a geometry/space from an space
inline
void phyRemoveFromSpace( geomid space, geomid id )
{
    dSpaceRemove( reinterpret_cast<dSpaceID>(space), id );
}

/// Converts a geometry id to an integer
inline
int phyGeomIDToInt( geomid id )
{
    return int(reinterpret_cast<size_t>(id));
}

/// Converts a world id to an integer
inline
int phyWorldIDToInt( worldid id )
{
    return int(reinterpret_cast<size_t>(id));
}

/// Converst a body id to an interger
inline
int phyBodyIDToInt( bodyid id )
{
    return int(reinterpret_cast<size_t>(id));
}

/// Sets if the geometries should be created enabled
inline
void phyCreateGeometries( worldid id, bool enabled )
{
    dWorldSetAutoDisableFlag( id, !enabled );
}

/// Creates a new world
inline
worldid phyCreateWorld()
{
    return dWorldCreate();
}

/// Destroys a world
inline
void phyDestroyWorld( worldid id )
{
    dWorldDestroy( id );
}

/// Sets the gravity of a given world
void phySetGravityWorld( worldid id, const vector3& gravity );

/// Sets the lengths of a box
inline
void phySetGravityWorld( worldid id, const vector3& gravity )
{
    dWorldSetGravity( id, gravity.x, gravity.y, gravity.z );
}

/// Returns the lengths of a box
inline
void phySetBoxLengths( geomid id, const vector3& lengths )
{
    dGeomBoxSetLengths( id, lengths.x, lengths.y, lengths.z );
}

/// Creates a geomtric box
inline
geomid phyCreateBox( phyreal lenx, phyreal leny, phyreal lenz )
{
    return dCreateBox( 0, lenx, leny, lenz );
}

/// Gets the bounding box of a geometry
void phyGetAABBGeometry( geomid id, nPhysicsAABB& boundingbox );

/// Creates a ray geometry
inline
geomid phyCreateRay( phyreal length  )
{
    return dCreateRay( 0, length );
}

/// Sets the length of a ray
inline
void phySetRayLength( geomid id, phyreal length )
{
    dGeomRaySetLength( id, length );
}

/// Sets the length of a ray
inline
phyreal phyGetRayLength( geomid id )
{
    return dGeomRayGetLength( id );
}

/// Sets the direction of a ray
void phySetRayDirection( geomid id, const vector3& direction );

/// Gets the direction of a ray
inline
void phyGetRayDirection( geomid id, vector3& direction ) 
{
    dVector3 start;
    dGeomRayGet( id, start, &direction.x );
}

/// Sets the start position of a ray
void phySetRayStartPosition( geomid id, const vector3& newposition );

/// Gets the start position of a ray
inline
void phyGetRayStartPosition( geomid id, vector3& position )
{
    dVector3 dir;

    dGeomRayGet( id, &position.x, dir );
}

/// Checks if there's collision between two geometries
inline
int phyCollide( geomid idA, geomid idB, int numContacts, contactgeom* contacts, size_t sizeContactData )
{
    return dCollide( idA, idB, numContacts, contacts, int(sizeContactData) );
}

/// Returs the contact point of a contact
void phyGetContactPoint( const contactgeom& contactGeom, vector3& contactpoint );

/// Returns the normal of a contact
inline
void phyGetContactNormal( contactdata& data, vector3& normal )
{
    normal.x = data.geom.normal[0];
    normal.y = data.geom.normal[1];
    normal.z = data.geom.normal[2];
}

inline
void phyGetContactNormal( const contactgeom& contactGeom, vector3& normal )
{
    normal.x = contactGeom.normal[0];
    normal.y = contactGeom.normal[1];
    normal.z = contactGeom.normal[2];
}

/// Returns the A geomtry id of the contact
inline
geomid phyGetContactGeometryA( const contactgeom& contactGeom )
{
    return contactGeom.g1;
}

/// Returns the B geomtry id of the contact
inline
geomid phyGetContactGeometryB( const contactgeom& contactGeom )
{
    return contactGeom.g2;
}

/// Returns the the penetration depth of a contact
inline
phyreal phyGetContactPenetrationDepth( const contactgeom& contactGeom )
{
    return contactGeom.depth;
}

/// Creates a rigid body
inline
bodyid phyCreateBody( worldid id )
{
   return dBodyCreate( id );
}

/// Attaches a body to the geometry
inline
void phyAttachBodyToGeom( geomid geom, bodyid body )
{
    dGeomSetBody( geom, body );
}

/// Creates a cylinder geometry
inline
geomid phyCreateCylinder( phyreal length, phyreal radius )
{
    return dCreateCCylinder( NoValidID, radius, length );
}

/// Creates a cone geometry
inline
geomid phyCreateCone( phyreal length, phyreal radius )
{
    return dCreateCone( NoValidID, radius, length );
}

/// Returns the length of a cylinder
phyreal phyGetLengthCylinder( geomid id );

/// Returns the radius of a cylinder
phyreal phyGetRadiusCylinder( geomid id );

/// Sets the length of a cylinder
void phySetLengthCylinder( geomid id, phyreal length );

/// Sets the radius of a cylinder
void phySetRadiusCylinder( geomid id, phyreal radius );

/// Creates a triangle mesh geometry
inline
geomid phyCreateTriMesh( trimeshdata& data )
{
    return dCreateTriMesh( 0, data ,0, 0, 0 );
}

/// Creates an entity representing the data for a trimesh
inline
trimeshdata phyCreateTriMeshData()
{
    return dGeomTriMeshDataCreate();
}

/// Creates an entity representing the data for a trimesh
inline
void phyDestroyTriMeshData( trimeshdata id )
{
    dGeomTriMeshDataDestroy( id );
}

/// Builds data for a trimesh
inline
void phyDataBuildTriMesh( 
                         trimeshdata dataId,
                         const void* vertexs,
                         int vertStride,
                         int vertCount,
                         const void* indexes,
                         int indexCount,
                         int indexStride,
                         const void* normals)
{
    dGeomTriMeshDataBuildSingle1( 
        dataId,
        vertexs,
        vertStride,
        vertCount,
        indexes,
        indexCount,
        indexStride,
        normals );
}

/// Associates data with a trimesh
inline
void phyTriMeshData( geomid id, trimeshdata dataId )
{
    dGeomTriMeshSetData( id, dataId );
}

/// Sets the categories where the geometry belongs
inline
void phyAddCategory( geomid id, int categories )
{
    dGeomSetCategoryBits( id, dGeomGetCategoryBits( id ) | categories );
}

/// Sets the categories wich the geom will collide with
inline
void phyAddCollideWith( geomid id, int categories )
{
    dGeomSetCollideBits( id, dGeomGetCollideBits( id ) | categories );
}

/// Sets the levels of a hash space
inline
void phySetLevelsHashSpace( geomid space, int max, int min )
{
    dHashSpaceSetLevels( reinterpret_cast<dSpaceID>(space), min, max );
}

/// Gets the levels of a hash space
inline
void phyGetLevelsHashSpace( geomid space, int& max, int& min )
{
    dHashSpaceGetLevels( reinterpret_cast<dSpaceID>(space), &min, &max );
}

/// Creates hash space
geomid phyCreateHashSpace();

/// Creates a quadtree space
inline
geomid phyCreateQuadTreeSpace( const vector3& center, const vector3& extens, int depth )
{
    dVector3 center_ode = { center.x, center.y, center.z };
    dVector3 extens_ode = { extens.x, extens.y, extens.z }; 

    return reinterpret_cast<geomid>(dQuadTreeSpaceCreate( 0, center_ode, extens_ode, depth ));
}

/// runs the physics simulation
inline
void phyRunSimulation( worldid id, phyreal step )
{
    dWorldQuickStep( id, step );
}

/// sets the mass of a body with box shape
void phySetBodyBoxMass( bodyid id, phyreal density, phyreal mass, const vector3& lengths );

/// sets the mass of a body with sphere shape
void phySetBodySphereMass( bodyid id, phyreal density, phyreal mass, phyreal radius );

/// sets the mass of a body with sphere shape
void phySetBodyCylinderMass( bodyid id, phyreal density, phyreal mass, phyreal radius, phyreal length );

/// sets the body position
inline
void phySetBodyPosition( bodyid id, const vector3& position )
{
    dBodySetPosition( id, position.x, position.y, position.z );
}

/// sets the body orientation
void phySetBodyOrientation( bodyid id, const matrix33& orientation );

/// Destroys a contacts group
inline
void phyDestroyJointGroup( jointgroupid id )
{
    dJointGroupDestroy(id);
}

/// Finds collisions within a space
void phyCollideWithinSpace( spaceid id, phyCollisionCallback callback );

/// Finds collisions within a space
void phyCollideWithinSpace( spaceid id, phyCollisionCallbackData callback, void* data );

/// Finds collisions between spaces
void phyCollideSpaces( geomid idA, geomid idB, phyCollisionCallback callback );

/// Finds collisions between spaces
void phyCollideSpaces( geomid idA, geomid idB, phyCollisionCallbackData callback, void* data );

/// Empties the contact group
inline
void phyEmptyJointGroup( jointgroupid id )
{
    dJointGroupEmpty( id );
}

/// Removes the categories to collide with
inline
void phyRemoveCollideWith( geomid id, int categories )
{
    dGeomSetCollideBits( id, dGeomGetCollideBits( id ) & ~categories );
}

/// Sets the mass of a body
void phySetBodyMass( bodyid id, phyreal mass, phymass& massdata );

/// Removes the categories where the geometry belongs
inline
void phyRemoveCategory( geomid id, int categories )
{
    dGeomSetCategoryBits( id, dGeomGetCategoryBits( id ) & ~categories );
}

/// Sets the set up for the world for this physics engine
void phyEngineWorldParameters( worldid id );

/// Creates a geometry transform
inline
geomid phyCreateGeomTrans()
{
    return dCreateGeomTransform(0);
}

/// sets the geometry for the geometry transform
inline
void phyGeomTransSetGeometry( geomid transid, geomid id )
{
    dGeomTransformSetGeom( transid, id );
}

/// gets the body position
void phyGetBodyPosition( bodyid id, vector3& position );

/// computes the mass for a body box shaped
inline
void phyComputeBoxMass( phymass& massdata, phyreal density, const vector3& lengths )
{
    dMassSetBox( &massdata, density, lengths.x, lengths.y ,lengths.z );
}

/// computes the mass for a body sphere shaped
inline
void phyComputeSphereMass( phymass& massdata, phyreal density, phyreal radius )
{
    dMassSetSphere( &massdata, density, radius );
}

/// computes the mass for a cylinder sphere shaped
inline
void phyComputeCylinderMass( phymass& massdata, phyreal density, phyreal radius, phyreal length )
{
    dMassSetCylinder( &massdata, density, 3, radius, length );
}

/// adds masses information
inline
void phyAddMass( phymass& massresult, const phymass& sourceA,const phymass& sourceB )
{
    massresult = sourceA;

    dMassAdd( &massresult, &sourceB );
}

/// translates the mass
inline
void phyMassTranslate( phymass& mass, const vector3& position )
{
    dMassTranslate( &mass, position.x, position.y, position.z );
}

/// rotates the mass
void phyMassRotate( phymass& mass, const matrix33& orientation );

/// gets the mass information of a body
inline
void phyGetBodyMass( bodyid id, phymass& mass )
{
    dBodyGetMass( id, &mass );
}

/// returns the scalar mass
inline
phyreal phyGetScalarMass( const phymass& mass )
{
    return mass.mass;
}

/// returns the bit flags with the collide bitfields
inline
int phyGetCollideWith( geomid id )
{
    return dGeomGetCollideBits(id);
}

/// returns the bit flags with the collide bitfields
inline
int phyGetCategories( geomid id )
{
    return dGeomGetCategoryBits(id);
}

/// sets the categories where the geometry belongs
inline
void phySetCategories( geomid id, int categories )
{
    dGeomSetCategoryBits( id, categories );
}

/// sets the categories with wich the geometry collide
inline
void phySetCollideWith( geomid id, int categories )
{
    dGeomSetCollideBits( id, categories );
}

/// destroys a body
inline
void phyDestroyBody( bodyid id )
{
    dBodyDestroy( id );
}

/// sets the linear velocity
inline
void phySetLinearVelocity( bodyid id, const vector3& velocity )
{
    dBodySetLinearVel( id, velocity.x, velocity.y, velocity.z );
}

/// gets the linear velocity
void phyGetLinearVelocity( bodyid id, vector3& velocity  );

/// sets the angular velocity
inline
void phySetAngularVelocity( bodyid id, const vector3& velocity )
{
    dBodySetAngularVel( id, velocity.x, velocity.y, velocity.z );
}

/// gets the angular velocity
void phyGetAngularVelocity( bodyid id, vector3& velocity );

/// adds a force to the body
inline
void phyAddForce( bodyid id, const vector3& force )
{
    dBodyAddForce( id, force.x, force.y, force.z );
}

/// adds a torque to the body
inline
void phyAddTorque( bodyid id, const vector3& torque )
{
    dBodyAddTorque( id, torque.x, torque.y, torque.z );
}

/// adds a relative force to the body
inline
void phyAddRelativeForce( bodyid id, const vector3& force )
{
    dBodyAddRelForce( id, force.x, force.y, force.z );
}

/// adds a relative torque to the body
inline
void phyAddRelativeTorque( bodyid id, const vector3& torque )
{
    dBodyAddRelTorque( id, torque.x, torque.y, torque.z );
}

/// adds a force at a given position
inline
void phyAddForceAtPosition( bodyid id, const vector3& force, const vector3& position )
{
    dBodyAddForceAtPos( id, force.x, force.y, force.z, position.x, position.y, position.z );
}

/// adds a relative force at a given position
inline
void phyAddRelativeForceAtPosition( bodyid id, const vector3& force, const vector3& position )
{
    dBodyAddRelForceAtPos( id, force.x, force.y, force.z, position.x, position.y, position.z );
}

/// adds a relative force at a relative position
inline
void phyAddRelativeForceAtRelativePosition( bodyid id, const vector3& force, const vector3& position )
{
    dBodyAddRelForceAtRelPos( id, force.x, force.y, force.z, position.x, position.y, position.z );
}

/// sets the absolute force applied to the object
inline
void phySetForce( bodyid id, const vector3& force )
{
    dBodySetForce( id, force.x, force.y, force.z );
}

/// sets the absolute torque applied to the object
inline
void phySetTorque( bodyid id, const vector3& torque )
{
    dBodySetTorque( id, torque.x, torque.y, torque.z );
}

/// gets the absolute force applied to the object
void phyGetForce( bodyid id, vector3& force );
 
/// gets the absolute torque applied to the object
void phyGetTorque( bodyid id, vector3& torque );

/// adds a force at relative position
inline
void phyAddForceAtRelativePosition( bodyid id, const vector3& force, const vector3& position )
{
    dBodyAddForceAtRelPos( id, force.x, force.y, force.z, position.x, position.y, position.z );
}

/// creates a joint group
inline
jointgroupid phyCreateJointGroup( int size )
{
    return dJointGroupCreate( size );
}

/// attaches two bodies with a joint
inline
void phyJointAttach( jointid id, bodyid idA, bodyid idB )
{
    dJointAttach( id, idA, idB );
}

/// destroys a joint
inline
void phyDestroyJoint( jointid id )
{
    dJointDestroy( id );
}

/// creates a contact joint
inline
jointid phyCreateContactJoint( worldid wid, jointgroupid jgid, contactdata data )
{
    return dJointCreateContact( wid, jgid, &data );
}

/// sets the coloumb friction coeficient in the data structure
void phySetColoumbFrictionCoeficient( surfacedata& data, phyreal coeficient, bool first = true );

/// sets the pyramid friction coeficient in the data structure
void phySetPyramidFrictionCoeficient( surfacedata& data, phyreal coeficient, bool first = true );

/// sets the bouncing coeficient in the data structure
inline
void phySetBouncingCoeficient( surfacedata& data, phyreal coeficient )
{
    data.mode |= dContactBounce;
    data.bounce = coeficient;
}

/// sets the dependent slip coeficient in the data structure
void phySetDependentSlipCoeficient( surfacedata& data, phyreal coeficient, bool first = true );

/// sets the softness ERP coeficient in the data structure
inline
void phySetSoftnessERPCoeficient( surfacedata& data, phyreal coeficient )
{
    data.mode |= dContactSoftERP;
    data.soft_erp = coeficient;
}

/// sets the softness CFM coeficient in the data structure
inline
void phySetSoftnessCFMCoeficient( surfacedata& data, phyreal coeficient )
{
    data.mode |= dContactSoftCFM;
    data.soft_cfm = coeficient;
}

/// sets the minumun velocity to bounce
inline
void phySetBounceVelocity( surfacedata& data, phyreal velocity )
{
    data.bounce_vel = velocity;
}

/// resets the surface data structure
inline
void phyReset( surfacedata& data )
{
    data.mode = 0;
}

/// returns he bodyid given a geomid
inline
bodyid phyGetBodyId( geomid id )
{
    return dGeomGetBody(id);
}

/// creates a two hinge joint
inline
jointid phyCreateTwoHingeJoint( worldid wid, jointgroupid jid )
{
    return dJointCreateHinge2( wid,jid );
}

/// sets the anchor for a two hinge joint
inline
void phySetTwoHingeAnchor( jointid id, const vector3& anchor )
{
    dJointSetHinge2Anchor( id, anchor.x, anchor.y, anchor.z );
}

/// sets the first axis for a two hinge joint
inline
void phySetTwoHingeFirstAxis( jointid id, const vector3& axis )
{
    dJointSetHinge2Axis1( id, axis.x, axis.y, axis.z );
}

/// sets the second axis for a two hinge joint
inline
void phySetTwoHingeSecondAxis( jointid id, const vector3& axis )
{
    dJointSetHinge2Axis2( id, axis.x, axis.y, axis.z );
}

/// returns the bodies of a joint
inline
bodyid phyJointGetBody( jointid id, int index )
{
    return dJointGetBody( id, index );
}

/// fills a joint with extra data
inline
void phyJointExtraData( jointid id, jointdata* data )
{
    dJointSetFeedback( id, data );
}

/// return the absolute value of the force applied to a body from a joint
phyreal phyGetForce( const jointdata* data, int index );

/// return the absolute value of the torque applied to a body from a joint
phyreal phyGetTorque( const jointdata* data, int index );

/// sets the material info
inline
void phySetMaterial( contactdata& data, surfacedata const& material )
{
    data.surface = material;
}

/// sets the contact information
inline
void phySetContact( contactdata& data, contactgeom const& geom )
{
    data.geom = geom;
}

/// sets the direction information of a contact
inline
void phySetContactDirection( contactdata& data, const vector3& direction )
{
    data.fdir1[0] = direction.x;
    data.fdir1[1] = direction.y;
    data.fdir1[2] = direction.z;

    data.surface.mode |= dContactFDir1;
}

/// sets the parameters for a hinge-2 type
void phyHinge2SetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value );

/// sets the parameters for a universal type
void phyUniversalSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value );

/// gets the axis direction 
inline
void phyGetHinge2Axis( jointid id, vector3& direction, const phy::jointaxis axis )
{
    if( axis == phy::axisA )
        dJointGetHinge2Axis1( id, &direction.x );
    else
        dJointGetHinge2Axis2( id, &direction.x );
}

/// creates a ball and socket joint
inline
jointid phyCreateBallSocketJoint( worldid wid, jointgroupid jid ) 
{
    return dJointCreateBall( wid, jid );
}

/// setting the anchor point for a ball socket anchor
inline
void phySetBallSocketAnchor( jointid id, const vector3& anchor )
{
    dJointSetBallAnchor( id, anchor.x, anchor.y, anchor.z );
}

/// creates a hinge joint
inline
jointid phyCreateHingeJoint( worldid wid, jointgroupid jid ) 
{
    return dJointCreateHinge( wid, jid );
}

/// creates a angular motor joint
inline
jointid phyCreateAngularMotorJoint( worldid wid, jointgroupid jid )
{
    return dJointCreateAMotor( wid, jid );
}

/// setting the anchor point for a hinge anchor
inline
void phySetHingeAnchor( jointid id, const vector3& anchor )
{
    dJointSetHingeAnchor( id, anchor.x, anchor.y, anchor.z );
}

/// setting the axis of a hinge joint
inline
void phySetHingeAxis( jointid id, const vector3& axis ) 
{
    dJointSetHingeAxis( id, axis.x, axis.y, axis.z );
}

/// creates a slider joint
inline
jointid phyCreateSliderJoint( worldid wid, jointgroupid jid ) 
{
    return dJointCreateSlider( wid, jid );
}

/// setting the axis of a slider joint
inline
void phySetSliderAxis( jointid id, const vector3& axis )
{
    dJointSetSliderAxis( id, axis.x, axis.y, axis.z );
}

/// creates a universal joint
inline
jointid phyCreateUniversalJoint( worldid wid, jointgroupid jid ) 
{
    return dJointCreateUniversal( wid, jid );
}

/// sets the anchor for an universal joint
inline
void phySetUniversalAnchor( jointid id, const vector3& anchor )
{
    dJointSetUniversalAnchor( id, anchor.x, anchor.y, anchor.z );
}

/// sets the first axis for an universal joint
inline
void phySetUniversalFirstAxis( jointid id, const vector3& axis )
{
    dJointSetUniversalAxis1( id, axis.x, axis.y, axis.z );
}

/// sets the second axis for an universal joint
inline
void phySetUniversalSecondAxis( jointid id, const vector3& axis )
{
    dJointSetUniversalAxis2( id, axis.x, axis.y, axis.z );
}

/// returns the anchor of a ball and socket joint
inline
void phyGetBallSocketAnchor( jointid id, vector3& anchor )
{
    dJointGetBallAnchor( id, &anchor.x );
}

/// returns the anchor of a hinge joint
inline
void phyGetHingeAnchor( jointid id, vector3& anchor )
{
    dJointGetHingeAnchor( id, &anchor.x );
}

/// returns the axis of a hinge joint
inline
void phyGetHingeAxis( jointid id, vector3& axis )
{
    dJointGetHingeAxis( id, &axis.x );
}

/// returns the angle of a hinge joint
inline
phyreal phyGetHingeAngle( jointid id )
{
    return dJointGetHingeAngle( id );
}

/// return the axis of a slider joint
inline
void phyGetSliderAxis( jointid id, vector3& axis )
{
    dJointGetSliderAxis( id, &axis.x );
}

/// returns the first axis of a two hinge joint
inline
void phyGetTwoHingeFirstAxis( jointid id, vector3& axis )
{
    dJointGetHinge2Axis1( id, &axis.x );
}

/// returns the second axis of a two hinge joint
inline
void phyGetTwoHingeSecondAxis( jointid id, vector3& axis )
{
    dJointGetHinge2Axis2( id, &axis.x );
}

/// returns the anchor of a two hinge joint
inline
void phyGetTwoHingeAnchor( jointid id, vector3& anchor )
{
    dJointGetHinge2Anchor( id, &anchor.x );
}

/// returns the second anchor of a two hinge joint
inline
void phyGetTwoHingeAnchor2( jointid id, vector3& anchor )
{
    dJointGetHinge2Anchor2( id, &anchor.x );
}

/// returns the anchor of a universal joint
inline
void phyGetUniversalAnchor( jointid id, vector3& anchor )
{
    dJointGetUniversalAnchor( id, &anchor.x );
}

/// returns the first axis of a universal joint
inline
void phyGetUniversalFirstAxis( jointid id, vector3& axis )
{
    dJointGetUniversalAxis1( id, &axis.x );
}

/// returns the second axis of a universal joint
inline
void phyGetUniversalSecondAxis( jointid id, vector3& axis )
{
    dJointGetUniversalAxis2( id, &axis.x );
}

/// returns if a body has contact joints
bool phyHasContactJoints( bodyid id );

/// returns the body orientation
void phyGetBodyOrientation( bodyid id, matrix33& orientation );

/// enables a body
inline
void phyEnableBody( bodyid id )
{
    dBodyEnable( id );
}

/// disables a body
inline
void phyDisableBody( bodyid id )
{
    dBodyDisable( id );
}

/// returns if a body it's enabled
inline
bool phyIsEnabled( bodyid id )
{
    return dBodyIsEnabled( id ) ? true : false;
}

/// returns if two bodies are connected
inline
bool phyAreConnected( bodyid idA, bodyid idB )
{
    return dAreConnected( idA, idB ) ? true : false;
}

/// sets the parameters of a hinge joint
void phyHingeSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value );

/// sets the parameters of an angular motor joint
void phyAngularMotorSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value );

/// creates a infinite plane geometry
inline
geomid phyCreatePlane( const vector4& /*equation*/ )
{
    return dCreatePlane( 0, 0, 0, 0, 0 );
}

/// sets a plane equation
inline
void phyPlaneSetEquation( geomid id, const vector4& equation )
{
    dGeomPlaneSetParams( id, equation.x, equation.y, equation.z, equation.w );
}

/// attach data to a geometry
inline
void phyAttachDataGeom( geomid id, void* data )
{
    dGeomSetData( id, data );
}

/// retrieve data from a geometry
inline
void* phyRetrieveDataGeom( geomid id )
{
    return dGeomGetData( id );
}

/// attach data to a body
inline
void phyAttachDataBody( bodyid id, void* data )
{
    dBodySetData( id, data );
}

/// retrieve data from a body
inline
void* phyRetrieveDataBody( bodyid id )
{
    return dBodyGetData( id );
}

/// set the joint parametes for a slider joint
void phySliderSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value );

/// set the joint axis for an angular motor
inline
void phySetAngularMotorAxis( jointid id, phy::jointaxis axis, const vector3& axi )
{
    dJointSetAMotorAxis( id, axis, 0, axi.x, axi.y, axi.z );
}

/// sets the number of axis of an angular motor
inline
void phySetNumberOfAxisAngularMotor( jointid id, const int numAxis )
{
    dJointSetAMotorNumAxes( id, numAxis );
}

/// gets the orientation of a geometry in quaternion
inline
void phyGetOrientationGeom( geomid id, vector4& orientation )
{
    dGeomGetQuaternion(id, &orientation.x );
}

/// sets if a body it's a fast rotating object
inline
void phySetFastRotating( bodyid id, bool is )
{
    dBodySetFiniteRotationMode( id, is ? 1 : 0 );
}

/// sets the axis for a fast rotating objects
inline
void phySetFastRotationAxis( bodyid id, const vector3& axis )
{
    dBodySetFiniteRotationAxis( id, axis.x, axis.y, axis.z );
}

/// returns the angle of the first axis of a universal joint
inline
phyreal phyGetUniversalAngleA( jointid id )
{
    return dJointGetUniversalAngle1( id );
}

/// returns the angle of the second axis of a universal joint
inline
phyreal phyGetUniversalAngleB( jointid id )
{
    return dJointGetUniversalAngle2( id );
}

/// creates a heightmap geometry
inline
geomid phyCreateHeightMap( phyreal* heights, phyreal length, int numNodes, int step )
{
    return dCreateTerrainY( 0, heights, length, numNodes, true, true, step );
}

/// returns the height in height map
inline
phyreal phyGetHeightInHeightMap( geomid id, const int x, const int z )
{
    return dGeomTerrainYPointDepthInt( id, x, 0, z );
}

/// returns the length of a cone
phyreal phyGetLengthCone( geomid id );

/// returns the radius of a cone
phyreal phyGetRadiusCone( geomid id );

/// sets the length of a cone
void phySetLengthCone( geomid id, phyreal length );

/// sets the radius of a cone
void phySetRadiusCone( geomid id, phyreal radius );

/// sets the contact direction
void phySetContactDirection( contactdata& data, const vector3& direction );

/// retrives the contact normal
void phyGetContactNormal( contactdata& data, vector3& normal );

/// enables the friction direction in a material
inline
void phyEnableFrictionDirection( surfacedata& data )
{
    data.mode |= dContactFDir1;
}

/// disables the friction direction in a material
inline
void phyDisableFrictionDirection( surfacedata& data )
{
    data.mode &= ~dContactFDir1;
}

/// sets the slip coeficient in a contact data POD
inline
void phySetSlipCoeficient( contactdata& data, const phyreal coeficient, const bool first )
{
    if( first )
        data.surface.slip1 = coeficient;
    else
        data.surface.slip2 = coeficient;
}

/// gets the slip coeficient in a contact data POD
inline
phyreal phyGetSlipCoeficient( contactdata const& data, const bool first )
{
    if( first )
        return data.surface.slip1;
    return data.surface.slip2;
}

/// updates heightmap bounding boxes
inline
void phyUpdateBoundingBoxesHeightMap( geomid id )
{
    dGeomTerrainYUpdateAABB( id );
}

/// retrieves if there is/are a hole/s in the heighmap
inline
bool phyHasTerrainYHoles( geomid id )
{
    return dTerrainYHasHoles( id );
}

/// sets a hole in a terrain
inline
void phySetTerrainYHoleIn( geomid id, int x, int z )
{
    dSetTerrainYHoleIn( id, x, z );
}

/// gets a hole in a terrain
inline
bool phyGetTerrainYHoleIn( geomid id, int x, int z )
{
    return dGetTerrainYHoleIn( id, x, z );
}

/// removes the holes from a terrain
inline
void phyRemoveTerrainYHoles( geomid id )
{
    dTerrainYRemoveHoles( id );
}

/// retrieves a body linear velocity treshold for auto-disable
inline
phyreal phyGetLinearVelocityThresholdAutoDisable( bodyid id )
{
    return dBodyGetAutoDisableLinearThreshold(id);
}

/// retrieves a body angular velocity treshold for auto-disable
inline
phyreal phyGetAngularVelocityThresholdAutoDisable( bodyid id )
{
    return dBodyGetAutoDisableAngularThreshold(id);
}

/// gets the box lengths
inline
void phyGetBoxLengths( geomid id, vector3& lengths )
{
    dGeomBoxGetLengths( id, &lengths.x );
}

/// sets the auto-disable flag to a body
inline
void phyBodyAutoDisableFlag( bodyid id, bool flag )
{
    dBodySetAutoDisableFlag( id, flag );
}

/// return the value of a given parameter
phyreal phyHingeGetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis );

/// return the number of geometries within a space
inline
int phyGetNumGeometriesWithinASpace( spaceid id ) {
    return dSpaceGetNumGeoms( id );
}

inline
geomid phyGetGeometryFromSpace( spaceid id, int index ) {
    return dSpaceGetGeom( id, index );
}

/// marks the geometry to be updated
void phyGeomUpdateGeometry( geomid id );

#endif 