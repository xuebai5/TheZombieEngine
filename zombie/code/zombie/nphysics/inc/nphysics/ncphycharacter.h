#ifndef NC_PHYCHARACTER_H
#define NC_PHYCHARACTER_H
//-----------------------------------------------------------------------------
/**
    @class ncPhyCharacter
    @ingroup NebulaPhysicsSystem
    @brief Represents any characters physics by a cylinder simulation.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Character
    
    @cppclass ncPhyCharacter
    
    @superclass ncPhyCompositeObj

    @classinfo A class that represents a character.
*/

#include "nphysics/ncphycompositeobj.h"

//-----------------------------------------------------------------------------
class nPhyAngularMotorJoint;
class nPhyGeomCylinder;
//-----------------------------------------------------------------------------

class ncPhyCharacter : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyCharacter,ncPhyCompositeObj);

public:
    /// constructor
    ncPhyCharacter();
    /// destructor
    ~ncPhyCharacter();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// sets the character height
    void SetHeight(phyreal);
    /// gets the character height
    phyreal GetHeight() const;
    /// sets the character wideness
    void SetWide(phyreal);
    /// gets the character wideness
    phyreal GetWide() const;

    /// creates the object
    void Create( nPhysicsWorld* world );

    /// function to be process b4 running the simulation
    void PreProcess();

    /// function to be process after running the simulation.
    void PostProcess();

    /// adds a force to the body
    void AddForce( const vector3& force );

    /// deactivated the object
    void Deactivate();

    /// deactivated the object
    void Activates();

    /// changes character height
    void ChangeHeight( const phyreal newHeight );

private:

    /// stores the character's height
    phyreal height;

    /// stores the character's wideness
    phyreal wideness;

    /// stores the angular motor
    nPhyAngularMotorJoint* motor;

    /// pointer to the cylinder
    nPhyGeomCylinder* cylinder;

    /// stores if the object it's active
    bool active;

};

#endif
