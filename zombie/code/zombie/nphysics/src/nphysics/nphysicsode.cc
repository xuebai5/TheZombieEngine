#include "precompiled/pchnphysics.h"

#ifdef PHYSICS_ODE

void ode_convert( const matrix33& source, dMatrix3& target )
{
    target[0] = source.M11;
    target[1] = source.M21;
    target[2] = source.M31;
    target[3] = 0;
    target[4] = source.M12;
    target[5] = source.M22;
    target[6] = source.M32;
    target[7] = 0;
    target[8] = source.M13;
    target[9] = source.M23;
    target[10] = source.M33;
    target[11] = 0;
}

void ode_convert( const dReal* source, vector3& target ) 
{
    target.x = *source;
    target.y = *++source;
    target.z = *++source;
}

void ode_convert( const dReal* source, matrix33& target )
{
    target.M11 = source[0];
    target.M21 = source[1];
    target.M31 = source[2];
    target.M12 = source[4];
    target.M22 = source[5];
    target.M32 = source[6];
    target.M13 = source[8];
    target.M23 = source[9];
    target.M33 = source[10];
}

void phySetOrientationGeometry( geomid id, const matrix33& neworientation )
{
    dMatrix3 matrix_ode;

    ode_convert( neworientation, matrix_ode );

    dGeomSetRotation( id, matrix_ode );
}

void phySetOrientationGeometry( geomid id, phyreal ax, phyreal ay, phyreal az, phyreal angle )
{
    dMatrix3 matrix_ode;

    dRFromAxisAndAngle( matrix_ode, ax, ay, az, angle );

    dGeomSetRotation( id, matrix_ode );
}

void phyGetPositionGeometry( geomid id, vector3& position )
{
    dReal const* pos_ode( dGeomGetPosition( id ));

    n_assert2( pos_ode , "Null pointer" );

    ode_convert( pos_ode, position );
}

void phyGetOrientationGeometry( geomid id, matrix33& orientation )
{
    dReal const* rot_ode( dGeomGetRotation( id ));

    n_assert2( rot_ode , "Null pointer" );

    ode_convert( rot_ode, orientation );
}

void phyGetAABBGeometry( geomid id, nPhysicsAABB& boundingbox )
{
    phyreal boundingbox_ode[6];
    dGeomGetAABB( id, boundingbox_ode );

    boundingbox.minx = boundingbox_ode[0];
    boundingbox.maxx = boundingbox_ode[1];
    boundingbox.miny = boundingbox_ode[2];
    boundingbox.maxy = boundingbox_ode[3];
    boundingbox.minz = boundingbox_ode[4];
    boundingbox.maxz = boundingbox_ode[5];
}

void phySetRayDirection( geomid id, const vector3& direction )
{
    dVector3 start;
    dVector3 dir;

    dGeomRayGet( id, start, dir );

    dGeomRaySet( id, start[0], start[1], start[2], direction.x, direction.y, direction.z ); 
}


void phySetRayStartPosition( geomid id, const vector3& newposition )
{
    dVector3 start;
    dVector3 dir;

    dGeomRayGet( id, start, dir );

    dGeomRaySet( id, newposition.x, newposition.y, newposition.z, dir[0], dir[1], dir[2] ); 
}


void phyGetContactPoint( const contactgeom& contactGeom, vector3& contactpoint )
{
    contactpoint.x = contactGeom.pos[0];
    contactpoint.y = contactGeom.pos[1];
    contactpoint.z = contactGeom.pos[2];
}


phyreal phyGetLengthCylinder( geomid id )
{
    phyreal radius_ode, length_ode;

    dGeomCCylinderGetParams( id, &radius_ode, &length_ode );

    return length_ode;
}

phyreal phyGetRadiusCylinder( geomid id )
{
    phyreal radius_ode, length_ode;

    dGeomCCylinderGetParams( id, &radius_ode, &length_ode );

    return radius_ode;
}

void phySetLengthCylinder( geomid id, phyreal length )
{
    phyreal radius_ode(phyGetRadiusCylinder( id ));

    dGeomCCylinderSetParams( id, radius_ode, length );
}

