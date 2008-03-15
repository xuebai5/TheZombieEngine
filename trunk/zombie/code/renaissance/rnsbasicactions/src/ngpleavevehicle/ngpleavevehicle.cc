#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpleavevehicle.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpleavevehicle/ngpleavevehicle.h"
#include "entity/nentityobject.h"
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "nphysics/nphysicsserver.h"
#include "ncgameplayvehicleseat/ncgameplayvehicleseat.h"
#include "nphysics/ncphycharacter.h"
#include "ncgameplayvehicle/ncgameplayvehicle.h"
#include "ncgameplayvehicleseat/ncgameplayvehicleseatclass.h"
#include "nclogicanimator/nclogicanimator.h"

nNebulaScriptClass(nGPLeaveVehicle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPLeaveVehicle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*, nEntityObject*,const int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPLeaveVehicle::nGPLeaveVehicle() :
    isdone(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPLeaveVehicle::~nGPLeaveVehicle()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPLeaveVehicle::Init (nEntityObject* entity, nEntityObject* vehicle, const int seatumber)
{
    // storing vehicle
    this->vehicle = vehicle;

    // storing driver
    this->entity = entity;

    // storing vehicle seat
    this->seat = this->vehicle->GetComponent<ncGameplayVehicle>()->GetSeat( seatumber );

    n_assert2( this->seat, "Failed to get data." );

    this->init = true;

    /// restoring normal animation
    this->entity->GetComponentSafe<ncLogicAnimator>()->SetIdle();

    return true;
}

//------------------------------------------------------------------------------
/**
IsDone
*/
bool
nGPLeaveVehicle::IsDone() const
{
    // @TODO:
    return this->isdone;
}

//-----------------------------------------------------------------------------
/**
    Main Loop

    @return if the run has end

    history:
        - 25-Jan-2006   David Reyes    created
*/
bool nGPLeaveVehicle::Run()
{
    /// leaving process

    vector3 exitPosition;

    /// finding a valid exit position
    if( !this->ExitPosition( exitPosition ) )
    {
        this->isdone = true;
        return true;
    }

    /// player restoring state
    ncGameplayPlayer* gpPlayer( this->entity->GetComponent<ncGameplayPlayer>() );

    // setting new position
    gpPlayer->SetShotPosition( exitPosition );

    // sets the default camera
    gpPlayer->SetFirstCameraMode( true );

    // setting normal player's state
    gpPlayer->SetState( ncGameplayPlayer::IS_NOTHING );

    // activating normal player's physics
    gpPlayer->ActivatePhysics();

    /// releasing vehicle's seat
    ncGameplayVehicleSeat* gpSeat( this->seat->GetComponent<ncGameplayVehicleSeat>() );

    /// checks if it's the drivers seat to switch off the vehicle
    if( gpSeat->GetSeatType() == ncGameplayVehicleSeatClass::driver )
    {
        gpSeat->GetVehicle()->GetComponent<ncGameplayVehicle>()->SwitchOffVehicle();
    }

    // setting no passenger
    gpSeat->SetPassenger(0);

    // informing the seat it's free
    gpSeat->SetSeatFree(true);

    /// setting the job is done
    this->isdone = true;

    return true; // done
}

//-----------------------------------------------------------------------------
/**
    Find a position to exit

    @param exit position

    @return if it's been able to find an exit point

    history:
        - 25-Jan-2006   David Reyes    created
*/
const bool nGPLeaveVehicle::ExitPosition( vector3& exitposition )
{
    // TODO: In the future the information of exit point will be retrieved from
    //  the dummies in the car brush. (also check the cylinder fits).
    // NOTE: this code it's temporary 'cos a demo need

    exitposition = 
        this->seat->GetComponent<ncGameplayVehicleSeat>()->GetExitPosition();

    matrix33 orientation;

    orientation.from_euler( this->vehicle->GetComponent<ncTransform>()->GetEuler() );

    exitposition = orientation * exitposition;

    exitposition += this->vehicle->GetComponent<ncTransform>()->GetPosition();

    exitposition.y += 5.f;

    nPhyCollide::nContact contactInfo;
    
    if( !nPhysicsServer::Collide( exitposition, vector3(0,-1,0), phyreal(10),1,&contactInfo ) )
    {
        // failed
        return false;
    }

    contactInfo.GetContactPosition( exitposition );

    exitposition.y +=2.f;    

    return true;
}
