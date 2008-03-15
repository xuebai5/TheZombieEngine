#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpblock.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpblock/ngpblock.h"
#include "ncaistate/ncaistate.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "zombieentity/nctransform.h"
#include "nclogicanimator/nclogicanimator.h"
#include "nphysics/ncphycharacterobj.h"
#include "rnsgameplay/ndamagemanager.h"
#include "rnsgameplay/ncgpweaponmelee.h"
#include "animcomp/nccharacter.h"

nNebulaScriptClass(nGPBlock, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPBlock)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*, int, int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPBlock::nGPBlock() :
    nGPBasicAction(),
    animator(0),
    weapon(0),
    ellapsedTime(0),
    startOffset(0),
    endOffset(0),
    living(0)

{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPBlock::~nGPBlock() 
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init

    @param initOffset
    Is a percentage of the total animation time that 
    indicate the time the player is not parrying at the begining of animation.
    @param finalOffset
    Is a percentage of the total animation time that 
    indicate the time the player is not parrying at the end of animation.
*/
bool
nGPBlock::Init (nEntityObject* entity, int initOffset, int finalOffset)
{
    bool valid = entity != 0;

    if ( valid )
    {
        this->entity = entity;

        // get the animator
        this->animator = entity->GetComponentSafe<ncLogicAnimator>();

        // get the primary weapon        
        nEntityObject* weaponEntity = 0;
        this->living = entity->GetComponentSafe<ncGameplayLiving>();        
        if ( this->living )
        {
            // get melee weapon            
            weaponEntity = this->living->GetCurrentWeapon();
            if (weaponEntity)
            {
                this->weapon = weaponEntity->GetComponentSafe<ncGPWeaponMelee>();
                n_assert( this->weapon );
            }
        }
        valid = this->weapon != 0;
    }

    if ( valid )
    {        
        //ncCharacter* character = this->entity->GetComponentSafe<ncCharacter>();
        // TODO: Change this when there are a relationship betwen ticks ans seconds
        //nTime blockTime = character->GetStateDuration(GP_ACTION_MELEEBLOCK);
        nTime blockTime;
        blockTime = 0;
        // Times that the entity is not blocking, are percentages of state duration
        this->startOffset = initOffset;
        this->endOffset = finalOffset;
        
        this->living->SetBlocking( false );
        this->animIndex = this->animator->SetMeleeBlock();
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPBlock::IsDone() const
{
    n_assert(this->init);

    return !this->animator || this->animator->HasFinished( this->animIndex );
}

//------------------------------------------------------------------------------
/**
    Run
*/
bool
nGPBlock::Run()
{
    n_assert(this->init);    
    this->ellapsedTime++;
   
    // Only block between the offsets
    if ( this->ellapsedTime > startOffset && this->ellapsedTime < this->endOffset && !this->living->IsBlocking())
    {
        this->living->SetBlocking( true ); 
    }
    else if ( this->living->IsBlocking() )
    {
        this->living->SetBlocking( false ); 
    }

    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
End
*/
void
nGPBlock::End()
{
    n_assert( this->living );
    if ( this->living )
    {
        this->living->SetBlocking( false );
    }
}
