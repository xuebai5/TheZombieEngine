//-----------------------------------------------------------------------------
//  ncphyragdolllimb_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyragdolllimb.h"
#include "nphysics/nphysicsgeom.h"
#include "nphysics/ncphyragdoll.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyRagDollLimb,ncPhyCompositeObj);

//-----------------------------------------------------------------------------

/// definition of a none valid id
const ncPhyRagDollLimb::rdId ncPhyRagDollLimb::rdNoValidID(0);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 09-Mar-2005   David Reyes    created
*/
ncPhyRagDollLimb::ncPhyRagDollLimb() :
    ragDoll(0),
    ragdollId(rdNoValidID)
{
    this->SetType( RagDoll );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 09-Mar-2005   David Reyes    created
*/
ncPhyRagDollLimb::~ncPhyRagDollLimb()
{
    // Empty
    if( this->GetRagDoll() )
    {
        this->GetRagDoll()->RemoveLimb( this );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets what is the rag-doll.

    @param ragdoll ragdoll object

    history:
        - 09-Mar-2005   David Reyes    created
*/
void ncPhyRagDollLimb::SetRagDoll( ncPhyRagDoll* ragdoll )
{
    this->ragDoll = ragdoll;
}

//-----------------------------------------------------------------------------
/**
    Gets the owner of this limb.

    @return ragdoll object

    history:
        - 09-Mar-2005   David Reyes    created
*/
ncPhyRagDoll* ncPhyRagDollLimb::GetRagDoll() const
{
    return this->ragDoll;
}

//-----------------------------------------------------------------------------
/**
    Returns if the limb it's free from the body.

    @return true/false

    history:
        - 09-Mar-2005   David Reyes    created
*/
const bool ncPhyRagDollLimb::IsFree() const
{
    // TODO: Implement
    return false;
}

//-----------------------------------------------------------------------------
/**
    Function to be procesed during the collision check.

    @param numContacts maximun number of contacts expected
    @param contacts container of contacts

    @return if any collision was processed

    history:
        - 09-Mar-2005   David Reyes    created
*/
bool ncPhyRagDollLimb::CollisionProcess( int numContacts, nPhyCollide::nContact* contacts )
{
    return this->ragDoll->CollisionProcess( numContacts, contacts );
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 30-Aug-2005   David Reyes    created
*/
void ncPhyRagDollLimb::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhyCompositeObj::InitInstance(initType);

#ifndef NGAME
    // make it not selectable
    ncEditor* editor( this->GetComponent<ncEditor>() );

    n_assert2( editor, "The component ncEditor must be present in this entity." );

    editor->SetEditorKeyInt( "editable", 0 );
#endif
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world it's the world where the object will be come to existance

    history:
        - 06-Sep-2005   David Reyes    created
*/
void ncPhyRagDollLimb::Create( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer" );

    if( this->IsSubSpaceAllowed() )
    {
        if( this->GetNumGeometries() > 1 )
        {
            // if more than one geometry creating a sub-space
            this->CreateSpace();

            // adding each geometry to the sub-space
            for( int index(0); index < this->GetNumGeometries(); ++index )
            {
                this->GetGeometry(index)->MoveToSpace( this->objectSpace );
            }
        }
    }

    ncPhysicsObj::Create( world );

    nPhyRigidBody *body(this->GetBody());

    if( body )
    {
        this->SetBody( body );

        /// assigns to each geometry de body
        body->Update( this->containerGeometries, this->GetMass(), this->GetDensity() );

        body->SetPhysicsObj( this );

        if( this->objectSpace )
        {
            this->objectSpace->SetCategories( this->GetCategories() );
            this->objectSpace->SetCollidesWith( this->GetCollidesWith() );
        }
    }

    this->GetBody()->SetContactWithOtherBodies( false );

    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        static_cast<nPhyGeomTrans*>(this->GetGeometry( index ))->GetGeometry()->SetGameMaterial( "dead" );
    }
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
