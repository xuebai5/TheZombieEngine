//------------------------------------------------------------------------------
//  ncgprocket_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "zombieentity/nctransform.h"

#include "rnsgameplay/ncgprocket.h"
#include "rnsgameplay/ncgprocketclass.h"
#include "rnsgameplay/ngameplayutils.h"

#include "nphysics/ncphycompositeobj.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsworld.h"
#include "nphysics/nphygeomray.h"

#include "rnsgameplay/ncgpexplosion.h"
#include "gameplay/nfxeventtriggertypes.h"

#ifndef NGAME
#include "napplication/nappviewport.h"

#include "ndebug/ndebugtrail.h"
#include "ndebug/ndebuggraphicsserver.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPRocket,ncGPThrowable);

//------------------------------------------------------------------------------
/**
*/
ncGPRocket::ncGPRocket():
    lifeTime (0),
    meters (0.0f),
    object (0),
    transform(0),
    ray( 0 )
#ifndef NGAME
    ,trail( 0 )
#endif//!NGAME
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPRocket::~ncGPRocket()
{
    if( this->ray )
    {
        this->ray->Release();
    }
#ifndef NGAME
    if( this->trail )
    {
        nDebugGraphicsServer::Instance()->Kill( this->trail );
    }
#endif//!NGAME
}

//------------------------------------------------------------------------------
/**
    @param dir initial direction
    @param pos initial position
*/
void
ncGPRocket::SetInitial( const vector3 & dir, const vector3 & pos )
{
    this->direction = dir;
    this->direction.norm();
    this->initialPos = pos;

    ncGPRocketClass * rocketClass = this->GetClassComponent<ncGPRocketClass>();
    this->finalPos = this->initialPos + this->direction * ( rocketClass->GetMaxMeters() / 2.0f );
}

//------------------------------------------------------------------------------
/**
*/
void
ncGPRocket::UpdateObjectDirection()
{
    vector3 base( 0, 0, -1 );
    quaternion q;
    q.set_from_axes( base, this->direction );

    this->transform->SetQuat( q );
}

//------------------------------------------------------------------------------
/**
*/
void
ncGPRocket::Create( )
{
    nPhysicsWorld * world = static_cast<nPhysicsWorld*>( nKernelServer::ks->Lookup( "/usr/physics/world" ) );
    n_assert2( world, "No physic world created" );
    if( !world )
    {
        return;
    }

    this->transform = this->GetComponent<ncTransform>();
    n_assert( this->transform );

    this->object = this->GetComponent<ncPhyCompositeObj>();
    n_assert( this->object )
    if( ! this->object )
    {
        return;
    }

    nPhyMaterial * mat = static_cast<nPhyMaterial*>( nKernelServer::ks->Lookup( "/usr/physics/rocket_mat" ) );
    if( ! mat )
    {
        mat = static_cast<nPhyMaterial*>( nKernelServer::ks->New( "nphymaterial", "/usr/physics/rocket_mat" ) );
        mat->SetCoeficientNormalFriction( phyreal( 0.5 ) );
        mat->SetCoeficientBounce( phyreal( 0.1 ) );
        mat->Update();
    }
    n_assert2( mat, "No material created" );
    if( ! mat )
    {
        return;
    }

    this->object->Enable();
    this->object->SetMaterial( mat->Id() );
    this->object->SetCategories( nPhysicsGeom::Dynamic );
    this->object->SetCollidesWith( -1 );

    this->object->SetPosition( this->initialPos );
    this->UpdateObjectDirection();

    ncGPRocketClass * rocketClass = this->GetClassComponent<ncGPRocketClass>();
    this->object->AddForce( this->direction * rocketClass->GetInitialThrustPower() );

    this->power = rocketClass->GetInitialThrustPower();
    this->lastPos = this->initialPos;
    this->actualPos = this->initialPos;

    this->SetValid( true );

    // create intersection ray
    if( ! this->ray )
    {
        this->ray = static_cast<nPhyGeomRay*>( nKernelServer::ks->New( "nphygeomray" ) );
        n_assert( this->ray );
        if( this->ray )
        {
            this->ray->SetCategories(0);
            this->ray->SetCollidesWith( ~nPhysicsGeom::Check );
        }
    }

#ifndef NGAME
    this->trail = nDebugGraphicsServer::Instance()->NewDebugTrail();
    n_assert( this->trail );
    if( this->trail )
    {
        this->trail->InsertPoint( this->initialPos );
    }
#endif//!NGAME
}

