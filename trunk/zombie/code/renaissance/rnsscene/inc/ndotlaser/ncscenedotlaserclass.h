#ifndef NC_SCENEDOTLASERCLASS_H
#define NC_SCENEDOTLASERCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSceneLightClass
    @ingroup Scene
    @author Cristobal Castillo

    @brief Graphics component object for dot laser. it is similar to light

    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/ncscenelightclass.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class ncSceneDotLaserClass : public ncSceneLightClass 
{
    NCOMPONENT_DECLARE(ncSceneDotLaserClass,ncSceneLightClass);

public:
    /// constructor
    ncSceneDotLaserClass();
    /// destructor
    virtual ~ncSceneDotLaserClass();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// Configure the laser projecion
    void SetLaserProjection(float dist1, float radius1, float dist2, float radius2);
    /// Get the laser projection
    void GetLaserProjection(float& dist1, float& radius1, float& dist2, float& radius2);
    /// SetTheLaserEpsilon
    void SetLaserEpsilon(float epsilon);
    /// Get the laser projection
    float GetLaserEpsilon();
    /// @name SaveCmds interface
    //@{
    /// 
    /// set the laser projecion , only use in persistence, not call directly
    void SetLaserProjectionScript(const vector4& v);
    //@}

private:

    /// the laser projection x dist1 , y radius1 , z dist2, w radius 2
    vector4 laserProjection;
    /// float the buble of contact point.
    float epsilon;
};

//------------------------------------------------------------------------------
/**
*/

//------------------------------------------------------------------------------
#endif // !NC_SCENEDOTLASER_H
