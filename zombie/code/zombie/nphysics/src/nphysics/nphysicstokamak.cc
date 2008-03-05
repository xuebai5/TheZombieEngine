#include "nphysics/nphysicsconfig.h"

#ifdef PHYSICS_TOKAMAK

//-----------------------------------------------
namespace {
    worldid PhysicsServer = 0;
}; 
//-----------------------------------------------


void tokamak_convert( const matrix33& source, neM3& target )
{
    target.M[0].v[0] = source.M11;
    target.M[0].v[1] = source.M21;
    target.M[0].v[2] = source.M31;
    target.M[0].v[3] = 0;
    target.M[1].v[0] = source.M12;
    target.M[1].v[1] = source.M22;
    target.M[1].v[2] = source.M32;
    target.M[1].v[3] = 0;
    target.M[2].v[0] = source.M13;
    target.M[2].v[1] = source.M23;
    target.M[2].v[2] = source.M33;
    target.M[2].v[3] = 0;
}

void tokamak_convert( const neM3& source, matrix33& target )
{
    target.M11 = source.M[0].v[0];
    target.M21 = source.M[0].v[1];
    target.M31 = source.M[0].v[2];
    target.M12 = source.M[1].v[0];
    target.M22 = source.M[1].v[1];
    target.M32 = source.M[1].v[2];
    target.M13 = source.M[2].v[0];
    target.M23 = source.M[2].v[1];
    target.M33 = source.M[2].v[2];
}


void phyDestroyGeometry( geomid id )
{
    if( id )
        delete id;
}

spaceid phyGetGeometrySpace( geomid id )
{
    // NOTE: To be done properly
    return id;
}

void phyEnableGeometry( geomid id )
{
    // Ignored not supported by tokamak
}

void phyDisableGeometry( geomid id )
{
    // Ignored not supported by tokamak
}

bool phyIsEnabledGeometry( geomid id )
{
    // Ignored not supported by tokamak
    return true;
}


void phySetPositionGeometry( geomid id, const vector3& newposition )
{
    neT3 transform;
    transform = id->GetTransform();
    transform.pos.v[0] = newposition.x;
    transform.pos.v[1] = newposition.y;
    transform.pos.v[2] = newposition.z;

    id->SetTransform(transform); 
}


void phySetOrientationGeometry( geomid id, const matrix33& newrotation )
{
    neT3 transform;
    transform = id->GetTransform();

    tokamak_convert( newrotation, transform.rot );

    id->SetTransform( transform );
}

void phyGetPositionGeometry( geomid id, vector3& position )
{
    neT3 transform;
    transform = id->GetTransform();

    position.x = transform.pos[0];
    position.y = transform.pos[1];
    position.z = transform.pos[2];
}

void phyGetRotationGeometry( geomid id, matrix33& rotation )
{
    neT3 transform;
    transform = id->GetTransform();
    
    tokamak_convert( transform.rot, rotation );
}

void phySetRadiusSphere( geomid id, phyreal radius )
{
    id->SetSphereDiameter( radius * phyreal(2.0) );
}

phyreal phyGetRadiusSphere( geomid id )
{
    phyreal radius;
    id->GetSphereDiameter(radius);
    return radius / phyreal(2.0);
}

geomid phyCreateSphere( phyreal radius, parent ParentObject )
{
    geomid sphere( ParentObject->AddGeometry() );
    
    sphere->SetSphereDiameter( radius * phyreal(2.0));

    return sphere;
}

geomid phyCreateSimpleSpace() 
{
    return NoValidID; // TODO: Finish code
}

geomid phyCreateHashSpace()
{
    return NoValidID; // TODO: Finish code
}

void phyAddToSpace( geomid space, geomid id )
{
// TODO: Finish code
}

void phyRemoveFromSpace( geomid space, geomid id )
{
    // TODO: Finish code
}

int phyGeomIDToInt( geomid id )
{
    return reinterpret_cast<int>(id);
}

int phyWorldIDToInt( worldid id )
{
    return reinterpret_cast<int>(id);
}

void phyCreateGeometries( worldid id, bool enabled )
{
    /// Not needed in tokamak
}

worldid phyCreateWorld( paramsworld paramsWorld  ) 
{
    if( PhysicsServer )
        return PhysicsServer;

    PhysicsServer = neSimulator::CreateSimulator( *paramsWorld );
    return PhysicsServer;
}

void phyDestroyWorld( worldid id )
{
    neSimulator::DestroySimulator( PhysicsServer );

    PhysicsServer = NoValidID;
}

void phySetGravityWorld( worldid id, const vector3& gravity )
{
    // Not suported by tokamak
}

void phySetBoxLengths( geomid id, const vector3& lengths )
{
    id->SetBoxSize( lengths.x, lengths.y, lengths.z );
}

