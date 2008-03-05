//------------------------------------------------------------------------------
//  ncphyindoorclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

#include "nphysics/ncphyindoorclass.h"

#include "zombieentity/ncloaderclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncPhyIndoorClass,ncPhysicsObjClass);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyIndoorClass)
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
/**
    Returns the indoor physics path.

    @param path physics chunck information directory

    history:
        - 31-May-2005   David Reyes    created
*/
void ncPhyIndoorClass::GetPath( nString& path ) const
{
    const ncLoaderClass * loader(this->GetComponent<ncLoaderClass>());
    
    n_assert2( loader, "Null pointer." );

    path = loader->GetResourceFile();
    path.StripExtension();
    path.Append("/physics/");    
}
