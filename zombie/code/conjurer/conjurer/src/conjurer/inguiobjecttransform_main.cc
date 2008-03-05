#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  inguiobjecttransform.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/inguiobjecttransform.h"
#include "entity/nentityobjectserver.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nphysics/ncphysicsobj.h"
#include "ndebug/nceditor.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncsuperentity.h"
#include "zombieentity/ncsubentity.h"

//------------------------------------------------------------------------------
/**
*/
InguiObjectTransform::InguiObjectTransform():
    indoor( 0 )
{
    refPoint = vector3 ( 0.0f, 0.0f, 0.0f );
    refPoint = vector3 ( 0.0f, 0.0f, 0.0f );
}
//------------------------------------------------------------------------------
/**
*/
InguiObjectTransform::~InguiObjectTransform()
{
    // empty
}
//------------------------------------------------------------------------------
/**
    Begin an operation and set the initial matrix
*/
void
InguiObjectTransform::Begin()
{
    this->GetTransform( this->initialMatrix );

    this->refPoint = this->initialMatrix.gettranslation();
}
//------------------------------------------------------------------------------
/**
    End the operation
*/
void
InguiObjectTransform::End()
{
    n_assert( this->refEntity.isvalid() );
    nEntityObjectServer::Instance()->SetEntityObjectDirty( this->refEntity, true );
}
//------------------------------------------------------------------------------
/**
    Cancel the operation
*/
void
InguiObjectTransform::Cancel()
{
    n_assert( this->refEntity.isvalid() );
    ncTransform* tc = this->refEntity->GetComponent<ncTransform>();
    tc->DisableUpdate(ncTransform::cSpatial);
    tc->SetPosition( this->initialMatrix.gettranslation() );
    tc->SetQuat( this->initialMatrix.getquatrotation() );
    tc->EnableUpdate(ncTransform::cSpatial);
    tc->SetScale( this->initialMatrix.getscale() );
}
//------------------------------------------------------------------------------
/**
    Set the initial matrix
*/
void
InguiObjectTransform::SetInitialMatrix( transform44& m )
{
    this->initialMatrix = m;
}
//------------------------------------------------------------------------------
/**
    Get the initial matrix
*/
void
InguiObjectTransform::GetInitialMatrix( transform44& m )
{
    m = this->initialMatrix;
}

//------------------------------------------------------------------------------
/**
    Get the initial position
*/
void
InguiObjectTransform::GetInitialPosition( vector3& p )
{
    p = this->initialMatrix.gettranslation();
}

