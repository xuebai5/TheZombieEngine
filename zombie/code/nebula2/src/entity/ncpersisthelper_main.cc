//------------------------------------------------------------------------------
//  ncpersisthelper.cc
//  (c) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnentity.h"
#include "entity/ncpersisthelper.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncPersistHelper,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPersistHelper)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncPersistHelper::ncPersistHelper()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncPersistHelper::~ncPersistHelper()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    return normal OID from persistence OID
*/
nEntityObjectId 
ncPersistHelper::FromPersistenceOID( nEntityObjectId oid ) const
{
    return oid;
}

//------------------------------------------------------------------------------
/**
    return persistence OID from normal OID
*/
nEntityObjectId 
ncPersistHelper::ToPersistenceOID( nEntityObjectId oid ) const
{
    return oid;
}