//------------------------------------------------------------------------------
/**
    @param deltaTime time transcurred since last call in seconds
*/
void
ncGPRocket::Run ( const float deltaTime )
{
    if( ! this->IsValid() )
    {
        return;
    }

    // update rocket
    this->lifeTime += deltaTime;
    this->lastPos = this->actualPos;
    this->object->GetPosition( this->actualPos );
    this->meters += vector3::distance( this->lastPos, this->actualPos );

#ifndef NGAME
    if( this->trail )
    {
        this->trail->InsertPoint( actualPos );
    }
#endif//!NGAME

    ncGPRocketClass * rocketClass = this->GetClassComponent<ncGPRocketClass>();

    // apply thrust
    if( this->lifeTime < rocketClass->GetMaxThrustTime() )
    {
        // update direction
        vector3 newDirection;
        newDirection = this->finalPos - actualPos;
        newDirection.norm();
        float angle = this->direction.dot( newDirection );
        float value = vector3::distance( this->direction, newDirection );
        if( angle < 0.2 && value < 1.0f )
        {
            this->direction = newDirection;
        }

        this->object->AddForce (this->direction * this->power * deltaTime );
        this->power = this->power * ( 1.0f / (1.0f + 0.01f * deltaTime) );

        this->UpdateObjectDirection();
    }

    // check for termination
    if( this->IsDone() )
    {
        ncGPExplosion * explosion = this->GetComponent<ncGPExplosion>();
        if( explosion )
        {
            explosion->Explode( this->actualPos - this->lastPos );
        }
        this->SetValid( false );
    }
    else
    {
        ncGameplay::Run (deltaTime);
    }
}

//------------------------------------------------------------------------------
/**
    @retval true if grenade is done
*/
bool
ncGPRocket::IsDone()
{
    // collision with objects
    const int MAX_CONTACTS = 10;
    nPhyCollide::nContact contacts[MAX_CONTACTS];
    int num( this->object->Collide( 1, contacts ) );
    if( num )
    {
        return true;
    }

    // intersection with objects
    if( this->ray )
    {
        vector3 direction = this->actualPos - this->lastPos;
        if( direction.len() > 0 )
        {
            this->ray->SetLength( direction.len() );
            direction.norm();
            this->ray->SetDirection( direction );
            this->ray->SetPosition( this->lastPos );
            this->object->Disable();
            num = nPhysicsServer::Instance()->Collide( ray, MAX_CONTACTS, contacts );
            this->object->Enable();
            if( num )
            {
                // get first contact
                vector3 contactPosition;
                float minDist = FLT_MAX;
                float dist = 0;
                int ind = 0;
                for( int i = 0 ; i < num ; ++i )
                {
                    contacts[i].GetContactPosition( contactPosition );
                    dist = ( contactPosition - this->lastPos ).lensquared();
                    if( dist < minDist )
                    {
                        minDist = dist;
                        ind = i;
                    }
                }

                // adjust rocket position to contact position
                contacts[ind].GetContactPosition( contactPosition );
                this->object->SetPosition( contactPosition );
                return true;
            }
        }
    }

    ncGPRocketClass * rocketClass = this->GetClassComponent<ncGPRocketClass>();

    return ( this->lifeTime > rocketClass->GetMaxLifeTime() || this->meters > rocketClass->GetMaxMeters() );
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @param gfxserver graphics server
*/
void
ncGPRocket::DebugDraw( nGfxServer2 * const gfxServer ) 
{
    n_assert( gfxServer );
    if( ! gfxServer )
    {
        return;
    }

    vector3 pos( 0.0f, 0.0f, 0.0f );
    if( this->object )
    {
        this->object->GetPosition( pos );
    }
    
    nAppViewport * viewport = static_cast<nAppViewport*>( nKernelServer::ks->Lookup( "/usr/rnsview" ) );
    n_assert( viewport );
    if( ! viewport )
    {
        return;
    }

    matrix44 matV = viewport->GetViewMatrix();
    matV.invert();
    matV *= nCamera2( viewport->GetCamera() ).GetProjection();
    vector4 pos4;
    pos4 = pos;
    pos4 = matV * pos4;

    if( pos4.w <= 0 )
    {
        return;
    }

    pos4.x = pos4.x / pos4.w;
    pos4.y = -pos4.y / pos4.w;

    ncGPRocketClass * rocketClass = this->GetClassComponent<ncGPRocketClass>();

    nString text;
    if( this->lifeTime < rocketClass->GetMaxThrustTime() )
    {
        text = "thrust: ";
        text.Append( nString( float( rocketClass->GetMaxThrustTime() - this->lifeTime ) ) );
    }
    else
    {
        text = "no thrust";
    }
    gfxServer->Text( text.Get(), vector4(0,1,0,1), pos4.x + 0.01f, pos4.y + 0.01f );
    text = "lifetime: ";
    text.Append( nString ( float(this->lifeTime)) );    
    gfxServer->Text( text.Get(), vector4(0,1,0,1), pos4.x + 0.01f, pos4.y + 0.01f + 0.03f );
    text = "meters: ";
    text.Append( nString( static_cast<int>( this->meters ) ) );
    gfxServer->Text( text.Get(), vector4(0,1,0,1), pos4.x + 0.01f, pos4.y + 0.01f + 0.06f );
}
#endif//!NGAME

//------------------------------------------------------------------------------