void phySetRadiusCylinder( geomid id, phyreal radius )
{
    phyreal length_ode(phyGetLengthCylinder( id ));

    dGeomCCylinderSetParams( id, radius, length_ode );
}

phyreal phyGetLengthCone( geomid id )
{
    phyreal radius_ode, length_ode;

    dGeomConeGetParams( id, &radius_ode, &length_ode );

    return length_ode;
}

phyreal phyGetRadiusCone( geomid id )
{
    phyreal radius_ode, length_ode;

    dGeomConeGetParams( id, &radius_ode, &length_ode );

    return radius_ode;
}

void phySetLengthCone( geomid id, phyreal length )
{
    phyreal radius_ode(phyGetRadiusCone( id ));

    dGeomConeSetParams( id, radius_ode, length );
}

void phySetRadiusCone( geomid id, phyreal radius )
{
    phyreal length_ode(phyGetLengthCone( id ));

    dGeomConeSetParams( id, radius, length_ode );
}

void phySetBodyBoxMass( bodyid id, phyreal density, phyreal mass, const vector3& lengths )
{
    dMass mass_ode;

    dMassSetBox( &mass_ode, density, lengths.x, lengths.y ,lengths.z );

    dMassAdjust( &mass_ode, mass );

    dBodySetMass( id, &mass_ode );
}

void phySetBodyCylinderMass( bodyid id, phyreal density, phyreal mass, phyreal radius, phyreal length )
{
    dMass mass_ode;

    dMassSetCylinder( &mass_ode, density, 3, radius, length );

    dMassAdjust( &mass_ode, mass );

    dBodySetMass( id, &mass_ode );
}

void phySetBodyMass( bodyid id, phyreal mass, phymass& massdata )
{
    dMassAdjust( &massdata, mass );

    dBodySetMass( id, &massdata );
}

void phyMassRotate( phymass& mass, const matrix33& orientation )
{
    dMatrix3 matrix_ode;

    ode_convert( orientation, matrix_ode );

    dMassRotate( &mass, matrix_ode );
}

void phySetBodySphereMass( bodyid id, phyreal density, phyreal mass, phyreal radius )
{
    dMass mass_ode;

    dMassSetSphere( &mass_ode, density, radius );

    dMassAdjust( &mass_ode, mass );

    dBodySetMass( id, &mass_ode );
}

void phySetBodyOrientation( bodyid id, const matrix33& orientation )
{
    dMatrix3 matrix_ode;

    ode_convert( orientation, matrix_ode );

    dBodySetRotation( id, matrix_ode );
}

static phyCollisionCallback regCollisionCallBack;

static phyCollisionCallbackData regCollisionCallBackData;

static void phyOdeCollisionCallBack( void* /*data*/, geomid gA, geomid gB )
{
    (*regCollisionCallBack)( gA, gB );
}

static void phyOdeCollisionCallBackData( void* data, geomid gA, geomid gB )
{
    (*regCollisionCallBackData)( gA, gB, data );
}

void phyCollideWithinSpace( spaceid id, phyCollisionCallback callback )
{
    regCollisionCallBack = callback;

    dSpaceCollide( id, 0, phyOdeCollisionCallBack );
}

void phyCollideWithinSpace( spaceid id, phyCollisionCallbackData callback, void* data )
{
    regCollisionCallBackData = callback;

    dSpaceCollide( id, data, phyOdeCollisionCallBackData );
}

void phyCollideSpaces( geomid idA, geomid idB, phyCollisionCallback callback )
{
    regCollisionCallBack = callback;

    dSpaceCollide2( idA, idB, 0,phyOdeCollisionCallBack );
}

void phyCollideSpaces( geomid idA, geomid idB, phyCollisionCallbackData callback, void* data )
{
    regCollisionCallBackData = callback;

    dSpaceCollide2( idA, idB, data,phyOdeCollisionCallBackData );
}


void phyEngineWorldParameters( worldid id )
{
    dWorldSetCFM (id,phyreal(1e-6));
    dWorldSetContactMaxCorrectingVel (id,phyreal(1));
    dWorldSetContactSurfaceLayer (id,phyreal(.001));
    dWorldSetERP(id,phyreal(1));
    phyCreateGeometries( id, false );
    dWorldSetAutoDisableLinearThreshold( id, phyreal(.05) );
    dWorldSetAutoDisableAngularThreshold( id, phyreal(.05) );
    dWorldSetAutoDisableSteps( id, 10 );
}

