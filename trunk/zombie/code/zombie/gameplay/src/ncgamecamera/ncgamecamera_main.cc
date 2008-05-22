//-----------------------------------------------------------------------------
//  ncgamecamera_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchgameplay.h"

#include "ncgamecamera/ncgamecamera.h"

#include "napplication/nappviewport.h"

#include "animcomp/nccharacter.h"

#include "nscene/ncscene.h"

#include "entity/nentityobjectserver.h"

#include "nphysics/nphygeomsphere.h"

#include "nphysics/nphysicsserver.h"

#include "nwaypointserver/routemanager.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncGameCamera,nComponentObject);

//-----------------------------------------------------------------------------

namespace
{
    const float MaxMinCorrection(float(.01));
}

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - nGameCameraManager   Zombie         created
*/
ncGameCamera::ncGameCamera() : 
    cameraType( free ),
    enabled( false ),
    cameraAttributes(0),
    anchor(0),
    viewport(0),
    lookAt(0),
    cameraOffset(0,0,0),
    futurePosition(0,0,0),
    futureOrientation(0,0,0),
    transPositionXType(dampen),
    transPositionYType(dampen),
    transPositionZType(dampen),
    transOrientationType(inmediatly),
    dampening(float(.3),float(.3),float(.3)), // percentatge
    hookedEntity(0),
    frameProportion(1),
    dampeningOrientation(float(.1)),
    maxDistance(15),
    minDistance(10),
    backupPosition(0,0,0),
    collideGeometry(0),
    routeid(-1),
    routeStep(float(.5)),
    lookAtPrevPosition( 0,0,0 ),
    anchorEntity(0),
    offsetLookAt(0)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - nGameCameraManager   Zombie         created
*/
ncGameCamera::~ncGameCamera()
{
    if( this->hookedEntity )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->hookedEntity );
    }
    if( this->collideGeometry )
    {
        n_delete( this->collideGeometry );
    }
}


//-----------------------------------------------------------------------------
/**
    Enables camera.

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::Enable()
{
    this->enabled = true;
}

//-----------------------------------------------------------------------------
/**
    Disables camera.

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::Disable()
{
    this->enabled = false;
}

//-----------------------------------------------------------------------------
/**
    Returns if the camera is enabled.

    @return true/false

    history:
        - 10-Nov-2005   Zombie         created
*/
const bool ncGameCamera::IsEnabled() const
{
    return this->enabled;
}

//-----------------------------------------------------------------------------
/**
    Sets the camera type.

    @param type camera type free, attach... etc..

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::SetCameraType( const type cameratype )
{
    this->cameraType = cameratype;
}

//-----------------------------------------------------------------------------
/**
    Returns the camera type.

    @return camera type free, attach... etc..

    history:
        - 10-Nov-2005   Zombie         created
*/
const ncGameCamera::type ncGameCamera::GetCameraType() const
{
    return this->cameraType;
}

//-----------------------------------------------------------------------------
/**
    Sets attributes to the camera.

    @param attributes camera attributes

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::SetAttributes( const int attributes )
{
    this->cameraAttributes = attributes;
}

//-----------------------------------------------------------------------------
/**
    Gets attributes to the camera.

    @return amera attributes

    history:
        - 10-Nov-2005   Zombie         created
*/
const int ncGameCamera::GetAttributes() const
{
    return this->cameraAttributes;
}

