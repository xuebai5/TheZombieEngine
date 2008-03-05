#include "precompiled/pchrnsgameplay.h"

/**
    @file ncgameplaywheel_main.cc

    (C) Conjurer Services, S.A. 2006
*/

#include "ncgameplaywheel/ncgameplaywheel.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/nctransform.h"
#include "nscene/ncsceneclass.h"
#include "nphysics/ncphyvehicle.h"
#include "ncsound/ncsound.h"
#include "nphysics/ncphyvehicleclass.h"
#include "nphysics/ncphywheel.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGameplayWheel,ncGameplay);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayWheel)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplayWheel::ncGameplayWheel() :
    wasSliding(false)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplayWheel::~ncGameplayWheel()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
	Runs the gameplay logic.

    @param deltaTime time in seconds, between this frame and the last

    history:
     - 08-Feb-2006   David Reyes    created
   
*/
void ncGameplayWheel::Run( const float /*deltaTime*/ )
{
    return;    
    // updates wheel sound
    //this->UpdateSound();
}

//------------------------------------------------------------------------------
/**
	Updates wheel sound.

    history:
     - 08-Feb-2006   David Reyes    created
   
*/
void ncGameplayWheel::UpdateSound()
{
    /*
    if( this->GetComponent<ncPhyWheel>()->IsSliding() )
    {
        if( !this->wasSliding )
        {
            this->GetComponent<ncSound>()->Play( "TiresSkid01", 0 );
        }
    }
    else
    {
        if( this->wasSliding )
        {
            this->GetComponent<ncSound>()->StopSound();
            this->wasSliding = false;
        }
        // materials
    }
    */
}


//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
