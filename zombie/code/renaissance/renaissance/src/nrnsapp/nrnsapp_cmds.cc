#include "precompiled/pchrenaissanceapp.h"
//------------------------------------------------------------------------------
//  nRnsApp_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nrnsapp/nrnsapp.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nrnsapp

    @cppclass
    nRnsApp

    @superclass
    ncommonapp

    @classinfo
    The main renaissance game application.
*/
NSCRIPT_INITCMDS_BEGIN(nRnsApp)
    NSCRIPT_ADDCMD('MFPS', void, ShowFPS, 1, (bool), 0, ());
NSCRIPT_INITCMDS_END()
