//------------------------------------------------------------------------------
//  ngpdeletespecialitem.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"
#include "ngpinventory/ngpdeletespecialitem.h"
#include "ncgameplayplayer/ncgameplayplayer.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPDeleteSpecialItem, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPDeleteSpecialItem)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*,int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
nGPDeleteSpecialItem::nGPDeleteSpecialItem()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPDeleteSpecialItem::~nGPDeleteSpecialItem()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Initial condition

    The item is already removed here

    @param entity Entity from whom the special item is going to be removed
    @param slot Special item slot
    @return True if the action succeeds, false otherwise (not carrying the item doesn't cause action failure)
*/
bool
nGPDeleteSpecialItem::Init( nEntityObject* entity, int slot )
{
    this->entity = entity;

    // Get player gameplay component
    ncGameplayPlayer* gameplay( this->entity->GetComponent<ncGameplayPlayer>() );
    n_assert2( gameplay, "DeleteSpecialItem action can only be applied on players" );
    if ( !gameplay )
    {
        return false;
    }

    // Remove the special item from the player's inventory
    gameplay->SetSpecialEquipment( slot, false );

    return true;
}

//------------------------------------------------------------------------------
/**
    Main execution loop

    Nothing to do for this action, just return finish code

    @return Always true
*/
bool
nGPDeleteSpecialItem::Run()
{
    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
    Stop condition

    @return Always true
*/
bool
nGPDeleteSpecialItem::IsDone()const
{
    return true;
}