void phyGetBodyPosition( bodyid id, vector3& position )
{
    const dReal* ode_position(dBodyGetPosition(id));

    position.x = *ode_position++;
    position.y = *ode_position++;
    position.z = *ode_position;
}

void phyGetLinearVelocity( bodyid id, vector3& velocity  )
{
    const dReal* ode_velocity(dBodyGetLinearVel( id ));

    velocity.x = *ode_velocity++;
    velocity.y = *ode_velocity++;
    velocity.z = *ode_velocity;
}

void phyGetAngularVelocity( bodyid id, vector3& velocity )
{
    const dReal* ode_velocity(dBodyGetAngularVel( id ));

    velocity.x = *ode_velocity++;
    velocity.y = *ode_velocity++;
    velocity.z = *ode_velocity;
}

void phyGetForce( bodyid id, vector3& force )
{
    const dReal* ode_force(dBodyGetForce( id ));

    force.x = *ode_force++;
    force.y = *ode_force++;
    force.z = *ode_force;
}
 
void phyGetTorque( bodyid id, vector3& torque )
{
    const dReal* ode_torque(dBodyGetTorque( id ));

    torque.x = *ode_torque++;
    torque.y = *ode_torque++;
    torque.z = *ode_torque;
}

void phySetColoumbFrictionCoeficient( surfacedata& data, phyreal coeficient, bool first )
{
    if( first )
        data.mu = coeficient;
    else
    {
        data.mode |= dContactMu2;
        data.mu2 = coeficient;
    }
}

void phySetPyramidFrictionCoeficient( surfacedata& data, phyreal coeficient, bool first )
{
    if( first )
        data.mode |= dContactApprox1_1;
    else
        data.mode |= dContactApprox1_2;

    if( first )
        data.mu = coeficient;
    else
    {
        data.mu2 = coeficient;
    }
}
    
void phySetDependentSlipCoeficient( surfacedata& data, phyreal coeficient, bool first )
{
    if( first )
        data.mode |= dContactSlip1;
    else
        data.mode |= dContactSlip2;
    if( first )
        data.slip1 = coeficient;
    else
        data.slip2 = coeficient;
}


phyreal phyGetForce( const jointdata* data, int index )
{
    // TODO: Speed up
    if( !index )
        return sqrt(    pow(data->f1[0], phyreal(2)) + 
                        pow(data->f1[1], phyreal(2)) + 
                        pow(data->f1[2], phyreal(2)) );
    return sqrt(    pow(data->f2[0], phyreal(2)) + 
                    pow(data->f2[1], phyreal(2)) + 
                    pow(data->f2[2], phyreal(2)) );    
}

phyreal phyGetTorque( const jointdata* data, int index )
{
    // TODO: Speed up
    if( !index )
        return sqrt(    pow(data->t1[0], phyreal(2)) + 
                        pow(data->t1[1], phyreal(2)) + 
                        pow(data->t1[2], phyreal(2)) );
    return sqrt(    pow(data->t2[0], phyreal(2)) + 
                    pow(data->t2[1], phyreal(2)) + 
                    pow(data->t2[2], phyreal(2)) );    
}

void phyHinge2SetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value )
{
    int parameter(0);

    switch( param )
    {
    case phy::lowerStop:
        parameter = int(dParamLoStop);
        break;
    case phy::higherStop:
        parameter = int(dParamHiStop);
        break;
    case phy::velocity:
        if( axis == phy::axisA )
            parameter = int(dParamVel);
        else
            parameter = int(dParamVel2);
        break;
    case phy::maxForce:
        if( axis == phy::axisA )
            parameter = int(dParamFMax);
        else
            parameter = int(dParamFMax2);
        break;
    case phy::fudgeFactor:
        parameter = int(dParamFudgeFactor);
        break;
    case phy::bounce:
        parameter = int(dParamBounce);
        break;
    case phy::CFM:
        parameter = int(dParamCFM);
        break;
    case phy::stopERP:
        parameter = int(dParamStopERP);
        break;
    case phy::stopCFM:
        parameter = int(dParamStopCFM);
        break;
    case phy::suspensionERP:
        parameter = int(dParamSuspensionERP);
        break;
    case phy::suspensionCFM:
        parameter = int(dParamSuspensionCFM);
        break;

    }

    dJointSetHinge2Param( id, parameter, value );
}


void phySliderSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value )
{
    int parameter(0);

    switch( param )
    {
    case phy::lowerStop:
        parameter = int(dParamLoStop);
        break;
    case phy::higherStop:
        parameter = int(dParamHiStop);
        break;
    case phy::velocity:
        if( axis == phy::axisA )
            parameter = int(dParamVel);
        else
            parameter = int(dParamVel2);
        break;
    case phy::maxForce:
        if( axis == phy::axisA )
            parameter = int(dParamFMax);
        else
            parameter = int(dParamFMax2);
        break;
    case phy::fudgeFactor:
        parameter = int(dParamFudgeFactor);
        break;
    case phy::bounce:
        parameter = int(dParamBounce);
        break;
    case phy::CFM:
        parameter = int(dParamCFM);
        break;
    case phy::stopERP:
        parameter = int(dParamStopERP);
        break;
    case phy::stopCFM:
        parameter = int(dParamStopCFM);
        break;
    case phy::suspensionERP:
        parameter = int(dParamSuspensionERP);
        break;
    case phy::suspensionCFM:
        parameter = int(dParamSuspensionCFM);
        break;

    }

    dJointSetSliderParam( id, parameter, value );
}

void phyUniversalSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value )
{
    int parameter(0);

    switch( param )
    {
    case phy::lowerStop:
        if( axis == phy::axisA )
            parameter = int(dParamLoStop);
        else
            parameter = int(dParamLoStop2);
        break;
    case phy::higherStop:
        if( axis == phy::axisA )
            parameter = int(dParamHiStop);
        else
            parameter = int(dParamHiStop2);
        break;
    case phy::velocity:
        if( axis == phy::axisA )
            parameter = int(dParamVel);
        else
            parameter = int(dParamVel2);
        break;
    case phy::maxForce:
        if( axis == phy::axisA )
            parameter = int(dParamFMax);
        else
            parameter = int(dParamFMax2);
        break;
    case phy::fudgeFactor:
        parameter = int(dParamFudgeFactor);
        break;
    case phy::bounce:
        parameter = int(dParamBounce);
        break;
    case phy::CFM:
        parameter = int(dParamCFM);
        break;
    case phy::stopERP:
        parameter = int(dParamStopERP);
        break;
    case phy::stopCFM:
        parameter = int(dParamStopCFM);
        break;
    case phy::suspensionERP:
        parameter = int(dParamSuspensionERP);
        break;
    case phy::suspensionCFM:
        parameter = int(dParamSuspensionCFM);
        break;

    }

    dJointSetUniversalParam( id, parameter, value );
}

/// sets the parameters of an angular motor joint
void phyAngularMotorSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis, const phyreal value )
{
    int parameter(0);

    switch( param )
    {
    case phy::lowerStop:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamLoStop);
            break;
        case phy::axisB:
            parameter = int(dParamLoStop2);
            break;
        case phy::axisC:
            parameter = int(dParamLoStop3);
            break;
        }
        break;
    case phy::higherStop:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamHiStop);
            break;
        case phy::axisB:
            parameter = int(dParamHiStop2);
            break;
        case phy::axisC:
            parameter = int(dParamHiStop3);
            break;
        }
        break;
    case phy::velocity:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamVel);
            break;
        case phy::axisB:
            parameter = int(dParamVel2);
            break;
        case phy::axisC:
            parameter = int(dParamVel3);
            break;
        }
        break;
    case phy::maxForce:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamFMax);
            break;
        case phy::axisB:
            parameter = int(dParamFMax2);
            break;
        case phy::axisC:
            parameter = int(dParamFMax3);
            break;
        }
        break;
    case phy::fudgeFactor:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamFudgeFactor);
            break;
        case phy::axisB:
            parameter = int(dParamFudgeFactor2);
            break;
        case phy::axisC:
            parameter = int(dParamFudgeFactor3);
            break;
        }
        break;
    case phy::bounce:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamBounce);
            break;
        case phy::axisB:
            parameter = int(dParamBounce2);
            break;
        case phy::axisC:
            parameter = int(dParamBounce3);
            break;
        }
        break;
    case phy::CFM:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamCFM);
            break;
        case phy::axisB:
            parameter = int(dParamCFM2);
            break;
        case phy::axisC:
            parameter = int(dParamCFM3);
            break;
        }
        break;
    case phy::stopERP:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamStopERP);
            break;
        case phy::axisB:
            parameter = int(dParamStopERP2);
            break;
        case phy::axisC:
            parameter = int(dParamStopERP3);
            break;
        }
        break;
    case phy::stopCFM:
        switch( axis )
        {
        case phy::axisA:
            parameter = int(dParamStopCFM);
            break;
        case phy::axisB:
            parameter = int(dParamStopCFM2);
            break;
        case phy::axisC:
            parameter = int(dParamStopCFM3);
            break;
        }
        break;
    case phy::suspensionERP:
        parameter = int(dParamSuspensionERP);
        break;
    case phy::suspensionCFM:
        parameter = int(dParamSuspensionCFM);
        break;

    }

    dJointSetAMotorParam( id, parameter, value );
}

void phyHingeSetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis /*axis*/, const phyreal value )
{
    int parameter(0);

    switch( param )
    {
    case phy::lowerStop:
        parameter = int(dParamLoStop);
        break;
    case phy::higherStop:
        parameter = int(dParamHiStop);
        break;
    case phy::velocity:
        parameter = int(dParamVel);
        break;
    case phy::maxForce:
        parameter = int(dParamFMax);
        break;
    case phy::fudgeFactor:
        parameter = int(dParamFudgeFactor);
        break;
    case phy::bounce:
        parameter = int(dParamBounce);
        break;
    case phy::CFM:
        parameter = int(dParamCFM);
        break;
    case phy::stopERP:
        parameter = int(dParamStopERP);
        break;
    case phy::stopCFM:
        parameter = int(dParamStopCFM);
        break;
    case phy::suspensionERP:
        parameter = int(dParamSuspensionERP);
        break;
    case phy::suspensionCFM:
        parameter = int(dParamSuspensionCFM);
        break;

    }

    dJointSetHingeParam( id, parameter, value );
}

bool phyHasContactJoints( bodyid id )
{
    int numJoints(dBodyGetNumJoints(id));

    for( int i(0); i < numJoints; ++i )
    {
        jointid jid(dBodyGetJoint( id, i ));

        if( dJointGetType( jid ) == dJointTypeContact )
            return true;
    }

    return false;
}

void phyGetBodyOrientation( bodyid id, matrix33& orientation )
{
    ode_convert( dBodyGetRotation( id ), orientation );
}


phyreal phyHingeGetParameter( jointid id, const phy::jointparameter param, const phy::jointaxis axis )
{
    int parameter(0);

    switch( param )
    {
    case phy::lowerStop:
        parameter = int(dParamLoStop);
        break;
    case phy::higherStop:
        parameter = int(dParamHiStop);
        break;
    case phy::velocity:
        if( axis == phy::axisA )
            parameter = int(dParamVel);
        else
            parameter = int(dParamVel2);
        break;
    case phy::maxForce:
        if( axis == phy::axisA )
            parameter = int(dParamFMax);
        else
            parameter = int(dParamFMax2);
        break;
    case phy::fudgeFactor:
        parameter = int(dParamFudgeFactor);
        break;
    case phy::bounce:
        parameter = int(dParamBounce);
        break;
    case phy::CFM:
        parameter = int(dParamCFM);
        break;
    case phy::stopERP:
        parameter = int(dParamStopERP);
        break;
    case phy::stopCFM:
        parameter = int(dParamStopCFM);
        break;
    case phy::suspensionERP:
        parameter = int(dParamSuspensionERP);
        break;
    case phy::suspensionCFM:
        parameter = int(dParamSuspensionCFM);
        break;

    }

    return dJointGetHingeParam( id, parameter );
}

void phyGeomUpdateGeometry( geomid id )
{
    dGeomForceUpdate(id);
}

#endif // End: PHYSICS_ODE

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
