//-----------------------------------------------------------------------------
//  nphypace_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyspace.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhySpace, "nphysicsgeom");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 21-Mar-2005   Zombie         created
*/
nPhySpace::nPhySpace() : 
    spaceWorld(0),
    spaceName(""),
    holdedSpaceID(NoValidID)
{
    // empty
}


//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 21-Mar-2005   Zombie         created
*/
nPhySpace::~nPhySpace()
{
    if( this->Id() != NoValidID )
    {
        for( ; this->GetNumGeometries(); )
        {
            this->GetGeometry(0)->MoveToSpace(0);
        }

        this->MoveToSpace(0);
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the geometry

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::Create()
{
    n_assert2( this->Id() != NoValidID , "Hasn't been possible to create the space" );

    nPhysicsGeom::Create();
}

//-----------------------------------------------------------------------------
/**
    Adds a geometry to the space

    @param geometry      geometry to be added to the space

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::Add( nPhysicsGeom* geometry )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );

    n_assert2( geometry , "Null pointer" );

    phyAddToSpace( this->Id(), geometry->Id() );    

    if( geometry->IsSpace() )
    {
        static_cast<nPhySpace*>(geometry)->SetWorld(this->GetWorld());
    }
}

//-----------------------------------------------------------------------------
/**
    Removes a geometry to the space

    @param geometry      geometry to be removed to the space

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::Remove( nPhysicsGeom* geometry )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );
    n_assert2( geometry , "Null pointer" );

    phyRemoveFromSpace( this->Id(), geometry->Id() );

    if( geometry->IsSpace() )
    {
        static_cast<nPhySpace*>(geometry)->SetWorld(0);
    }    
}

//-----------------------------------------------------------------------------
/**
    Enables this space and the geometris within

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::Enable()
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );
   

    if( this->IsEnabled() )
    {
        return;
    }

    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        nPhysicsGeom* geom(this->GetGeometry(index));
        
        n_assert2( geom, "Data corruption" );

        if( geom->GetOwner() )
        {
            if( !geom->GetOwner()->IsEnabled() )
            {
                geom->GetOwner()->Enable();
            }
        }
        else
        {
            geom->Enable();
        }
    }

    nPhysicsGeom::Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables this space and the geometris within

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::Disable()
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );
   

    if( !this->IsEnabled() )
    {
        return;
    }

    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        nPhysicsGeom* geom(this->GetGeometry(index));
        
        n_assert2( geom, "Data corruption" );

        if( geom->GetOwner() )
        {
            if( geom->GetOwner()->IsEnabled() )
            {
                geom->GetOwner()->Disable();
            }
        }
        else
        {
            geom->Disable();
        }
    }

    nPhysicsGeom::Disable();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the geometries contained in this space.

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::Draw( nGfxServer2* server )
{
    n_assert2( this->Id() != NoValidID , "No valid geometry" );

    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        this->GetGeometry(index)->Draw(server);
    }

    nPhysicsGeom::Draw( server );
}
#endif
//-----------------------------------------------------------------------------
/**
    Sets the world of this space.

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::SetWorld( nPhysicsWorld* world )
{
    this->spaceWorld = world;

    if( this->Id() != NoValidID )
    {
        // check for sub-spaces and update world
        for( int index(0); index < this->GetNumGeometries(); ++index )
        {
            if( this->GetGeometry(index)->IsSpace() )
            {
                static_cast<nPhySpace*>(this->GetGeometry(index))->SetWorld( world );
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the position of this space.

    @param newposition new world position

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::SetPosition( const vector3& /*newposition*/ )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Sets the orientation of this space.

    @param ax new ax angle orientation
    @param ay new ay angle orientation
    @param az new az angle orientation

    history:
        - 21-Mar-2005   Zombie         created
*/
void nPhySpace::SetOrientation( phyreal /*ax*/, phyreal /*ay*/, phyreal /*az*/ )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Returns current position.

    @param position space's position

    history:
        - 22-Mar-2005   Zombie         created
*/
void nPhySpace::GetPosition( vector3& /*position*/ ) const
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Returns current orientation.

    @param orientation space orientation

    history:
        - 22-Mar-2005   Zombie         created
*/
void nPhySpace::GetOrientation( matrix33& /*orientation*/ ) const
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Returns current angles.

    @param ax ax angle orientation
    @param ay ay angle orientation
    @param az az angle orientation

    history:
        - 22-Mar-2005   Zombie         created
*/
void nPhySpace::GetOrientation( phyreal& /*ax*/, phyreal& /*ay*/, phyreal& /*az*/ )
{
    // Empty
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
