//------------------------------------------------------------------------------
//  nfxeventtrigger.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchgameplay.h"
#include "gameplay/nfxeventtrigger.h"

//------------------------------------------------------------------------------
#include "gameplay/ngamematerialserver.h"
#include "gameplay/ngamematerial.h"

//------------------------------------------------------------------------------
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "rnsgameplay/ninventoryitem.h"
//------------------------------------------------------------------------------
#include "nspecialfx/nfxobject.h"
#include "nspecialfx/nfxserver.h"
#include "nspecialfx/nfxspawn.h"
#include "nspecialfx/nfxdecal.h"
#include "nspecialfx/nfxmaterial.h"
//------------------------------------------------------------------------------
#include "nscene/ncscene.h"
#include "nscene/ntransformnode.h"
#include "nphysics/ncphysicsobjclass.h"

//------------------------------------------------------------------------------
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nFxEventTrigger, "nobject");

nFxEventTrigger* nFxEventTrigger::singleton = 0;

namespace
{
    const char* FileFxNonMaterialsPath("wc:/libs/fx/fx_general.n2");
}

//------------------------------------------------------------------------------
/**
*/
nFxEventTrigger::nFxEventTrigger() :
    fxMatIndependent(0,1)
{
    nFxEventTrigger::singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nFxEventTrigger::~nFxEventTrigger()
{
    this->fxMatIndependent.Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool 
nFxEventTrigger::Launch(const nLaunchSpecialFxData& fxData)
{
    NLOG(specialfx, (9 | nFXServer::LAUNCH, "LaunchImpact event: %s  game material: %s", fxData.eventType.Get() , ( fxData.gameMat ? fxData.gameMat->GetName() : 0 )  ));

    nFxMaterial* fxMat;
    vector3 impactNormal;
    if( fxData.gameMat )
    {
        fxMat = fxData.gameMat->GetFxMaterialObject();
        impactNormal = fxData.impactNormal;
    } else
    {
        // example explosion in the air
        fxMat = nFXServer::Instance()->GetGenericFxMaterial();
        impactNormal = vector3(0.f, 1.f, 0.f);
    }

    if( ! fxMat )
    {
        return false;
    } else
    {
        //important: all special fx must be exported with normal(0,1,0)
        //           is compatible with particle system, it start in (0,1,0) direction
        quaternion objectRotation;
        objectRotation.set_from_axes( vector3(0,1,0), impactNormal); // Move axes 0,1,0 to normal
        nFxMaterial::nFxEffects* impactEffects = fxMat->GetFxEffectFor( fxData.eventType.Get() );
        // Any material not has defined effects for all events
        if( impactEffects )
        {
            for( int i=0; i< impactEffects->effectsIndexList.Size(); i++)
            {
                nFxMaterial::nFxNode* fxnode = fxMat->GetFxNodeFor( impactEffects, i );
                if (!fxnode)
                {
                    n_assert3_always( ( "The material %s has %d effects but the effect %d isn't defined" , fxData.gameMat->GetName() , impactEffects->effectsIndexList.Size() , i) );
                    return false;
                }                   

                //randomize , select one of all posible values
                int randomIndex = n_rand_int( fxnode->fxlist.Size()-1);
                int key = fxnode->keys[randomIndex];
                ncSpecialFXClass::FxType type = fxnode->GetType(randomIndex);

                if( key != 0 ) 
                {
                    switch (type)
                    {
                        case ncSpecialFXClass::FX_DECAL:
                            {
                                if ( nFxEventTrigger::CanReceiveDecal(fxData.entityObject) )
                                {
                                    nFXDecal* effect = static_cast<nFXDecal*>(nFXServer::Instance()->PullEffect(key));
                                    effect->SetTargetEntity(fxData.entityObject);
                                    effect->SetFXPosition(fxData.impactPoint);
                                    effect->SetFXScale(fxData.scale);
                                    effect->SetFXRotation(objectRotation);
                                    effect->SetRandomRotation(true);
                                    NLOG(specialfx, (10 | nFXServer::LAUNCH , "Decal %s impactPoint: %f %f %f normal: %f %f %f rotation: %f %f %f %f", effect->GetFXClass(), fxData.impactPoint.x, fxData.impactPoint.y, fxData.impactPoint.z, fxData.impactNormal.x, fxData.impactNormal.y, fxData.impactNormal.z, objectRotation.x, objectRotation.y, objectRotation.z, objectRotation.w ));
                                }
                                break;
                            }
                        case ncSpecialFXClass::FX_SPAWN:
                            {
                                nFXSpawn* effect = static_cast<nFXSpawn*>(nFXServer::Instance()->PullEffect(key));
                                effect->SetFXPosition(fxData.impactPoint);
                                effect->SetFXScale(fxData.scale);
                                effect->SetFXRotation(objectRotation);
                                NLOG(specialfx, (10 | nFXServer::LAUNCH , "Spawn %s impactPoint: %f %f %f", effect->GetFXClass(), fxData.impactPoint.x, fxData.impactPoint.y, fxData.impactPoint.z ));
                                break;
                            }
                        default:
                            {
                                n_error("LoadSFx: invalid fxtype '%d'!", type);
                                break;
                            }
                    }
                }
            }
        }
        return true;
    }
}

//------------------------------------------------------------------------------
/**
    Return if the object can receive the decal
*/
bool 
nFxEventTrigger::CanReceiveDecal(nEntityObject* object)
{
    // Only receive decals statics objects.
    /// todo Remove this method ans use decals in moviles objects.
    if (!object)
    {
        return false;
    } else
    {
        ncPhysicsObjClass* phyClass = object->GetClassComponent<ncPhysicsObjClass>();
        if (!phyClass)
        {
            return true;
        }
        return !(phyClass->GetMobile());
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
nFxEventTrigger::LaunchExplosion(const vector3& explosionPoint)
{
    NLOG(specialfx, (9 | nFXServer::LAUNCH, "LaunchExplosion"));

    nFxSpecial* fxnode = this->GetFxFor( EXPLOSION );

    if( fxnode )
    {
        //randomize
        int randomIndex = n_rand_int( fxnode->fxlist.Size()-1);
        int key = fxnode->fxlist[randomIndex].key;

        if( key == 0 )
        {
            return false;
        }

        nFXSpawn* effect = static_cast<nFXSpawn*>(nFXServer::Instance()->PullEffect(key));
        n_assert(effect);
        effect->SetFXPosition(explosionPoint);
        NLOG(specialfx, (10| nFXServer::LAUNCH, "%s position: %f %f %f", effect->GetFXClass(), explosionPoint.x, explosionPoint.y, explosionPoint.z ));

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nFxEventTrigger::Splash( nString whereFrom )
{
    NLOG(specialfx, (9| nFXServer::LAUNCH, "Splash"));

    nFxSpecial* fxnode = this->GetFxFor( nFxEventTrigger::StringToType(whereFrom.Get()) );
    int key = fxnode->fxlist[0].key;

    if( fxnode && (key != 0))
    {
        nFXSpawn* effect = static_cast<nFXSpawn*>(nFXServer::Instance()->PullEffect(key));
        NLOG(specialfx, (10| nFXServer::LAUNCH, "%s wherefrom: %s", effect->GetFXClass(), whereFrom.Get() ));
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nFxEventTrigger::Shoot( nEntityObject* shooter, nEntityObject* weapon)
{
    //NLOG(specialfx, (9| nFxEventTrigger::LAUNCH, "Shoot"));

    nFxSpecial* fxnode = 0;

    ncGameplayPlayer* player = shooter->GetComponent<ncGameplayPlayer>();
/** ZOMBIE REMOVE
    ncGPWeapon* gpweapon = weapon->GetComponentSafe<ncGPWeapon>();
*/
    if( player && player->IsFirstCameraMode() )
    {
/** ZOMBIE REMOVE
        //if silencer, do not show muzzle flash
        if( gpweapon && !gpweapon->IsMounted(player->GetInventory()->GetInventoryItem("Rns_wac_slb")) )
*/
        {
            fxnode = this->GetFxFor(M4_1P_FIRE);
        }
    }
    else
    {
        fxnode = this->GetFxFor(M4_3P_FIRE);
    }

    if(fxnode)
    {
        int key = fxnode->fxlist[0].key;
        if( key == 0)
        {
            return false;
        }

        nFXSpawn* effect = static_cast<nFXSpawn*>(nFXServer::Instance()->PullEffect(key));
        n_assert(effect);

        effect->SetParentEntity(weapon);

        //NLOG(specialfx, (10| nFxEventTrigger::LAUNCH, "The shoot effect %s", effect->GetFXClass()));
    }

    return false;
}
//------------------------------------------------------------------------------
/**
*/
nFxEventTrigger::nFxSpecial*
nFxEventTrigger::GetFxFor( nFxSubType type )
{
    for( int i=0; i< this->fxMatIndependent.Size(); i++)
    {
        if(this->fxMatIndependent[i].fxsubtype == type)
        {
            return &this->fxMatIndependent[i];
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nFxEventTrigger * 
nFxEventTrigger::Instance()
{
    n_assert( nFxEventTrigger::singleton );
    return nFxEventTrigger::singleton;
}


//------------------------------------------------------------------------------
/**
*/
bool 
nFxEventTrigger::LoadSFx(nFxMaterial* fxMaterial)
{
    n_assert( fxMaterial );
    if (!fxMaterial) 
    {
        return false;
    }

    bool result = true;

    for(int i=0; i< fxMaterial->GetNumFxNodes(); i++)
    {
        nFxMaterial::nFxNode* fxNode = fxMaterial->GetFxNodeByIndex(i);
        ncSpecialFXClass::FxType type = fxNode->GetType(i);
        nFXObject* fxObject = 0;
        
        for( int j=0; j< fxNode->fxlist.Size(); j++)
        {
            switch (type)
            {
                case ncSpecialFXClass::FX_DECAL:
                    fxObject = nFXServer::Instance()->NewDecalEffect( fxNode->fxlist[j].Get() );
                    break;
                case ncSpecialFXClass::FX_SPAWN:
                    fxObject = nFXServer::Instance()->NewSpawnEffect( fxNode->fxlist[j].Get() );
                    break;
                default:
                    n_error("LoadSFx: invalid fxtype '%d'!", type);
                    fxObject = 0;
            }

            if( fxObject)
            {
                fxNode->keys[j] = fxObject->GetKey();
                NLOG(specialfx, ( 8 | nFXServer::CREATION, "Create effect %s for %s fxmaterial" , fxNode->id.Get() , fxMaterial->GetName() ));
            }
            else
            {
                fxNode->keys[j] = 0;
                NLOG(specialfx, ( 0 | nFXServer::CREATION, "I can't create effect %s for %s fxmaterial" , fxNode->id.Get() , fxMaterial->GetName() ));
                result =  false;
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nFxEventTrigger::LoadSFx()
{
    int totalMat = nGameMaterialServer::Instance()->GetNumMaterials();
    bool result = true;

    for( int numMat=0; numMat < totalMat ; numMat++)
    {
        nGameMaterial* gamematerial = nGameMaterialServer::Instance()->GetMaterialByIndex(numMat);
        n_assert(gamematerial);

        nFxMaterial* fxMaterial = gamematerial->GetFxMaterialObject();

        if( fxMaterial )
        {
            bool result1 = this->LoadSFx(fxMaterial);
            result = result1 && result;
        }
    }

    nFxMaterial* fxMaterial = nFXServer::Instance()->GetGenericFxMaterial();
    if ( fxMaterial )
    {
        bool result1 = this->LoadSFx(fxMaterial);
        result = result1 && result;
    }



    if( this->LoadChunk() )
    {
        for( int i=0; i< this->fxMatIndependent.Size(); i++)
        {
            nFxSpecial* fxNode = &this->fxMatIndependent[i];
            nFXObject* fxObject = 0;
            
            for(int j=0; j< fxNode->fxlist.Size(); j++)
            {
                fxObject = nFXServer::Instance()->NewSpawnEffect( fxNode->fxlist[j].fxclass.Get() );
                if( fxObject )
                {
                    fxNode->fxlist[j].key = fxObject->GetKey();
                }
                else
                {
                    fxNode->fxlist[j].key = 0;
                    result = false;
                }
            }
        }

    }
    
    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nFxEventTrigger::LoadChunk()
{
    // load resource
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);
    if( ps )
    {
        nString path = FileFxNonMaterialsPath;

        if( nFileServer2::Instance()->FileExists( path.Get() ) )
        {
            nKernelServer::Instance()->PushCwd( this );
            nObject* loaded = nKernelServer::Instance()->Load( path.Get(), false );
            nKernelServer::Instance()->PopCwd();
            if( loaded )
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/** 
*/
void
nFxEventTrigger::BeginGroup( nString& fxType, int numFx )
{
    nFxSpecial group;
    group.fxsubtype = this->StringToType(fxType.Get());
    group.fxlist.SetFixedSize(numFx);
    this->fxMatIndependent.Append(group);
}

//-----------------------------------------------------------------------------
/**
*/
bool
nFxEventTrigger::AddFxClassToGroup( int numFx, nString& fxClassName )
{
    nFxSpecial& group = this->fxMatIndependent.Back();
    group.fxlist[numFx].fxclass = fxClassName;
    group.fxlist[numFx].key = 0;
    return true;
}

//-----------------------------------------------------------------------------
/**
*/
void
nFxEventTrigger::EndGroup()
{
    //
}

//-----------------------------------------------------------------------------
/**
*/
bool
nFxEventTrigger::AddFxSpecialClass(nString& fxClassName, nString& subtype )
{
    nFxClasses uniqueClass;
    uniqueClass.fxclass = fxClassName;
    uniqueClass.key = 0;
    
    nFxSpecial fxspecial;
    fxspecial.fxlist.Append(uniqueClass);
    fxspecial.fxsubtype = this->StringToType(subtype.Get());

    this->fxMatIndependent.Append(fxspecial);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nFxEventTrigger::TypeToString(nFxSubType t)
{
    switch (t)
    {
        case MELEE_LEFT:           return "melee_left";
        case MELEE_CENTER:         return "melee_center";
        case MELEE_RIGHT:          return "melee_right";
        case SHOT_FRONT:         return "shot_front";
        case SHOT_BACK:          return "shot_back";
        case SHOT_LEFT_FRONT:    return "shot_left_front";
        case SHOT_RIGHT_FRONT:   return "shot_right_front";
        case SHOT_LEFT_BACK:     return "shot_left_back";
        case SHOT_RIGHT_BACK:    return "shot_right_back";
        case SHOT_LEFT:          return "shot_left";
        case SHOT_RIGHT:         return "shot_right";
        case EXPLOSION:           return "explosion";
        case M4_1P_FIRE:          return "m4_1p_fire";
        case M4_3P_FIRE:          return "m4_3p_fire";
        case SCOPE:               return "scope";

        default:
            n_error("nFxEventTrigger::TypeToString(): invalid subtype value '%d'!", t);
            return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFxEventTrigger::nFxSubType
nFxEventTrigger::StringToType(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "melee_left"))         return MELEE_LEFT;
    if (0 == strcmp(str, "melee_center"))       return MELEE_CENTER;
    if (0 == strcmp(str, "melee_right"))        return MELEE_RIGHT;
    if (0 == strcmp(str, "shot_front"))       return SHOT_FRONT;
    if (0 == strcmp(str, "shot_back"))        return SHOT_BACK;
    if (0 == strcmp(str, "shot_left_front"))  return SHOT_LEFT_FRONT;
    if (0 == strcmp(str, "shot_right_front")) return SHOT_RIGHT_FRONT;
    if (0 == strcmp(str, "shot_left_back"))   return SHOT_LEFT_BACK;
    if (0 == strcmp(str, "shot_right_back"))  return SHOT_RIGHT_BACK;
    if (0 == strcmp(str, "shot_left"))        return SHOT_LEFT;
    if (0 == strcmp(str, "shot_right"))       return SHOT_RIGHT;
    if (0 == strcmp(str, "explosion"))         return EXPLOSION;
    if (0 == strcmp(str, "m4_1p_fire"))        return M4_1P_FIRE;
    if (0 == strcmp(str, "m4_3p_fire"))        return M4_3P_FIRE;
    if (0 == strcmp(str, "scope"))             return SCOPE;
    if (0 == strcmp(str, "none"))              return INVALIDTYPE;

    n_error("nFxEventTrigger::StringToType(): invalid subtype string '%s'!", str);
        return INVALIDTYPE;
}

//------------------------------------------------------------------------------
/**
*/
void 
nFxEventTrigger::MeleeSplash( const vector3& impactPos, nEntityObject* eObj)
{
    ncGameplayPlayer* player = eObj->GetComponent<ncGameplayPlayer>();
    if( player && player->IsFirstCameraMode() )
    {
        vector3 playerPos, impactPos0;
        player->GetShotPosition(playerPos);
        playerPos.y = 0;

        impactPos0.set(impactPos);
        impactPos0.y = 0;

        vector3 playerFacing;
        player->GetShotDirection(playerFacing);
        
        vector3 impactVector = impactPos0 - playerPos;

        float angleToFront = vector3::angle(playerFacing, impactVector);
        vector3 cross = playerFacing * impactVector;
        float sinAngle = (cross % vector3(0.f, 1.f, 0.f)) /
                         (playerFacing.len() * impactVector.len());

        //front
        static float min = N_PI / 4;
        static float max = N_PI - min;
        // Front
        if( angleToFront <= min )
        {
            this->Splash( nString(this->TypeToString(nFxEventTrigger::MELEE_CENTER)));
        }
        else
        {
            // Rear
            if( angleToFront > max )
            {
                this->Splash( nString(this->TypeToString(nFxEventTrigger::MELEE_RIGHT)));
                this->Splash( nString(this->TypeToString(nFxEventTrigger::MELEE_LEFT)));
            }
            else
            {
                // Left
                if( sinAngle >= 0 )
                {
                    this->Splash( nString(this->TypeToString(nFxEventTrigger::MELEE_LEFT)));
                }
                else
                {
                    // Right
                    this->Splash( nString(this->TypeToString(nFxEventTrigger::MELEE_RIGHT)));
                }
            }
        }
    }
    else
    {
        //do sth to non player living entities??
        return;
    }
}

//------------------------------------------------------------------------------
/**
*/
nFXObject *
nFxEventTrigger::Scope( )
{
    NLOG(specialfx, (9| nFXServer::LAUNCH, "Scope"));

    nFxSpecial* fxnode = this->GetFxFor( SCOPE );
    if( fxnode )
    {
        int key = fxnode->fxlist[0].key;

        if( key != 0 )
        {
            return nFXServer::Instance()->PullEffect(key);
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
