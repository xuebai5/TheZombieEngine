#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  ncspecialfx.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/ncspecialfx.h"
#ifndef NGAME
#include "zombieentity/ncdictionary.h"
#include "kernel/ntimeserver.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpecialFX,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpecialFX)
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('MSTO', void, SetTimeOffset, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GSTO', bool, GetTimeOffset, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSpecialFX::ncSpecialFX()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSpecialFX::~ncSpecialFX()
{
    // empty
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncSpecialFX::SetTimeOffset(bool /*value*/)
{
    ncDictionary* varContext = this->GetComponent<ncDictionary>();
    if (varContext)
    {
        varContext->SetLocalFloatVariable("timeOffset", (float) nTimeServer::Instance()->GetFrameTime());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpecialFX::GetTimeOffset()
{
    return true;
}
#endif