//-----------------------------------------------------------------------------
/**
    Adds a set of attributes.

    @param attributes camera attributes

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::AddAttributes( const int attributes )
{
    this->cameraAttributes |= attributes;
}

//-----------------------------------------------------------------------------
/**
    Removes a set of attributes.

    @param attributes camera attributes

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::RemoveAttributes( const int attributes )
{
    this->cameraAttributes &=  ~attributes;
}

//-----------------------------------------------------------------------------
/**
    Sets the anchor point.

    @param entity an entity that MUST have the ncTransform component.

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::SetAnchorPoint( nEntityObject* entity )
{
    this->anchorEntity = entity;

    if( ! entity )
    {
        this->SetCameraType( free );
        if( this->hookedEntity )
        {
            ncCharacter* character( this->lookAt->GetComponent<ncCharacter>() );

            n_assert2( character, "In orther to hook the entity must have the component ncCharacter." );

            character->Dettach( this->hookedEntity->GetId() );

            nEntityObjectServer::Instance()->RemoveEntityObject( this->hookedEntity );
            this->hookedEntity = 0;
        }
        this->anchor = 0;
        return;
    }

    if( !(this->GetCameraType() & (attach | thirdperson | hooked | firstperson )) )
    {
        n_assert2_always( "It's the wrong type of camera." );
        return;
    }

    if( this->GetCameraType() & hooked )
    {
        // hooking
        this->hookedEntity = nEntityObjectServer::Instance()->NewLocalEntityObject( "nefakeentity" );

        n_assert2( this->hookedEntity, "Failed to create a hooked entity." );

        ncCharacter* character( entity->GetComponent<ncCharacter>() );

        n_assert2( character, "In orther to hook the entity must have the component ncCharacter." );

        character->Attach( "camera", this->hookedEntity->GetId() );

        this->anchor = this->hookedEntity->GetComponent<ncTransform>();
    }
    else
    {
        this->anchor = entity->GetComponent<ncTransform>();
    }

    this->LookAt( entity );

    n_assert2( this->anchor, "The entity is missing the ncTransform component." );
}

//-----------------------------------------------------------------------------
/**
    Returns the anchor point.

    @return entity

    history:
        - 10-Nov-2005   Zombie         created
*/
nEntityObject* ncGameCamera::GetAnchorPoint() const
{
    if( !(this->GetCameraType() & (attach | hooked | thirdperson) ) )
    {
        return 0;
    }

    if( anchorEntity )
    {
        return this->anchorEntity;
    }

    return this->lookAt->GetEntityObject();
}

//-----------------------------------------------------------------------------
/**
    Updates camera.

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::Update( const nTime& time )
{


    n_assert2( this->GetViewPort(), "No viewport available." );

    if( (this->GetCameraType() == free) || (this->GetCameraType() == firstperson) )
    {
        // nothing to do
        return;
    }

    // expected 60 FPS (should be a var holding it)
    this->frameProportion = float(time) / (float(1)/float(60));

    // begin: safety
    if( this->frameProportion > float(4) )
    {
        this->frameProportion = float(4);
    }
    // end: safety

    // backups original position
    this->backupPosition = this->GetViewPort()->GetViewerPos();

    // updates camera position
    this->UpdatePosition();
    
    // updates camera orientation
    this->UpdateOrientation();

    // updates transition
    this->UpdateTransition();

    // last checks
    this->CheckMaxMinDistance();

    // checks collision
    this->CheckCollision();

    // it has to be the last one
    if( this->GetAttributes() & lookat )
    {
        this->UpdateLookAt();           
    }

}

//-----------------------------------------------------------------------------
/**
    Sets the height respect the anchor point.

    @param value height value

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::SetHeight( const float value )
{
    this->cameraOffset.y = value;
}

//-----------------------------------------------------------------------------
/**
    Gets the height respect the anchor point.

    @return height value

    history:
        - 10-Nov-2005   Zombie         created
*/
const float ncGameCamera::GetHeight() const
{
    return this->cameraOffset.y;
}

//-----------------------------------------------------------------------------
/**
    Sets the distance respect the anchor point.

    @param value distance value

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::SetDistance( const float value )
{
    this->cameraOffset.z = value;
}

//-----------------------------------------------------------------------------
/**
    Gets the distance respect the anchor point.

    @return distance value

    history:
        - 10-Nov-2005   Zombie         created
*/
const float ncGameCamera::GetDistance() const
{
    return this->cameraOffset.z;
}

//-----------------------------------------------------------------------------
/**
    Sets the route to follow (rail camera).

    @param name route name

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::SetRoute( const int id )
{
    this->routeid = id;
}

//-----------------------------------------------------------------------------
/**
    Gets the route to follow (rail camera).

    @return route name

    history:
        - 10-Nov-2005   Zombie         created
*/
const int ncGameCamera::GetRoute() const
{
    return this->routeid;
}

//-----------------------------------------------------------------------------
/**
    Sets the viewport.

    @param view the viewport where the camera will be applied

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::SetViewPort( nAppViewport* view )
{
    n_assert2( view, "Null pointer." );

    this->viewport = view;
}

//-----------------------------------------------------------------------------
/**
    Gets the viewport.

    @return the viewport where the camera will be applied

    history:
        - 10-Nov-2005   Zombie         created
*/
nAppViewport* ncGameCamera::GetViewPort() const
{
    return this->viewport;
}

