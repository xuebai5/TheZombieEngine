#include "precompiled/pchngeomipmap.h"
//------------------------------------------------------------------------------
//  ngeomipmapnode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//---------------------------------------------------------------------------
#include "ngeomipmap/ngeomipmapresourceloader.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ngeomipmapresourceloader
    
    @superclass
    nresourceloader

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds_nGeoMipMapResourceLoader(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @param  ps writes the nCmd object contents out to a file.
    @return    success or failure
*/
bool
nGeoMipMapResourceLoader::SaveCmds(nPersistServer* ps)
{
    if (nResourceLoader::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
