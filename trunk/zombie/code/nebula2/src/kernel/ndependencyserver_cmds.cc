//------------------------------------------------------------------------------
//  ndependencyserver_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/ndependencyserver.h"

void n_initcmds_nDependencyServer(nClass *cl)
{
    cl->BeginCmds();
    cl->EndCmds();
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
