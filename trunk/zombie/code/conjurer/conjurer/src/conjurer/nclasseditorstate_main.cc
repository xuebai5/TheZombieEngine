#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nobjecteditorstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nclasseditorstate.h"
#include "conjurer/nimpostorbuilder.h"

nNebulaScriptClass(nClassEditorState, "neditorstate");

//------------------------------------------------------------------------------
/**
*/
nClassEditorState::nClassEditorState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nClassEditorState::~nClassEditorState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nClassEditorState::BuildImpostorLevel(const char *className)
{
    nImpostorBuilder builder;
    builder.SetClass(className);
    builder.BuildImpostorAsset();
}

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nClassEditorState)

    NSCRIPT_ADDCMD('BCIL', void, BuildImpostorLevel, 1, (const char *), 0, ());


    //cl->BeginSignals( 5 );
    //N_INITCMDS_ADDSIGNAL( ClassModified );
    //cl->EndSignals();

NSCRIPT_INITCMDS_END()
