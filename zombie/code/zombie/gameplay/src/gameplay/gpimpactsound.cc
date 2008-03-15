#include "precompiled/pchgameplay.h"
//------------------------------------------------------------------------------
//  gpimpactsound.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gameplay/gpimpactsound.h"
#include "ncsound/ncsound.h"
#include "gameplay/ngamematerial.h"
#include "gameplay/ngamematerialserver.h"

#include "nphysics/ncphywheel.h"
#include "nphysics/ncphyvehicle.h"

#include "nphysics/ncphycharacter.h"

namespace {
    phyreal MinLinearSquaredSpeedToProduceSound( phyreal(1) );
    phyreal MinAngularSquaredSpeedToProduceSound( phyreal(1) );
    phyreal SlidingAngle( phyreal(.1) );
}

//-----------------------------------------------------------------------------
/**
    Process a single impact info. 

    @param obj dynamic object
    @param contact contact information
    @param gmaterial none dynamic object material

    history:
        - 14-Feb-2006   David Reyes    created
*/
inline void gpProcessOnImpact( ncPhysicsObj* objA, const nPhyCollide::nContact* contact, nGameMaterial* gmaterial, geomid contactGeom )
{
    n_assert_return( objA, );
    n_assert_return( contact, );
    n_assert_return( gmaterial, );
    n_assert_return( contactGeom != NoValidID, );

    if( objA->GetComponent<ncPhyCharacter>() )
    {
        // nothing to do with the players
        return;
    }
    
    // checking minimun speed
    vector3 speed;
    vector3 angularSpeed;
    
    objA->GetLinearVelocity( speed );
    objA->GetAngularVelocity( angularSpeed );

    if( angularSpeed.lensquared() < MinAngularSquaredSpeedToProduceSound && 
        speed.lensquared() < MinLinearSquaredSpeedToProduceSound )
    {
        ncSound* sound( objA->GetComponent<ncSound>() );
        if ( sound && sound->IsPlaying() )
        {
            sound->StopSound();
        }
        
        return;
    }
    
    // special case for wheels, get the vehicle and use always the same wheel for launch and stop sounds
    // @TODO JAVI: DEMO code. Find a better way after nSoundScheduler refactoring.
    if ( objA->GetComponent<ncPhyWheel>() )
    {
        ncPhyVehicle* vehicle = objA->GetComponentSafe<ncPhyWheel>()->GetVehicle();
        if ( vehicle )
        {
            ncPhyVehicle::Wheel* wheel = (*vehicle)[0];
            n_assert_return( wheel, );
            ncPhyWheel* soundWheel = wheel->GetWheelObject();
            n_assert_return( soundWheel, );
            if ( soundWheel )
            {
                objA = soundWheel->GetComponentSafe<ncPhysicsObj>();
                n_assert_return( objA, );
            }            
        }                
    }

    speed.norm();
    vector3 normal;        
    contact->GetContactNormal( normal );
    
    ncSound* sound( objA->GetComponent<ncSound>() );
    if( sound )
    {
        if( ! sound->IsPlaying() )
        {
            // checking if sliding
            if( fabs(speed.dot( normal )) < SlidingAngle )
            {
                if ( ! sound->IsPlaying() )
                {
                    sound->PlayMaterial( "slide", gmaterial->GetGameMaterialName(),0 );
                }                    
            }
            else
            {
                sound->PlayMaterial( "hit", gmaterial->GetGameMaterialName(),1 );
            }
        }
    }
    else
    {
        nSoundScheduler* scheduler = nSoundScheduler::Instance();
        n_assert_return( scheduler, );
        vector3 position;
        position = objA->GetEntityObject()->GetComponentSafe<ncTransform>()->GetPosition();
        
        if( ! (fabs(speed.dot( normal )) < SlidingAngle) )
        {
            // hit sound
            const char * soundName;

            // plays hit sound. If material doesn't have hit sound reproduce the other material sound.
            // This is because floors that not have hit sound event makes that sound the object impacted in floor.               
            soundName = gmaterial->GetHitSoundEvent();
            if ( ! soundName )
            {
                nPhysicsGeom * geom = static_cast<nPhysicsGeom*>( phyRetrieveDataGeom( contactGeom ) );
                n_assert_return( geom, );
                uint matId = geom->GetGameMaterialId();
                nGameMaterial* material = nGameMaterialServer::Instance()->GetMaterial( matId );
                NLOGCOND_REL(resource, !material, (NLOGUSER | 0, "Material for geom %s not exists or is not assigned",
                             geom->GetClass()->GetProperName() )) ;
                if ( material )
                {
                    soundName = material->GetHitSoundEvent();
                    NLOGCOND_REL(resource, !soundName, (NLOGUSER | 0, "Hit sound for material %s not exists", material->GetName() )) ;
                }                
            }
            
            if ( soundName )
            {
                scheduler->PlayBySoundName( position, soundName, 1 );                             
            }
        }
        else
        {
            const char * soundName = gmaterial->GetSlideSoundEvent();
            NLOGCOND_REL(resource, !soundName, (NLOGUSER | 0, "Slide sound for material %s not exists", gmaterial->GetName() )) ;
            if ( soundName )
            {
                scheduler->PlayBySoundName( position, soundName, 1 ); 
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Processes the sound information for a given impact. 

    @param objA object involve in the impact
    @param objB object involve in the impact
    @param contact contact information

    history:
        - 14-Feb-2006   David Reyes    created
*/
void gpImpactSoundProcess( ncPhysicsObj* objA, ncPhysicsObj* objB, const nPhyCollide::nContact* contact )
{
    n_assert_return( objA, );
    n_assert_return( objB, );
    n_assert_return( contact, );

    const bool objADynamic(objA->GetMass() ? true : false);

    if( objADynamic )
    {
        nGameMaterial* gmaterial(contact->GetGameMaterialB());
        if ( gmaterial )
        {
            gpProcessOnImpact( objA, contact, gmaterial, contact->GetGeometryIdA() );
        }
    }
    else
    {
        nGameMaterial* gmaterial(contact->GetGameMaterialA());
        if ( gmaterial )
        {
            gpProcessOnImpact( objB, contact, gmaterial, contact->GetGeometryIdB() );
        }
    }
}