void phyGetBoxLengths( geomid id, vector3& lengths )
{
    neV3 lengths_tokamak;

    id->GetBoxSize( lengths_tokamak );

    lengths.x = lengths_tokamak.v[0];
    lengths.y = lengths_tokamak.v[1];
    lengths.z = lengths_tokamak.v[2];
}

geomid phyCreateBox( phyreal lenx, phyreal leny, phyreal lenz, parent ParentObject )
{
    return ParentObject->AddGeometry();
}

void phyGetAABBGeometry( geomid id, nPhysicsAABB& boundingbox )
{
    f32 diameter;

    neT3 transform_tokamak(id->GetTransform());

    if( id->GetSphereDiameter(diameter) )
    {
        transform_tokamak.pos;

        diameter /= phyreal(2.0);

        boundingbox.minx = transform_tokamak.pos[0] - diameter;
        boundingbox.maxx = transform_tokamak.pos[0] + diameter;
        boundingbox.miny = transform_tokamak.pos[1] - diameter;
        boundingbox.maxy = transform_tokamak.pos[1] + diameter;
        boundingbox.minz = transform_tokamak.pos[2] - diameter;
        boundingbox.maxz = transform_tokamak.pos[2] + diameter;
    }


    // Note: To be implemented
}

geomid phyCreateRay( phyreal length, parent ParentObject )
{
    return reinterpret_cast<geomid>(ParentObject->AddSensor());
}

void phySetRayLength( geomid id, phyreal length )
{
    /// Ignored by tokamak
}

phyreal phyGetRayLength( geomid id )
{
    /// Ignored by tokamak    
    return 0;
}

void phySetRayDirection( geomid id, const vector3& direction )
{
    neV3 direction_tokamak;

    direction_tokamak.v[0] = direction.x;
    direction_tokamak.v[1] = direction.y;
    direction_tokamak.v[2] = direction.z;

    neSensor* sensor( reinterpret_cast<neSensor*>(id) );

    sensor->SetLineSensor( sensor->GetLineVector(), direction_tokamak );
}

void phyGetRayDirection( geomid id, vector3& direction )
{
    neSensor* sensor( reinterpret_cast<neSensor*>(id) );

    direction.x = sensor->GetLineVector().v[0];
    direction.y = sensor->GetLineVector().v[1];
    direction.z = sensor->GetLineVector().v[2];
}

void phySetRayStartPosition( geomid id, const vector3& newposition )
{
    neV3 position_tokamak;

    position_tokamak.v[0] = newposition.x;
    position_tokamak.v[1] = newposition.y;
    position_tokamak.v[2] = newposition.z;

    neSensor* sensor( reinterpret_cast<neSensor*>(id) );
    
    sensor->SetLineSensor( position_tokamak, sensor->GetLineVector() );
}

void phyGetRayStartPosition( geomid id, vector3& position )
{
    neSensor* sensor( reinterpret_cast<neSensor*>(id) );

    position.x = sensor->GetLinePos().v[0];
    position.y = sensor->GetLinePos().v[1];
    position.z = sensor->GetLinePos().v[2];
    
}

int phyCollide( geomid idA, geomid idB, int numContacts, contactgeom* contacts, size_t sizeContactData )
{
    neCollisionTable* CollTable_Tokamak( PhysicsServer->GetCollisionTable() );

    CollTable_Tokamak->Set( idA->ID(), idB->ID() ); 
}

void phyGetContactPoint( const contactgeom& contactGeom, vector3& contactpoint )
{
    contactpoint.x = contactGeom.worldContactPointB.v[0];
    contactpoint.y = contactGeom.worldContactPointB.v[1];
    contactpoint.z = contactGeom.worldContactPointB.v[2];
}

void phyGetContactNormal( const contactgeom& contactGeom, vector3& normal )
{
    normal.x = contactGeom.collisionNormal.v[0];
    normal.y = contactGeom.collisionNormal.v[1];
    normal.z = contactGeom.collisionNormal.v[2];
}

phyreal phyGetContactPenetrationDepth( const contactgeom& contactGeom )
{
    vector3 vect;
    
    vect.x = contactGeom.worldContactPointB.v[0] - contactGeom.worldContactPointA.v[0];
    vect.y = contactGeom.worldContactPointB.v[1] - contactGeom.worldContactPointA.v[1];
    vect.z = contactGeom.worldContactPointB.v[2] - contactGeom.worldContactPointA.v[2];

    return vect.len();
}

int phyBodyIDToInt( bodyid id ) 
{
    return reinterpret_cast<int>(id);
}

bodyid phyCreateBody( worldid id )
{
    return id->CreateRigidBody();
}

paramsworld phyGetParamsWorld()
{
    paramsworld params(new neSimulatorSizeInfo);

    return params;
}

void phyAttachBodyToGeom( geomid geom, bodyid body )
{
}

#endif 