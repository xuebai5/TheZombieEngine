#include "precompiled/pchgpbasicactions.h"
//------------------------------------------------------------------------------
//  ngpbasicactions.cc
//------------------------------------------------------------------------------

#include "ngpbasicaction/ngpbasicaction.h"
#include "entity/nentityobject.h"

nNebulaScriptClass(nGPBasicAction, "nobject");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPBasicAction)
    NSCRIPT_ADDCMD('IBII', bool, IsInit, 0, (), 0, ());
    NSCRIPT_ADDCMD('IBID', bool, IsDone, 0, (), 0, ());
    NSCRIPT_ADDCMD('IBAR', bool, Run, 0, (), 0, ());
    NSCRIPT_ADDCMD('IBAE', void, End, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESOE', void, SetOwnerEntity, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('EGOE', nEntityObject*, GetOwnerEntity, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Constructor
*/
nGPBasicAction::nGPBasicAction() : 
    init(false),
    entity (0)
{    
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPBasicAction::~nGPBasicAction()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Run

    @brief  Executes the main loop of the action
    @return true if the action is done, false else
*/
bool
nGPBasicAction::Run()
{
    bool done = this->IsDone();

    if ( !done )
    {
        // main loop
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    End
*/
void
nGPBasicAction::End()
{
    this->entity = 0;
    this->init = false;
}