//-----------------------------------------------------------------------------
/**
    Updates camera position.

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::UpdatePosition()
{
    const type cType(this->GetCameraType());

    switch( cType )
    {
    case attach:
        this->UpdatePositionAttached();
        break;
    case hooked:
        this->futurePosition = this->hookedEntity->GetComponent<ncTransform>()->GetPosition();
        break;
    case thirdperson:
        this->ComputePositionThirdPerson();
        break;
    case rails:
        this->UpdatePositionByRoute();
        break;
    }
}

//-----------------------------------------------------------------------------
/**
    Updates orientation.

    history:
        - 10-Nov-2005   Zombie         created
*/
void ncGameCamera::UpdateOrientation()
{
    const type cType(this->GetCameraType());

    switch( cType )
    {
    case attach:
        if( this->GetAttributes() & objOrientation )
        {
            this->UpdateObjectOrientation();
        }
        break;
    case hooked:        
        this->futureOrientation = this->hookedEntity->GetComponent<ncTransform>()->GetEuler();
        break;
    case thirdperson:
        break;
    }
}

//-----------------------------------------------------------------------------
/**
    Updates the position for an attached camera.

    history:
        - 11-Nov-2005   Zombie         created
*/
void ncGameCamera::UpdatePositionAttached()
{
    n_assert2( this->GetViewPort(), "Missing viewport camera." );
    n_assert2( this->GetAnchorPoint(), "Missing anchor point." );

    this->futurePosition = this->anchor->GetPosition() + this->cameraOffset;
}

//-----------------------------------------------------------------------------
/**
    Set camera offset.

    @param offset offset in the different axis

    history:
        - 11-Nov-2005   Zombie         created
*/
void ncGameCamera::SetCameraOffset( const vector3& offset )
{
    this->cameraOffset = offset;
}

//-----------------------------------------------------------------------------
/**
    Get camera offset.

    @return offset in the different axis

    history:
        - 11-Nov-2005   Zombie         created
*/
const vector3& ncGameCamera::GetCameraOffset() const
{
    return this->cameraOffset;
}

//-----------------------------------------------------------------------------
/**
    Sets where to look at.

    @return offset in the different axis

    history:
        - 11-Nov-2005   Zombie         created
*/
void ncGameCamera::LookAt( nEntityObject* entity )
{
    n_assert2( entity, "Null pointer." );

    this->lookAt = entity->GetComponent<ncTransform>();

    n_assert2( this->lookAt, "Missing ncTransform component." );
}

//-----------------------------------------------------------------------------
/**
    Updates lookat info.

    @return offset in the different axis

    history:
        - 11-Nov-2005   Zombie         created
*/
void ncGameCamera::UpdateLookAt()
{
    // temporary code: until the viewport camera has the functionality

    static vector3 up( 0,1, 0 ); // <--- assumption

    vector3 vposition(this->GetViewPort()->GetViewerPos());

    vposition.y += offsetLookAt;

    static bool is(false);

    vector3 direction( vposition - this->lookAt->GetPosition() );

    direction.norm();

    vector3 xCameraComponent,yCameraComponent;

    vector3 euler;

    xCameraComponent = up * direction;
    yCameraComponent = direction * xCameraComponent; 
    matrix33 m( xCameraComponent,yCameraComponent, direction );
    euler = m.to_euler();

    this->GetViewPort()->Transform().seteulerrotation( euler );
}

