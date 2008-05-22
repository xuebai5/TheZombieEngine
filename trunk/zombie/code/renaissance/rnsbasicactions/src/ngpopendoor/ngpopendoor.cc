//-----------------------------------------------------------------------------
//  ngpopendoor.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpopendoor/ngpopendoor.h"

#include "ncgameplaydoor/ncgameplaydoor.h"

#include "napplication/napplication.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nGPOpenDoor, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPOpenDoor)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPOpenDoor::nGPOpenDoor() :
    door(0),
    done(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPOpenDoor::~nGPOpenDoor()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init

    @param entity entity to open

    @return true/false if the init has gone well

*/
bool nGPOpenDoor::Init( nEntityObject * entity )
{
    this->entity = entity;

    n_assert2( entity, "Ill formated basic action." );

    this->door = entity->GetComponent<ncGamePlayDoor>();

    n_assert2( door, "Basic Action applied to the wrong entity." );

    // sets door open state (if possible)
    this->done = door->Open();

    return this->IsDone();
}

//-----------------------------------------------------------------------------
/**
    Returns if the action has been finished

    @param true/false

    history:
        - 07-Dec-2005   Zombie         created
*/
bool nGPOpenDoor::IsDone() const
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
bool nGPOpenDoor::Run()
{
    this->done = this->door->IsOpened();

    return this->IsDone();
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
