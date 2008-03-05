#ifndef NC_PHYWHEEL_H
#define NC_PHYWHEEL_H
//-----------------------------------------------------------------------------
/**
    @class ncPhyWheel
    @ingroup NebulaPhysicsSystem
    @brief Specializated object that represents a vehicle wheel.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Wheel.
    
    @cppclass ncPhyWheel
    
    @superclass ncPhyCompositeObj

    @classinfo Specializated object that represents a vehicle wheel.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphycompositeobj.h"

//-----------------------------------------------------------------------------

class ncPhyVehicle;

//-----------------------------------------------------------------------------
class ncPhyWheel : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyWheel,ncPhyCompositeObj);

public:

    /// constructor
    ncPhyWheel();

    /// destructor
    ~ncPhyWheel();

    /// sets the vehicle that owns this wheel
    void SetVehicle( ncPhyVehicle* vehicle );

    /// gets the vehicle that owns this wheel
    ncPhyVehicle* GetVehicle() const;

    /// processes the information when a dynamic material is finded
    void ProcessDynamicMaterial( nPhyContactJoint* cjoint );

    /// resets the wheel state
    void Resets();

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// process b4 run the world
    void PreProcess();

    /// process after run the world
    void PostProcess();

private:

    /// stores the vehicle where the wheel belongs if any
    ncPhyVehicle* wheelOwner;

    /// stores the dynamic material default properties
    surfacedata dynamicMaterialData;

    /// counter of contacts
    int contacts;

    /// stores if the precomputed values has been calculated
    bool preComputedValues;

    /// precomputes the values that will be repeated during a single step
    void PreCompute();

    /// stores precomputed speed
    phyreal preComputedSpeed;

    /// stores precomputed front direction
    vector3 preComputeFrontDirection;

    /// stores precomputed up direction
    vector3 preComputeUpDirection;

};

#endif