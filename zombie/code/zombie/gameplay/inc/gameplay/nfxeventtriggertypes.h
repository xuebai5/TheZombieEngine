#ifndef N_FXEVENTTRIGGERTYPES_H
#define N_FXEVENTTRIGGERTYPES_H
//------------------------------------------------------------------------------
/**
    @struct nLaunchSpecialFxData
    @brief Module contaisn the data for nFxEventTrigger

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#include "util/nstring.h"
#include "mathlib/vector.h"
class nFXObject;
class nEntityObject;
class nGameMaterial;

struct nLaunchSpecialFxData
{
    /// default constructor 0
    nLaunchSpecialFxData();
    /// default constructor 1
    nLaunchSpecialFxData(const char* eventType,const vector3& impactPoint, nGameMaterial* gameMat, nEntityObject* entityObject, float scale, const vector3& normal);
    /// The name of event example Impact_M4
    nString eventType;
    /// The position of 
    vector3 impactPoint;
    /// the material, if 0 launch the generic effect
    nGameMaterial* gameMat;
    /// Aply to object, example decals
    nEntityObject* entityObject;
    /// scale the object
    float scale;
    vector3 impactNormal;
};


inline
nLaunchSpecialFxData::nLaunchSpecialFxData(const char* eventType,const vector3& impactPoint, nGameMaterial* gameMat, nEntityObject* entityObject, float scale, const vector3& normal) :
    eventType( eventType ),
    impactPoint( impactPoint ),
    gameMat( gameMat ),
    entityObject( entityObject ),
    scale( scale ),
    impactNormal( normal)
{
    //empty
}

inline
nLaunchSpecialFxData::nLaunchSpecialFxData() :
    gameMat(0),
    entityObject(0)
{
}

//------------------------------------------------------------------------------
#endif//NFXEVENTTRIGGERTYPES_H
