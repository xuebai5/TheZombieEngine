#ifndef NC_PHYAREADENSITY_H
#define NC_PHYAREADENSITY_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyAreaDensity
    @ingroup NebulaPhysicsSystem
    @brief A base class to treat an area with a different density.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Area Density
    
    @cppclass ncPhyAreaDensity
    
    @superclass ncPhySimpleObj

    @classinfo A base class to treat an area with a different density.
*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphysimpleobj.h"

//-----------------------------------------------------------------------------
class ncPhyAreaDensity : public ncPhySimpleObj
{

    NCOMPONENT_DECLARE(ncPhyAreaDensity,ncPhySimpleObj);

public:
    /// constructor
    ncPhyAreaDensity();

    /// destructor
    ~ncPhyAreaDensity();

    /// check if the box is under the fluid
    virtual bool IsUnderFluid( const nPhysicsAABB& box ) const;

    /// process function
    virtual void PreProcess();

    /// function to be procesed during the collision check
    virtual bool CollisionProcess( int numContacts, nPhyCollide::nContact* contacts );

    /// begin:scripting

    /// sets the resistance value of the fluid
    void SetResistance(const phyreal);
    /// gets the resistance value of the fluid
    phyreal GetResistance() const;

    /// end:scripting

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

protected:

    /// resistance of the fluid
    void Resistance( ncPhysicsObj* obj );

private:

    /// fluid resistance value
    phyreal fluidResistance;

};

//-----------------------------------------------------------------------------
/**
    Sets the resistance value of the fluid.

    @param value resistance coeficient ranges from 0 to 1.

    history:
        - 17-Mar-2005   David Reyes    created
*/
inline
void ncPhyAreaDensity::SetResistance( const phyreal value )
{
    n_assert2( value < 1, "Resistance value out of range (1..0)." );
    n_assert2( value >= 0, "Resistance value out of range (1..0)." );

    this->fluidResistance = phyreal(1)-value;
}

//-----------------------------------------------------------------------------
/**
    Sets the resistance value of the fluid.

    @return resistance coeficient ranges from 0 to 1.

    history:
        - 17-Mar-2005   David Reyes    created
*/
inline
phyreal ncPhyAreaDensity::GetResistance() const
{
    return this->fluidResistance;
}

#endif 