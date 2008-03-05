//------------------------------------------------------------------------------
//  ncgpgrenade_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpgrenade.h"
#include "rnsgameplay/ncgpgrenadeclass.h"
#include "rnsgameplay/ngameplayutils.h"

#include "nphysics/ncphycompositeobj.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsworld.h"

#include "rnsgameplay/ncgpexplosion.h"
#include "gameplay/nfxeventtriggertypes.h"

#ifndef NGAME
#include "napplication/nappviewport.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPGrenade,ncGPThrowable);

//------------------------------------------------------------------------------
/**
*/
ncGPGrenade::ncGPGrenade():
    lifeTime (0),
    object (0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPGrenade::~ncGPGrenade()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param dir initial direction
    @param pos initial position
*/
void
ncGPGrenade::SetInitial( const vector3 & dir, const vector3 & pos )
{
    this->direction = dir;
    this->initialPos = pos;
}

//------------------------------------------------------------------------------
/**
*/
void
ncGPGrenade::Create( )
{
    nPhysicsWorld * world = static_cast<nPhysicsWorld*>( nKernelServer::ks->Lookup( "/usr/physics/world" ) );
    n_assert2( world, "No physic world created" );
    if( !world )
    {
        return;
    }

    this->object = this->GetComponent<ncPhyCompositeObj>();
    n_assert( this->object )
    if( ! this->object )
    {
        return;
    }

    nPhyMaterial * mat = static_cast<nPhyMaterial*>( nKernelServer::ks->Lookup( "/usr/physics/grenade_mat" ) );
    if( ! mat )
    {
        mat = static_cast<nPhyMaterial*>( nKernelServer::ks->New( "nphymaterial", "/usr/physics/grenade_mat" ) );
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

    world->Add( this->GetEntityObject() );

    ncGPGrenadeClass* grenadeClass = this->GetClassComponent<ncGPGrenadeClass>();
    this->object->AddForce( this->direction * grenadeClass->GetLaunchPower() );

    this->SetValid( true );
}

//------------------------------------------------------------------------------
/**
    @param deltaTime time transcurred since last call in seconds
*/
void 
ncGPGrenade::Run ( const float deltaTime )
{
    if( ! this->IsValid() )
    {
        return;
    }

    // update grenade
    this->lifeTime += deltaTime;

    // check for termination
    if( this->IsDone() )
    {
        ncGPExplosion * explosion = this->GetComponent<ncGPExplosion>();
        if( explosion )
        {
            explosion->Explode( vector3(0.f, -1.f, 0.f) );
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
ncGPGrenade::IsDone()
{
    ncGPGrenadeClass* grenadeClass = this->GetClassComponent<ncGPGrenadeClass>();

    return (this->lifeTime > grenadeClass->GetMaxLifeTime() );
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @param gfxserver graphics server
*/
void 
ncGPGrenade::DebugDraw( nGfxServer2 * const gfxServer )
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

    nString text;
    text.Append( nString ( (int)this->lifeTime) );
    gfxServer->Text( text.Get(), vector4(0,1,0,1), pos4.x + 0.01f, pos4.y + 0.01f );
}
#endif//!NGAME

//------------------------------------------------------------------------------
