#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpentervehicle.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpentervehicle/ngpentervehicle.h"
#include "entity/nentityobject.h"
#include "ncgameplayvehicle/ncgameplayvehicle.h"
#include "nphysics/ncphycharacter.h"
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "ncgameplayvehicleseat/ncgameplayvehicleseatclass.h"
#include "nphysics/ncphyvehicle.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncgameplayvehicleseat/ncgameplayvehicleseatclass.h"


nNebulaScriptClass(nGPEnterVehicle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPEnterVehicle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*,nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPEnterVehicle::nGPEnterVehicle() :
    isdone(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPEnterVehicle::~nGPEnterVehicle()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPEnterVehicle::Init (nEntityObject* eDriver,nEntityObject* eVehicle)
{
    // @TODO:
    this->vehicle = eVehicle;

    this->entity = eDriver;

    this->init = true;

    if( !eVehicle->GetComponent<ncGameplayVehicle>()->IsASeatAvailable() )
    {
        return false;
    }

    if( !eVehicle->GetComponent<ncGameplayVehicle>()->ReserveSeat( this->GetOwnerEntity() ) )
    {
        return false;
    }

    this->entity->GetComponent<ncGameplayPlayer>()->SetFirstCameraMode( false );

    nEntityObject* seat(this->entity->GetComponent<ncGameplayPlayer>()->GetSeat());

    if( seat->GetComponent<ncGameplayVehicleSeat>()->GetSeatType() == ncGameplayVehicleSeatClass::driver )
    {
        eVehicle->GetComponent<ncGameplayVehicle>()->SwitchOnVehicle();
        /// setting animation
        this->entity->GetComponentSafe<ncLogicAnimator>()->SetDrivingIdle();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPEnterVehicle::IsDone() const
{
    return this->isdone;
}

//-----------------------------------------------------------------------------
/**
    Main Loop

    @return if the run has end

    history:
        - 25-Jan-2006   David Reyes    created
*/
bool nGPEnterVehicle::Run()
{
    /// entering process
    ncGameplayPlayer* gpPlayer( this->entity->GetComponent<ncGameplayPlayer>() );

    // disabling the player physics
    gpPlayer->DeactivePhysics();

    ncGameplayVehicleSeat * seat = gpPlayer->GetSeat()->GetComponentSafe<ncGameplayVehicleSeat>();
    if( seat )
    {
        ncGameplayVehicleSeatClass * seatClass = seat->GetClassComponentSafe<ncGameplayVehicleSeatClass>();

        // sets player state
        switch( seat->GetSeatType() )
        {
            case ncGameplayVehicleSeatClass::driver:
                gpPlayer->SetState( ncGameplayPlayer::IS_DRIVER );
                break;

            case ncGameplayVehicleSeatClass::normal:
                gpPlayer->SetState( ncGameplayPlayer::IS_PASSENGER );
                gpPlayer->SetElevationLimits( vector2( seatClass->GetMaxPitch(), seatClass->GetMinPitch() ) );
                gpPlayer->SetTurnLimits( vector2( seatClass->GetMaxPitch(), seatClass->GetMinYaw() ) );
                break;

            default:
                n_assert_always();
        }
    }

    /// setting the job is done
    this->isdone = true;

    return true; // done
}