//-----------------------------------------------------------------------------
/**
    Updates transition.

    history:
        - 11-Nov-2005   Zombie         created
*/
void ncGameCamera::UpdateTransition()
{
    // computes how to go to the real position
    

    vector3 position( this->GetViewPort()->GetViewerPos() );

    // x component
    switch( transPositionXType )
    {
    case inmediatly:
        this->ComputeInmediatlyTransition( position.x, this->futurePosition.x );
        break;
    case dampen:
        this->ComputeDampenTransition( position.x, this->futurePosition.x, this->dampening.x );
        break;
    case none:
        break;
    }

    // y component
    switch( transPositionYType )
    {
    case inmediatly:
        this->ComputeInmediatlyTransition( position.y, this->futurePosition.y );
        break;
    case none:
        break;
    case dampen:
        this->ComputeDampenTransition( position.y, this->futurePosition.y, this->dampening.y );
        break;
    }

    // z component
    switch( transPositionZType )
    {
    case inmediatly:
        this->ComputeInmediatlyTransition( position.z, this->futurePosition.z );
        break;
    case none:
        break;
    case dampen:
        this->ComputeDampenTransition( position.z, this->futurePosition.z, this->dampening.z );
        break;
    }

    this->GetViewPort()->SetViewerPos( position );

    // computes how to go to the real orientation
    vector3 finalOrientation(this->GetViewPort()->Transform().geteulerrotation());

    matrix33 mA,mB;

    quaternion result;

    // x component
    switch( transOrientationType )
    {
    case inmediatly:
        this->ComputeInmediatlyTransition( finalOrientation.x, this->futureOrientation.x );
        this->ComputeInmediatlyTransition( finalOrientation.y, this->futureOrientation.y );
        this->ComputeInmediatlyTransition( finalOrientation.z, this->futureOrientation.z );
        break;
    case dampen:
        
        mA.from_euler( this->futureOrientation );
        mB.from_euler( finalOrientation );

        result.lerp( 
            quaternion(mB.get_quaternion()), 
            quaternion(mA.get_quaternion()), this->dampeningOrientation );

        finalOrientation = matrix33( result ).to_euler();

        break;
    case none:
        break;
    }

    //begin: temporary code
    matrix33 camera;

    camera.from_euler(finalOrientation);

    vector3 zcomponent( camera.z_component() );

    zcomponent *= float(-1);

    static vector3 up(0,1,0);

    vector3 xcomponent( up * zcomponent );
    vector3 ycomponent( zcomponent * xcomponent );

    camera.set( xcomponent, ycomponent, zcomponent );

    finalOrientation = camera.to_euler();
    //end: temporary code

    this->GetViewPort()->Transform().seteulerrotation(finalOrientation);
}

//-----------------------------------------------------------------------------
/**
    Compute inmediatly transtion.

    @param value real world value

    @param shouldbe where it should be

    history:
        - 11-Nov-2005   Zombie         created
*/
void ncGameCamera::ComputeInmediatlyTransition( float& value, const float shouldbe )
{
    value = shouldbe;
}

//-----------------------------------------------------------------------------
/**
    Updates the object orientation.

    history:
        - 11-Nov-2005   Zombie         created
*/
void ncGameCamera::UpdateObjectOrientation()
{
    this->futureOrientation = this->anchor->GetEuler();
}

//-----------------------------------------------------------------------------
/**
    Compute dampened transition.

    history:
        - 15-Nov-2005   Zombie         created
*/
void ncGameCamera::ComputeDampenTransition( float& value, const float shouldbe, const float vDampen )
{
    value += ( shouldbe - value ) * vDampen * this->frameProportion;
}

//-----------------------------------------------------------------------------
/**
    Computes the position for a third person camera.

    history:
        - 17-Nov-2005   Zombie         created
*/
void ncGameCamera::ComputePositionThirdPerson()
{
    this->futurePosition = this->GetCameraOffset();

    matrix44 orientation(this->anchor->GetTransform());

    this->futurePosition = orientation * this->futurePosition;
}

//-----------------------------------------------------------------------------
/**
    Checks for max and min distance.

    history:
        - 17-Nov-2005   Zombie         created
*/
void ncGameCamera::CheckMaxMinDistance()
{
    if( this->GetCameraType() != thirdperson )
    {
        return;
    }

    vector3 positionCamera(this->GetViewPort()->GetViewerPos());

    vector3 distance( positionCamera - this->anchor->GetPosition() );

    const float lenghSquared( distance.lensquared() );

    if( lenghSquared > this->maxDistance * this->maxDistance )
    {
        // too far
        distance.norm();

        distance *= (this->maxDistance*(float(1)-MaxMinCorrection));

        this->GetViewPort()->SetViewerPos(this->anchor->GetPosition() + distance);
    }
    else if( lenghSquared < this->minDistance * this->minDistance )
    {
        // too close
        distance.norm();

        distance *= (this->minDistance*(float(1)+MaxMinCorrection));

        this->GetViewPort()->SetViewerPos(this->anchor->GetPosition() + distance);
    }
}

