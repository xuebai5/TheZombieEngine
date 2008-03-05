//------------------------------------------------------------------------------
//  ngameplayutils.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ngameplayutils.h"

#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"

//------------------------------------------------------------------------------
namespace nGameplayUtils
{
nPhyGeomRay * shotRay = 0;
nPhyGeomRay * sightRay = 0;

#ifndef NGAME
unsigned long numShots = 0;
unsigned long numCollisions = 0;
unsigned long maxCollisions = 0;
unsigned int lastCollisions = 0;
unsigned long totalCollisions = 0;
float maxDistCollision = 0.0f;
float lastDistCollision = 0.0f;

//------------------------------------------------------------------------------
unsigned long 
GetNumShots()
{
    return numShots;
}

//------------------------------------------------------------------------------
unsigned long 
GetNumCollisions()
{
    return numCollisions;
}

//------------------------------------------------------------------------------
unsigned long 
GetMaxCollisions()
{
    return maxCollisions;
}

//------------------------------------------------------------------------------
unsigned long 
GetLastCollisions()
{
    return lastCollisions;
}

//------------------------------------------------------------------------------
unsigned long 
GetTotalCollisions()
{
    return totalCollisions;
}

//------------------------------------------------------------------------------
float 
GetMaxDistCollision()
{
    return maxDistCollision;
}

//------------------------------------------------------------------------------
float 
GetLastDistCollision()
{
    return lastDistCollision;
}
#endif//NGAME

//------------------------------------------------------------------------------
/**
*/
void
Init()
{
    if( ! shotRay )
    {
        shotRay = static_cast<nPhyGeomRay*>( nKernelServer::ks->New( "nphygeomray" ) );
        n_assert( shotRay );
        if( shotRay )
        {
            shotRay->SetCategories(0);
            shotRay->SetCollidesWith( ~(nPhysicsGeom::Check|nPhysicsGeom::Ramp) );
        }
    }

#ifndef NGAME
    numShots = 0;
    numCollisions = 0;
    maxCollisions = 0;
    lastCollisions = 0;
    totalCollisions = 0;
    maxDistCollision = 0.0f;
    lastDistCollision = 0.0f;
#endif//NGAME
}

//------------------------------------------------------------------------------
/**
*/
void
Destroy()
{
    if( shotRay )
    {
        shotRay->Release();
        shotRay = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
ShotRay( const vector3 & pos, const vector3 & dir, const float dist,
                       vector3 & shotPos, geomid * geom, ncPhysicsObj ** obj, vector3 & shotNormal, nGameMaterial ** gmaterial )
{
    n_assert2( shotRay, "Need to do nGameplayUtils::Init()" );
    if( !shotRay )
    {
        return false;
    }

#ifndef NGAME
    ++numShots;
#endif//NGAME

    const int contactSize = 20;
    bool founded = false;

    float actualDist = 0;
    float distIncr = 5.0f;
    geomid shotGeom = NoValidID;
    ncPhysicsObj * shotObj = 0;

    vector3 shotDir( dir );
    shotDir.norm();

    while( ! founded && actualDist < dist )
    {
        shotRay->SetLength( distIncr );
        shotRay->SetDirection( shotDir );
        shotRay->SetPosition( pos + shotDir*actualDist );

        nPhyCollide::nContact contact[ contactSize ];
        unsigned int num( nPhysicsServer::Instance()->Collide( shotRay, contactSize, contact ) );

        if( num )
        {
            founded = true;
#ifndef NGAME
            ++numCollisions;
            lastCollisions = num;
            totalCollisions += num;
            if( int(num) > maxCollisions )
            {
                maxCollisions = num;
            }
#endif//NGAME


            float minDist = FLT_MAX;
            float dist = 0;
            unsigned int ind = 0;
            for( unsigned int i = 0 ; i < num ; ++i )
            {
                contact[i].GetContactPosition( shotPos );
                contact[i].GetContactNormal( shotNormal );
                dist = ( shotPos - pos ).lensquared();
                if( dist < minDist )
                {
                    minDist = dist;
                    ind = i;
                }
            }

            if( contact[ind].GetGeometryIdA() == shotRay->Id() )
            {
                shotGeom = contact[ind].GetGeometryIdB();
                shotObj = contact[ind].GetPhysicsObjB();
                *gmaterial = contact[ind].GetGameMaterialB();
            }
            else
            {
                shotGeom = contact[ind].GetGeometryIdA();
                shotObj = contact[ind].GetPhysicsObjA();
                *gmaterial = contact[ind].GetGameMaterialA();
            }

            contact[ind].GetContactPosition( shotPos );
            contact[ind].GetContactNormal( shotNormal );
        }

        actualDist += distIncr;
    }

#ifndef NGAME
    lastDistCollision = actualDist;
    if( actualDist > maxDistCollision )
    {
        maxDistCollision = actualDist;
    }
#endif//NGAME

    if( geom )
    {
        *geom = shotGeom;
    }
    if( obj )
    {
        *obj = shotObj;
    }

    return founded;
}

}// namespace nGameplayUtils

//------------------------------------------------------------------------------
