//------------------------------------------------------------------------------
//  ncpersisthelpersubentity_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchncshared.h"
#include "zombieentity/ncpersisthelpersubentity.h"
#include "zombieentity/ncsubentity.h"
#include "entity/nentityobjectserver.h"


//------------------------------------------------------------------------------
nNebulaComponentObject(ncPersistHelperSubEntity,ncPersistHelper);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPersistHelperSubEntity)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncPersistHelperSubEntity::ncPersistHelperSubEntity()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncPersistHelperSubEntity::~ncPersistHelperSubEntity()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    return normal OID from persistence OID
*/
nEntityObjectId 
ncPersistHelperSubEntity::FromPersistenceOID( nEntityObjectId oid ) const
{
    ncSubentity * subentity = this->GetComponent<ncSubentity>();

    if (subentity)
    {
        // If subentity is linked, return subentity id
        if ( nEntityObjectServer::Local == nEntityObjectServer::Instance()->GetEntityObjectType( oid ) )
        {
            if ( ! subentity->GetSuperentity() )
            {
                nEntityClass* cl = this->GetEntityClass();
                n_assert( cl );
                n_error("Error: Local entity reference found in not linked subentity (local ident = %d, entity class = %s)", oid, cl->GetFullName());
                return 0;
            }
            // Search subentity in the superentity with persistence id and convert the id to the runtime id
            if ( subentity->SearchEntity( oid ) )
            {
                return oid;
            }
            else
            {
                nEntityClass* cl = this->GetEntityClass();
                n_assert( cl );
                n_error("Error: Failed to resolve a local entity reference (local ident = %d, entity class = %s)", oid, cl->GetFullName());
                return 0;
            }
        }
    }

    return oid;
}

//------------------------------------------------------------------------------
/**
    return persistence OID from normal OID
*/
nEntityObjectId 
ncPersistHelperSubEntity::ToPersistenceOID( nEntityObjectId oid ) const
{
    ncSubentity * subentity = this->GetComponent<ncSubentity>();

    if (subentity)
    {
        // If oid is local
        if ( nEntityObjectServer::Local == nEntityObjectServer::Instance()->GetEntityObjectType( oid ) )
        {
            // Return persisted oid
            return subentity->GetPersistenceOID();
        }
    }

    // Else return normal id
    return oid;
}
