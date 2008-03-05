//------------------------------------------------------------------------------
//  ncgameplayplayerclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "ncgameplayplayer/ncgameplayplayerclass.h"

nNebulaComponentClass(ncGameplayPlayerClass,ncGameplayLivingClass);

//------------------------------------------------------------------------------
/**
*/
ncGameplayPlayerClass::ncGameplayPlayerClass():
    isLocalPlayer(true),	
    jumpVertical(50000),jumpHorizontal(5000),jumpTime(0.7f),
    maxStamina(5000),jumpStamina(1000),sprintStamina(4),
    tiredTime(1.0f),
    staminaRecover(50),
    playerHeight( 2.0f ), playerWide( 0.5f ),
    standHeight( 2.0f ), crouchHeight( 1.5f ), proneHeight( 1.0f ),
    leanDistance( 0.0f )
{
	this->baseSpeed = 7.f;
	this->multSpeed = 100;
	this->multRunSpeed = 300;
    this->multIronsightSpeed = 66;
	this->multRunIronsightSpeed = 120;
	this->multCrouchSpeed = 66;
	this->multProneSpeed = 33;
	this->multCrouchIronsightSpeed = 100;
	this->multProneIronSightSpeed = 0;
	this->multSwimSpeed = 33;
}

//------------------------------------------------------------------------------
/**
*/
ncGameplayPlayerClass::~ncGameplayPlayerClass()
{
    // empty
}

//------------------------------------------------------------------------------
