//------------------------------------------------------------------------------
//  ndebugmodule_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugmodule.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ndebugmodule

    @cppclass
    nDebugModule

    @superclass
    nroot

    @classinfo
    abstract class for creating application-specific modules
*/
NSCRIPT_INITCMDS_BEGIN(nDebugModule)

    cl->BeginSignals(1);
    N_INITCMDS_ADDSIGNAL(DebugOptionChanged);
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
