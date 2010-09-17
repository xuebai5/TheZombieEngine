#include "precompiled/pchsummoner.h"
//------------------------------------------------------------------------------
//  nshooterstate_input.cc
//  (C) 2010 M.A. Garcias <ma@magarcias.com>
//------------------------------------------------------------------------------
#include "shootem/nshootemstate.h"

#include "mathlib/line.h"

#include "input/ninputserver.h"
#include "nwaypointserver/nwaypointserver.h"
#include "gfx2/ngfxserver2.h"
#include "animcomp/nccharacter.h"

//------------------------------------------------------------------------------

void
nShootemState::InitPlayerPath()
{
    nWayPointServer* waypointServer = nWayPointServer::Instance();

//    int pathId = waypointServer->GetPathId("Player");
//    waypointServer->pa
    int numWaypoints = waypointServer->GetNumWayPoints( "Player" );
    if (numWaypoints > 1)
    {
        float minLength = 0.f;
        this->currentWaypoint = -1;

        for (int index=0; index<numWaypoints; index++)
        {
            WayPoint* waypoint = waypointServer->GetWayPoint( "Player", index );
            this->playerPath.Append( waypoint->GetPosition() );

            vector3 distance(this->playerPos - waypoint->GetPosition());
            distance.y = 0;
            float length = distance.len();
            if (this->currentWaypoint == -1 || length < minLength)
            {
                this->currentWaypoint = index;
                minLength = length;
            }
        }
    }
}

//------------------------------------------------------------------------------

void 
nShootemState::SnapToPath(vector3& pos)
{
    //find the closest point in the path thast is _behind_ the position
    //and get the rotation corresponding to that segment
    int numWaypoints = this->playerPath.Size();
    float minDistance = 0.f;
    int currentWaypoint = -1;
    for (int index=0; index<numWaypoints-1; index++)
    {
        //check the point is in front of the starting point
        vector3 vecPath(this->playerPath[index+1] - this->playerPath[index]);
        vector3 vecPos(pos - this->playerPath[index]);
        if (vecPath.dot(vecPos) < 0.f)
            continue;

        //check distance to the 
        line3 segment(this->playerPath[index], this->playerPath[index+1]);
        float distance = segment.distance(pos);
        if (currentWaypoint == -1 || distance < minDistance)
        {
            currentWaypoint = index;
            minDistance = distance;
        }
    }

    if (currentWaypoint != -1)
    {
        line3 segment(this->playerPath[currentWaypoint], this->playerPath[currentWaypoint+1]);

        //adjust position to max distance
        if (minDistance > this->fMaxDistanceToPath)
        {
            float t = segment.closestpoint(pos);
            vector3 closest( segment.ipol(t) );
            vector3 vecPos(pos - closest);
            vecPos.norm();
            pos = closest + vecPos * this->fMaxDistanceToPath;
        }
    }
}

//------------------------------------------------------------------------------

bool
nShootemState::HandleInput(nTime frameTime)
{
    nInputServer* inputServer = nInputServer::Instance();

    //player rotate
    float angleSpace = this->turnSpeed * float(frameTime);
    float mouse_x = (inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right"));
    this->playerRot.y += mouse_x * angleSpace;

    //player move
    float moveSpace = this->playerSpeed * float(frameTime);

    PlayerState playerState = PS_Idle;
    vector3 vecMove;
    if (inputServer->GetButton("StrafeLeft"))
    {
        vecMove.x += moveSpace;
        playerState = PS_StrafeLeft;
    }
    if (inputServer->GetButton("StrafeRight"))
    {
        vecMove.x -= moveSpace;
        playerState = PS_StrafeRight;
    }
    if (inputServer->GetButton("Forward"))
    {
        vecMove.z += moveSpace;
        playerState = PS_Forward;
    }
    if (inputServer->GetButton("Backward"))
    {
        vecMove.z -= moveSpace;
        playerState = PS_Backward;
    }

    //update player state
    this->SetPlayerState( playerState );

    matrix44 matWorld;
    matWorld.rotate_y(this->playerRot.y);
    matWorld.translate(this->playerPos);

    //update player position
    this->playerPos = matWorld * vecMove;

    //adjust to path
    vector3 cameraRot;
    this->SnapToPath( this->playerPos, cameraRot );
    float diffAngle = playerRot.y - this->playerRot.y;
    float angle = n_clamp(diffAngle, -angleSpace, angleSpace);
    this->playerRot.y += angle;

    //adjust to terrain
    this->AdjustHeight( this->playerPos );

    //update camera position applying threshold
    vector3 eyePos = matWorld * this->cameraOffset;
    //if ((eyePos.z - this->cameraPos.z) > this->cameraThreshold)
    //{
    //    this->cameraPos.z = eyePos.z - this->cameraThreshold;
    //}
    //else if ((this->cameraPos.z - eyePos.z) > this->cameraThreshold)
    //{
    //    this->cameraPos.z = eyePos.z + this->cameraThreshold;
    //}
    //this->cameraPos.x = eyePos.x;
    this->cameraPos = eyePos;
    float cameraYaw = this->playerRot.y + n_deg2rad(180);//can't figure this out!
    float cameraTurn = cameraYaw - this->cameraAngles.rho;
    cameraTurn = n_clamp(cameraTurn, -angleSpace, angleSpace);
    this->cameraAngles.rho += cameraTurn;

    //shoot
    if (inputServer->GetButton("PrimaryAttack"))
    {
        this->AddProjectile();
    }

    return false;
}

//------------------------------------------------------------------------------

void nShootemState::SetPlayerState(PlayerState state)
{
    n_assert(this->refPlayerEntity.isvalid());
    ncCharacter* character = this->refPlayerEntity->GetComponentSafe<ncCharacter>();

    if (state != this->playerState)
    {
        this->playerState = state;
        switch (state)
        {
        case PS_Forward:
        case PS_Backward:
            character->SetActiveState("wis0_2a_walk", state==PS_Backward, true, true );
            character->SetActiveState("wss0_2a_walk", state==PS_Backward, true, true );
            break;
        case PS_StrafeLeft:
        case PS_StrafeRight:
            character->SetActiveState("ais0_2a_walk", state==PS_Backward, true, true );
            character->SetActiveState("ass0_2a_walk", state==PS_Backward, true, true );
            break;
        case PS_Die:
            character->SetActiveState("$$$2_2a_die", false, false, true );
            break;
        case PS_Idle:
        default://TEMP
            character->SetActiveState("$is0_2a_idle00", false, true, true );
            character->SetActiveState("$ss0_2a_idle00", false, true, true );
            break;
        }
    }
}

//------------------------------------------------------------------------------

void nShootemState::DrawPlayerPath()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->BeginLines();
    gfxServer->DrawLines3d( this->playerPath.Begin(), this->playerPath.Size(), vector4(1,1,0,1));
    gfxServer->EndLines();
}
