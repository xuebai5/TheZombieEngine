//-----------------------------------------------------------------------------
//  nphyboom_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyboom.h"
#include "nphysics/nphygeomray.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyBoom, "nobject");

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 25-Oct-2004   Zombie         created
*/
nPhyBoom::nPhyBoom() :
    boomForce(0),
    boomType(explosion),
    boomLength(0),
    world(0)
{   
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 25-Oct-2004   Zombie         created
*/
nPhyBoom::~nPhyBoom()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Sets the world for this explosion.

    @param world world where this explosion will live

    history:
        - 25-Oct-2004   Zombie         created
*/
void nPhyBoom::GoBoomAt( const vector3& position ) 
{
    n_assert2( this->world, "Is has to belong to a world" );
    
    // storing last position
    this->at = position;

    /// register explosion in to the world
    this->world->RegisterExplosion( *this );
}

//-----------------------------------------------------------------------------
/**
    Assignament operator (deep copy).

    @param boom     an explosion

    history:
        - 25-Oct-2004   Zombie         created
*/
nPhyBoom& nPhyBoom::operator = ( const nPhyBoom& boom )
{
    this->SetWorld(boom.GetWorld());
    this->SetType(boom.Type());
    this->SetForce(boom.GetForce());
    this->SetLength(boom.GetLength());
    this->at = boom.At();
    this->SetRadius(boom.GetRadius());
    this->SetDirection( boom.GetDirection() );
    return *this;
}


//-----------------------------------------------------------------------------
/**
    Checks if affected.

    @return last registered explosion position

    history:
        - 25-Oct-2004   Zombie         created
*/
bool nPhyBoom::IsAffected( const nPhyRigidBody* body ) const
{

    nPhyCollide::nContact contact;

    ncPhysicsObj* phyobj(body->GetPhysicsObj());

    n_assert2( phyobj, "Data corruption" );

    nPhysicsAABB boundingBox;

    phyobj->GetAABB( boundingBox );

    vector3 direction;

    /// checks each point of the object
    for( int i(0); i < 8; ++i ) 
    {   
        boundingBox.GetVertex( i, direction );

        if( this->Type() == blast )
        {
            if( IsBlastAffected(direction) == false )
            {
                continue;
            }
        }

        direction -= this->At();

        if( !nPhysicsServer::Instance()->Collide( this->At(), direction, this-> GetLength(), 1, &contact, nPhysicsGeom::All & ~(nPhysicsGeom::Check |nPhysicsGeom::Ramp) ) )
        {
            continue;
        }

        if( contact.GetGeometryA()->Type() != nPhysicsGeom::Ray )
        {
            if( !contact.GetGeometryA() )
            {
                continue;
            }
            
            if( contact.GetGeometryA()->GetBody() != body )
            {
                continue;
            }
        }
        else
        {
            if( !contact.GetGeometryB() )
            {
                continue;
            }
            
            if( contact.GetGeometryB()->GetBody() != body )
            {
                continue;
            }
        }
        
        return true;
    }

    /// checks the center point
    body->GetPosition( direction );

    if( this->Type() == blast )
    {
        if( IsBlastAffected(direction) == false )
        {
            return false;
        }
    }

    direction -= this->At();

    if( !nPhysicsServer::Instance()->Collide( this->At(), direction, this-> GetLength(), 1, &contact ))
    {
        return false;
    }

    if( contact.GetGeometryA()->Type() != nPhysicsGeom::Ray )
    {
        if( !contact.GetGeometryA() )
        {
            return false;
        }
        
        if( contact.GetGeometryA()->GetBody() != body )
        {
            return false;
        }
    }
    else
    {
        if( !contact.GetGeometryB() )
        {
            return false;
        }
        
        if( contact.GetGeometryB()->GetBody() != body )
        {
            return false;
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
/**
    Checks for a blast.

    @param point where the blast has happened

    history:
        - 02-Dec-2004   Zombie         created
*/
bool nPhyBoom::IsBlastAffected( const vector3& point ) const
{
    // computing angle cone
    phyreal angle(atan2(  this->GetRadius(), this->GetLength() ));

    vector3 baseCone( this->GetDirection()*this->GetLength() );
    
    vector3 newHypotenus( point - this->At() );

    phyreal newangle( baseCone.dot( newHypotenus ) );

    newangle /= baseCone.len() * newHypotenus.len();

    newangle = acos( newangle );

    if( newangle > angle )
    {
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
