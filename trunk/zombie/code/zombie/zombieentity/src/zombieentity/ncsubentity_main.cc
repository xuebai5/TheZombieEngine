#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncsubentity_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "zombieentity/ncsubentity.h"
#include "zombieentity/nctransform.h"
#include "entity/nobjectmagicinstancer.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/ncsuperentity.h"
#include "nphysics/ncphysicsobj.h"
#include "zombieentity/ncsuperentity.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif //NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSubentity, nComponentObject);

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( ncSubentity, UpdateEntityReferences );
NSIGNAL_DEFINE( ncSubentity, SubEntitiesCreated);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSubentity)

    NSCRIPT_ADDCMD_COMPOBJECT('JSRP', void, SetRelativePosition, 1, (const vector3 &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CSRS', void, SetRelativeScale, 1, (const vector3 &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGRP', const vector3 &, GetRelativePosition, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSRT', void, SetRelativeTransform, 1, (const quaternion &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGRT', const quaternion &, GetRelativeTransform, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSPI', void, SetPersistenceOID, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGPI', nEntityObjectId, GetPersistenceOID, 0, (), 0, ());

    cl->BeginSignals(1);
    NCOMPONENT_ADDSIGNAL( UpdateEntityReferences );
    cl->EndSignals();
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncSubentity::ncSubentity() :
    /// @todo Trick to set to old version, put 0 in scale , backward compatibility with old persistence
    relativeScale(0.f,0.f,0.f) 
{ 
    #ifndef NGAME
    superEntity = 0;
    #endif//!NGAME
}


//------------------------------------------------------------------------------
/**
    Destructor
*/
ncSubentity::~ncSubentity()
{
    // empty   
}

//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncSubentity::InitInstance(nObject::InitInstanceMsg initType)
{

    switch ( initType )
    {
    case nObject::LoadedInstance:
        /// todo this is a trick for mark the current superentity, this is not ok with thread
        this->superEntity = ncSuperentity::GetCurrentSuperEntity();
        //not break
    case nObject::ReloadedInstance:
        {
            ncTransform* tr = this->GetComponentSafe<ncTransform>();
            ncTransform::compUpdate previousFlag = tr->GetUpdateFlag();
            tr->DisableUpdate( ncTransform::allComponents );
            this->UpdatenTrasformFromRelativePosition();
            tr->EnableUpdate( previousFlag );
        }
        break;
    default:
        ;//empty
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Update relative position of subentity if linked to a owner entity
*/
void
ncSubentity::UpdateRelativePosition()
{
    if ( this->superEntity )
    {
        // Get sub and super transforms
        ncTransform* supTrComp = this->superEntity->GetComponent<ncTransform>();
        n_assert( supTrComp );
        ncTransform* subTrComp = this->entityObject->GetComponent<ncTransform>();
        n_assert( subTrComp );
        matrix44d invParent4x4(supTrComp->GetTransform());
        invParent4x4.invert_simple();
        
        // Get matrix of subentity relative to super
        
        matrix44d global4x4(subTrComp->GetTransform());
        matrix44d local4x4 = global4x4 * invParent4x4;

        local4x4.get(this->relativeScale, this->relativeTransform, this->relativePos);

        ncSuperentity* supEnt = this->superEntity->GetComponentSafe<ncSuperentity>();
        supEnt->UpdateAABB();

        //Coherent transform load and save, dirty trick 
        if ( this->relativeScale.x != 0.f) //if it detect old version then prevent infinite recursion
        {
            this->UpdatenTrasformFromRelativePosition();
        }
        
    }
}
#endif//!NGAME

//------------------------------------------------------------------------------
/**
    @brief Get editor owner entity
*/
nEntityObject*
ncSubentity::GetSuperentity()
{
    return this->superEntity;
}

//------------------------------------------------------------------------------
/**
    @brief Set editor owner entity
*/
void
ncSubentity::SetSuperentity( nEntityObject* obj )
{
    n_assert( obj );
    this->superEntity = obj;
}

//------------------------------------------------------------------------------
/**
    SetRelativePosition
*/
void
ncSubentity::SetRelativePosition (const vector3 &position)
{
    this->relativePos = position;
}


//------------------------------------------------------------------------------
/**
    SetRelativePosition
*/
void
ncSubentity::SetRelativeScale(const vector3 &scale)
{
    this->relativeScale = scale;
}

//------------------------------------------------------------------------------
/**
    GetRelativePosition
*/
const
vector3 &
ncSubentity::GetRelativePosition() const
{
    return this->relativePos;
}

//------------------------------------------------------------------------------
/**
    SetRelativeTransform
*/
void
ncSubentity::SetRelativeTransform (const quaternion & q)
{
    this->relativeTransform = q;
}

//------------------------------------------------------------------------------
/**
    GetRelativeTransform
*/
const
quaternion &
ncSubentity::GetRelativeTransform() const
{
    return this->relativeTransform;
}

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncSubentity::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        if ( this->superEntity )
        {
            // -- relativePos
            ps->Put (this->entityObject, 'JSRP', this->relativePos.x, this->relativePos.y, this->relativePos.z);

            // -- relative tranform
            ps->Put (this->entityObject, 'JSRT', this->relativeTransform.x, this->relativeTransform.y, this->relativeTransform.z, this->relativeTransform.w);

            ps->Put (this->entityObject, 'CSRS', this->relativeScale.x, this->relativeScale.y, this->relativeScale.z);

            // -- persisted subentity id
            ps->Put (this->entityObject, 'JSPI', this->subEntityId);
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Search an OID in this superentity's list of subentities.
    @return True if found, and the runtime OID is returned in the parameter. False otherwise
*/
bool
ncSubentity::SearchEntity( nEntityObjectId& roid ) const
{
    ncSuperentity* supComp = this->superEntity->GetComponent<ncSuperentity>();
    n_assert( supComp );
    nObjectInstancer* subs = supComp->GetSubentities();
    n_assert( subs );

    // Search oid in the superentity object instancer
    for (int subIndex = 0; subIndex < subs->Size(); subIndex++)
    {
        nObject* obj = subs->At( subIndex );
        n_assert( obj->IsA("nentityobject") );
        nEntityObject* entObj = static_cast<nEntityObject*>( obj );
        ncSubentity* subEnt = entObj ->GetComponent<ncSubentity>();
        if ( !subEnt )
        {
            nEntityClass* cl = this->superEntity->GetEntityClass();
            n_assert( cl );
            n_error("Error: Found a non-subentity entity (oid=%d) belonging to a superentity (class=%s)", roid, cl->GetFullName() );
        }
        if ( subEnt->GetPersistenceOID() == roid )
        {
            // Found it
            roid = entObj->GetId();
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Set persistence oid
*/
void
ncSubentity::SetPersistenceOID( nEntityObjectId oid )
{
    this->subEntityId = oid;
}

//------------------------------------------------------------------------------
/**
    @brief Get persistence oid
*/
nEntityObjectId
ncSubentity::GetPersistenceOID()
{
    return this->subEntityId;
}

//------------------------------------------------------------------------------
/**
    @returns true if entity is a subentity
*/
bool
ncSubentity::IsSubentity() const
{
    return ( this->superEntity != 0 );
}

//------------------------------------------------------------------------------
/**
    @returns true if entity is a subentity
*/
void
ncSubentity::UpdatenTrasformFromRelativePosition()
{
    if ( this->superEntity)
    {
        ncTransform* supTrComp = this->superEntity->GetComponent<ncTransform>();
        ncTransform* transfComp = this->GetComponent<ncTransform>();
        n_assert(supTrComp);
        n_assert(transfComp);

        // obsolete version
        bool oldPersistenceVersion = (this->relativeScale.x == 0.f); //Exactly 0
        if (oldPersistenceVersion)
        {
            //backward compability, the same transform with obsolete code.
            const vector3 &relPos = this->GetRelativePosition();
            vector3 absPos = supTrComp->GetTransform().transform_coord( relPos );

            transfComp->SetPosition( absPos );

            matrix44 relTr = matrix44( this->GetRelativeTransform() );
            matrix44 globalTr( supTrComp->GetQuat() );

            globalTr = relTr * globalTr;

            quaternion q0 =  globalTr.get_quaternion() ;
            q0.normalize(); // remove


            transfComp->SetQuat( q0);


            #ifndef NGAME
            // Change the old relative position to new version of relative position
            this->UpdateRelativePosition();
            #endif
        } else
        {
            matrix44 local4x4;
            matrix44 parent4x4(supTrComp->GetTransform());
            vector3 globalScale, globalPos;
            quaternion globalQuat;


            local4x4.scale(this->relativeScale);
            local4x4.mult_simple(matrix44(this->relativeTransform));
            local4x4.translate(this->relativePos);      

        
            // Get matrix of subentity relative to super        
            matrix44 global4x4(local4x4 * parent4x4);
            global4x4.get(globalScale, globalQuat, globalPos);

            transfComp->SetScale(globalScale);
            transfComp->SetQuat(globalQuat);
            transfComp->SetPosition(globalPos);

        }

        ncPhysicsObj* phyComp = this->GetComponent<ncPhysicsObj>();
        if ( phyComp )
        {
            phyComp->AutoInsertInSpace();
        }

    }
}


//------------------------------------------------------------------------------
