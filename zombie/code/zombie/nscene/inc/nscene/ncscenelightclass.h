#ifndef NC_SCENELIGHTCLASS_H
#define NC_SCENELIGHTCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSceneLightClass
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component object for light entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncsceneclass.h"
#include "util/nsafekeyarray.h"
#include "util/narray2.h"

//------------------------------------------------------------------------------
class ncSceneLightClass : public ncSceneClass
{

    NCOMPONENT_DECLARE(ncSceneLightClass,ncSceneClass);

public:
    /// constructor
    ncSceneLightClass();
    /// destructor
    virtual ~ncSceneLightClass();

    #ifndef NGAME
    /// dummy interface for the LightType property
    void SetLightType(int);
    int GetLightType();
    #endif
};

//------------------------------------------------------------------------------
#endif
