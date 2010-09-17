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
nShootemState::SnapToPath(vector3& pos, vector3& rot)
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

        //adjust rotation to the segment
        polar2 angles( segment.vec() );
        rot.y = angles.rho;
    }
}

//------------------------------------------------------------------------------

bool
nShootemState::HandleInput(nTime frameTime)
{
    nInputServer* inputServer = nInputServer::Instance();

    //player rotate
    float angleSpace = this->turnSpeed * float(frameTime);

    //if (inputServer->GetButton("StrafeLeft"))
    //{
    //    this->playerRot.y += angleSpace;
    //}
    //if (inputServer->GetButton("StrafeRight"))
    //{
    //    this->playerRot.y -= angleSpace;
    //}

    //player move
    float moveSpace = this->playerSpeed * float(frameTime);

    vector3 vecMove;
    if (inputServer->GetButton("Forward"))
    {
        vecMove.z += moveSpace;
    }
    if (inputServer->GetButton("Backward"))
    {
        vecMove.z -= moveSpace;
    }
    if (inputServer->GetButton("StrafeLeft"))
    {
        vecMove.x += moveSpace;
    }
    if (inputServer->GetButton("StrafeRight"))
    {
        vecMove.x -= moveSpace;
    }

    matrix44 matWorld;
    matWorld.rotate_y(this->playerRot.y);
    matWorld.translate(this->playerPos);

    //update player position
    this->playerPos = matWorld * vecMove;

    //adjust to path
    vector3 playerRot;
    this->SnapToPath( this->playerPos, playerRot );
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
    this->cameraAngles.rho = this->playerRot.y + n_deg2rad(180);//can't figure this out!

    //shoot
    if (inputServer->GetButton("PrimaryAttack"))
    {
        this->AddProjectile();
    }

    return false;
}

//------------------------------------------------------------------------------

void nShootemState::DrawPlayerPath()
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->BeginLines();
    gfxServer->DrawLines3d( this->playerPath.Begin(), this->playerPath.Size(), vector4(1,1,0,1));
    gfxServer->EndLines();
}