//-----------------------------------------------------------------------------
/**
    Once setup build data for the camera.

    history:
        - 17-Nov-2005   Zombie         created
*/
void ncGameCamera::Build()
{
    if( this->GetAttributes() & collision )
    {
        this->collideGeometry = n_new(nPhyGeomSphere);

        n_assert2( this->collideGeometry, "Failed to create a collision geometry." );

        this->collideGeometry->SetCollidesWith( nPhysicsGeom::All );

        static_cast<nPhyGeomSphere*>(this->collideGeometry)->SetRadius(3);
    }

    if( this->GetCameraType() == rails )
    {
        this->routeid = nTag( this->GetRoute() ).KeyMap();
    }
}

//-----------------------------------------------------------------------------
/**
    Checks collision.

    history:
        - 17-Nov-2005   Zombie         created
*/
void ncGameCamera::CheckCollision()
{
    if( !(this->GetAttributes() & collision) )
    {
        return;
    }

    this->collideGeometry->SetPosition( this->GetViewPort()->GetViewerPos() );

    nPhyCollide::nContact contacts[10];

    int numCollisions( nPhysicsServer::Instance()->Collide( this->collideGeometry,1, contacts ) );

    if( numCollisions )
    {

        vector3 normals(0,0,0);

        for( int counter(0); counter <numCollisions; ++counter )
        {
            vector3 position;

            contacts[counter].GetContactPosition( position );

            vector3 normal;

            contacts[counter].GetContactNormal( normal );

            if( normal.y < 0 )
                normal.y = -normal.y;

            float depth(contacts[counter].GetContactPenetrationDepth());

            if( depth < 0 )
                depth = -depth;

            normals += position + (normal * float(3.2) *  depth );
        }

        normals *= float(1)/float(numCollisions);
 
        this->GetViewPort()->SetViewerPos( normals );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the step pace for a route.

    @param value step value

    history:
        - 23-Nov-2005   Zombie         created
*/
void ncGameCamera::SetStep( const float value )
{
    n_assert2( value > 0, "The step has to be bigger than zero." );

    this->routeStep = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the step pace for a route.

    @return step value

    history:
        - 23-Nov-2005   Zombie         created
*/
const float ncGameCamera::GetStep() const
{
    return this->routeStep;
}

//-----------------------------------------------------------------------------
/**
    Updates position base in a route.

    history:
        - 23-Nov-2005   Zombie         created
*/
void ncGameCamera::UpdatePositionByRoute()
{
    // @todo: I don't like this implementation, find a better way

    if( this->GetAttributes() & directed )
    {
        vector3 movement( this->lookAt->GetPosition() - this->lookAtPrevPosition );

        const float step( movement.len() );

        if( step < this->GetStep() * this->frameProportion )
        {
            return;
        }

        this->lookAtPrevPosition = this->lookAt->GetPosition();

        vector3 posA,posB;
    
        RouteManager::Instance().FollowRoute( this->routeid, this->GetStep() * this->frameProportion , posA );

        RouteManager::Instance().Reverse( this->routeid );
        
        RouteManager::Instance().FollowRoute( this->routeid, this->GetStep() * this->frameProportion , posB );
        
        RouteManager::Instance().FollowRoute( this->routeid, this->GetStep() * this->frameProportion , posB );

        const vector3& pos(this->lookAt->GetPosition());

        vector3 direction( pos - posA );

        const float lengthA( direction.lensquared() );

        direction = pos - posB;

        if( direction.lensquared() < lengthA )
        {
            this->futurePosition = posB;
        }
        else
        {
            this->futurePosition = posA;

            RouteManager::Instance().Reverse( this->routeid );

            RouteManager::Instance().FollowRoute( this->routeid, this->GetStep() * this->frameProportion , posB );
            
            RouteManager::Instance().FollowRoute( this->routeid, this->GetStep() * this->frameProportion , posB );
        }
    }
    else
    {
        RouteManager::Instance().FollowRoute( this->routeid, this->GetStep() * this->frameProportion , this->futurePosition );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the transposition type for position in X.

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetTranspositionXType( const transition typeTrans )
{
    this->transPositionXType = typeTrans;
}

//-----------------------------------------------------------------------------
/**
    Gets the transposition type for position in X.

    history:
        - 25-Nov-2005   Zombie         created
*/
const ncGameCamera::transition ncGameCamera::GetTranspositionXType() const
{
    return this->transPositionXType;
}

//-----------------------------------------------------------------------------
/**
    Sets the transposition type for position in Y.

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetTranspositionYType( const transition typeTrans )
{
    this->transPositionYType = typeTrans;
}

//-----------------------------------------------------------------------------
/**
    Gets the transposition type for position in Y.

    history:
        - 25-Nov-2005   Zombie         created
*/
const ncGameCamera::transition ncGameCamera::GetTranspositionYType() const
{
    return this->transPositionYType;
}


//-----------------------------------------------------------------------------
/**
    Sets the transposition type for position in Z.

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetTranspositionZType( const transition typeTrans )
{
    this->transPositionZType = typeTrans;
}

//-----------------------------------------------------------------------------
/**
    Gets the transposition type for position in Z.

    history:
        - 25-Nov-2005   Zombie         created
*/
const ncGameCamera::transition ncGameCamera::GetTranspositionZType() const
{
    return this->transPositionZType;
}

//-----------------------------------------------------------------------------
/**
    Sets the dampening for the different axis (0..1) in position

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetDampeningPosition( const vector3& dampen )
{
    this->dampening = dampen;
}

//-----------------------------------------------------------------------------
/**
    Gets the dampening for the different axis (0..1) in position

    history:
        - 25-Nov-2005   Zombie         created
*/
const vector3& ncGameCamera::GetDampeningPosition() const
{
    return this->dampening;
}

//-----------------------------------------------------------------------------
/**
    Sets the dampening for the orientation

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetDampeningOrientation( const float percentatge )
{
    n_assert2( percentatge >= 0, "The value has to be between 0..1" );
    n_assert2( percentatge <= float(1), "The value has to be between 0..1" );
    
    this->dampeningOrientation = percentatge;
}

//-----------------------------------------------------------------------------
/**
    Gets the dampening for the orientation

    history:
        - 25-Nov-2005   Zombie         created
*/
const float ncGameCamera::GetDampeningOrientation() const
{
    return this->dampeningOrientation;
}

//-----------------------------------------------------------------------------
/**
    Sets the max distance of the camera

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetMaxDistance(const float distance)
{
    this->maxDistance = distance;
}

//-----------------------------------------------------------------------------
/**
    Gets the max distance of the camera

    history:
        - 25-Nov-2005   Zombie         created
*/
const float ncGameCamera::GetMaxDistance() const
{
    return this->maxDistance; 
}

//-----------------------------------------------------------------------------
/**
    Sets the min distance of the camera

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetMinDistance(const float distance)
{
    this->minDistance = distance;
}

//-----------------------------------------------------------------------------
/**
    Gets the min distance of the camera

    history:
        - 25-Nov-2005   Zombie         created
*/
const float ncGameCamera::GetMinDistance() const
{
    return this->minDistance;
}

//-----------------------------------------------------------------------------
/**
    Sets the orientation transition type

    history:
        - 25-Nov-2005   Zombie         created
*/
void ncGameCamera::SetTranspositionOrientationType(const transition trans)
{
    this->transOrientationType = trans;
}

//-----------------------------------------------------------------------------
/**
    Gets the orientation transition type

    history:
        - 25-Nov-2005   Zombie         created
*/
const ncGameCamera::transition ncGameCamera::GetTranspositionOrientationType() const
{
    return this->transOrientationType;
}

//-----------------------------------------------------------------------------
/**
    Get internal anchor point

    @returns the anchor point transformation

    history:
        - 25-Nov-2005   Zombie         created
*/
ncTransform* ncGameCamera::GetInternalAnchor()
{
    return this->anchor;
}

//-----------------------------------------------------------------------------
/**
    Sets the lookat offset in y axis

    @param newvalue new offset in the y axis

    history:
        - 03-Feb-2006   Zombie         created
*/
void ncGameCamera::SetLookAtOffset(const float newvalue )
{
    this->offsetLookAt = newvalue;
}

//-----------------------------------------------------------------------------
/**
    Gets the lookat offset in y axis

    @return new offset in the y axis

    history:
        - 03-Feb-2006   Zombie         created
*/
const float ncGameCamera::GetLookAtOffset() const
{
    return this->offsetLookAt;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