//------------------------------------------------------------------------------
/**
    Set transform
*/
void
InguiObjectTransform::SetTransform( transform44& t )
{
    if ( !this->refEntity.isvalid() )
    {
        return;
    }

    #define mult_vec_comp( v, w ) vector3( v.x * w.x, v.y * w.y, v.z * w.z)

    matrix44 m( this->initialMatrix.getquatrotation() );
    vector3 p = this->initialMatrix.gettranslation();

    m.set_translation( p - this->refPoint );

    matrix44 rot;
    if ( t.iseulerrotation() )
    {
        vector3 angles = t.geteulerrotation();
        rot.rotate_x(angles.x);
        rot.rotate_y(angles.y);
        rot.rotate_z(angles.z);
    }
    else
    {
        rot.set( t.getquatrotation() );
    }

    rot.set_translation( t.gettranslation() );

    m = m * rot;

    vector3 s = mult_vec_comp( this->initialMatrix.getscale(), t.getscale());
    vector3 relPos = m.pos_component();
    relPos = mult_vec_comp( relPos, t.getscale());
    m.set_translation( relPos );

    m.translate( this->refPoint );
    
    // Terrain collision checking
    if ( this->collisionHeightmap )
    {
        if ( this->indoor )
        {
            // The entity is transformed because it's contained in an indoor. So heightmap collision is linked to its parent one
            // ( the indoor is already transformed because it is before in the object transform array )

            ncTransform* indoorTrnsf = indoor->GetComponent<ncTransform>();
            vector3 p = indoorTrnsf->GetPosition();

            float h;
            vector3 n;
            if ( this->collisionHeightmap->GetHeightNormal( p.x, p.z, h, n ) && abs(p.y - h) < TINY )
            {
                h = p.y;
                p = m.pos_component();
                p.y = h + this->initialMatrix.gettranslation().y - refPoint.y;
                m.set_translation( p );
            }
        }
        else
        {

            vector3 refTransformed = m * this->refPoint;

            vector3 p = m.pos_component();
            vector3 n;
            float h;
            bool inHeightmap = this->collisionHeightmap->GetHeightNormal( p.x, p.z, h, n );
            if ( ( this->terrainCollisionType == colSetAlways || ( this->terrainCollisionType == colLessEqual && p.y <= h ) ) && inHeightmap )
            {
                p.y = h;
                m.set_translation( p );
            }
        }
    }
    quaternion q = m.get_quaternion();

    ncTransform* tc = this->refEntity->GetComponent<ncTransform>();
    tc->DisableUpdate(ncTransform::cSpatial);
    tc->SetPosition( m.pos_component() );
    tc->SetQuat( q );
    tc->EnableUpdate(ncTransform::cSpatial);
    tc->SetScale( s );

    // Add entity to physic space
    ncPhysicsObj* phyComp = this->refEntity->GetComponent<ncPhysicsObj>();
    if ( phyComp )
    {
        phyComp->AutoInsertInSpace();
    }

    // Update subentity relative transform
    ncSubentity* se = this->refEntity->GetComponent<ncSubentity>();
    if ( se )
    {
        se->UpdateRelativePosition();
    }

    // Update child subentities relative position
    ncSuperentity* supEnt = this->refEntity->GetComponent<ncSuperentity>();
    if ( supEnt )
    {
        supEnt->UpdateSubentities();
    }

    nEntityObjectServer::Instance()->SetEntityObjectDirty( this->refEntity, true );
}
//------------------------------------------------------------------------------
/**
    Get transform
*/
void
InguiObjectTransform::GetTransform( transform44& t )
{
    if ( this->refEntity.isvalid() )
    {
        ncTransform* tc = this->refEntity->GetComponent<ncTransform>();
        t.settranslation( tc->GetPosition() );
        t.setquatrotation( tc->GetQuat() );
        t.setscale( tc->GetScale() );
    }
}
//------------------------------------------------------------------------------
/**
    Set entity
*/
void
InguiObjectTransform::SetEntity( nEntityObject *ent )
{
    n_assert( ent );
    ncTransform* tc = ent->GetComponent<ncTransform>();

    if ( tc )
    {
        this->refEntity = ent;
    }
    else
    {
        this->refEntity = 0;
    }
}
//------------------------------------------------------------------------------
/**
    Get entity
*/
nEntityObject*
InguiObjectTransform::GetEntity()
{
    if ( this->refEntity.isvalid() )
    {
        return this->refEntity;
    }else
    {
        return 0;
    }
}
//------------------------------------------------------------------------------
/**
    Set reference point
*/
void
InguiObjectTransform::SetReferencePoint( vector3 ref )
{
    this->refPoint = ref;
}

//------------------------------------------------------------------------------
/**
    @brief Set collision heightmap
*/
void
InguiObjectTransform::SetOutdoor( nEntityObject * outdoor, TerrainCollisionType colType )
{
    this->terrainCollisionType = colType;

    if ( outdoor )
    {
        ncTerrainGMMClass * comp = outdoor->GetClassComponent<ncTerrainGMMClass>();
        n_assert(comp);
        this->collisionHeightmap = comp->GetHeightMap();
    }
    else
    {
        this->collisionHeightmap = 0;
    }
}

//------------------------------------------------------------------------------

/**
    Set containing indoor
*/
void
InguiObjectTransform::SetContainingIndoor( nEntityObject* indoor )
{
    this->indoor = indoor;
}

//------------------------------------------------------------------------------

/**
    Get containing indoor
*/
nEntityObject*
InguiObjectTransform::GetContainingIndoor()
{
    return this->indoor;
}

//------------------------------------------------------------------------------
