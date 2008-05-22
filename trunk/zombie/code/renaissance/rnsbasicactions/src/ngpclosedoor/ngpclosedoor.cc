//-----------------------------------------------------------------------------
//  ngpclosedoor.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpclosedoor/ngpclosedoor.h"

#include "ncgameplaydoor/ncgameplaydoor.h"

#include "napplication/napplication.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nGPCloseDoor, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPCloseDoor)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPCloseDoor::nGPCloseDoor() :
    door(0),
    done(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPCloseDoor::~nGPCloseDoor()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
    @param entity entity to close
    @return true/false if the init has gone well

*/
bool nGPCloseDoor::Init( nEntityObject * entity )
{
    this->entity = entity;

    n_assert2( entity, "Ill formated basic action." );

    this->door = entity->GetComponent<ncGamePlayDoor>();

    n_assert2( door, "Basic Action applied to the wrong entity." );

    // sets door close state (if possible)
    this->done = door->Close();

    return this->IsDone();
}

//-----------------------------------------------------------------------------
/**
    Returns if the action has been finished

    @param true/false

    history:
        - 07-Dec-2005   Zombie         created
*/
bool nGPCloseDoor::IsDone() const
{
    return this->done;       
}

//-----------------------------------------------------------------------------
/**
    Main loop

    @param true/false

    history:
        - 07-Dec-2005   Zombie         created
*/
bool nGPCloseDoor::Run()
{
    this->done = this->door->IsClosed();

    return this->IsDone();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
