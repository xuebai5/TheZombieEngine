//------------------------------------------------------------------------------
//  ncscenedotlaserclass_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsscene.h"
#include "ndotlaser/ncscenedotlaserclass.h"

//------------------------------------------------------------------------------
/**
*/
ncSceneDotLaserClass::ncSceneDotLaserClass() :
    laserProjection( 0.1f , 0.03f, 7.12f, 0.16f), //Default values
    epsilon(0.05f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneDotLaserClass::~ncSceneDotLaserClass()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
void 
ncSceneDotLaserClass::SetLaserProjection(float dist1, float radius1, float dist2, float radius2)
{
#ifndef NGAME
    if (dist1 < 0.f)
    {
        dist1 = 0.f;
    }
    if ( dist2 < dist1 + N_TINY)
    {
        dist2 = dist1 + 0.001f;
    }

    if ( radius2 < radius1 )
    {
        radius2 = radius1;
    }
#endif //NGAME

    this->laserProjection = vector4( dist1, radius1, dist2, radius2 );
}

//------------------------------------------------------------------------------
/**
*/
void 
ncSceneDotLaserClass::GetLaserProjection(float& dist1, float& radius1, float& dist2, float& radius2)
{
    dist1   = this->laserProjection.x;
    radius1 = this->laserProjection.y;
    dist2   = this->laserProjection.z;
    radius2 = this->laserProjection.w;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncSceneDotLaserClass::SetLaserProjectionScript(const vector4& v)
{
    this->laserProjection = v;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncSceneDotLaserClass::SetLaserEpsilon(float epsilon)
{
    this->epsilon = n_max( 0.0001f,epsilon);
}

//------------------------------------------------------------------------------
/**
*/
float
ncSceneDotLaserClass::GetLaserEpsilon()
{
    return this->epsilon;
}


//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
