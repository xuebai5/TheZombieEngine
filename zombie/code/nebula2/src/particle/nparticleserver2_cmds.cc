//------------------------------------------------------------------------------
//  nparticleserver_cmds.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "particle/nparticleserver2.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nparticleserver
    
    @cppclass
    nParticleServer
    
    @superclass
    nparticleserver

    @classinfo
    Server class of the particle subsystem.
*/
void
n_initcmds_nParticleServer2(nClass* cl)
{
    cl->BeginCmds();
    cl->EndCmds();
}
