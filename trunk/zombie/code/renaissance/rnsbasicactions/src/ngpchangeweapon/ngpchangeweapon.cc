#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpchangeweapon.cc
//------------------------------------------------------------------------------

#include "ngpchangeweapon/ngpchangeweapon.h"
#include "ncaistate/ncaistate.h"

nNebulaScriptClass(nGPChangeWeapon, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPChangeWeapon)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPChangeWeapon::nGPChangeWeapon() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPChangeWeapon::~nGPChangeWeapon()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPChangeWeapon::Init (nEntityObject* entity, nEntityObject* weapon)
{
    bool init = true;

    this->entity = entity;
    this->weapon = weapon;
    this->init = true;

    // @TODO:
    // Here will be the method to change the weapon, using the propper animation

    return init;
}

//------------------------------------------------------------------------------
/**
    Run
*/
bool
nGPChangeWeapon::Run()
{
    bool done = this->IsDone();

    if ( done && this->entity )
    {
        ncAIState* state = this->entity->GetComponent <ncAIState>();
        n_assert(state);

        if ( state )
        {
            state->SetActiveWeapon (this->weapon);
        }
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPChangeWeapon::IsDone() const
{
    // @TODO: Here we must check if the animation is over, and then, say true.
    return nGPBasicAction::IsDone();
}