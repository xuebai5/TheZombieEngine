#ifndef N_PHYCONTACTJOINT_H
#define N_PHYCONTACTJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhyContactJoint
    @ingroup NebulaPhysicsSystem
    @brief Representation of a contact joint.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Contact Joint
    
    @cppclass nPhyContactJoint
    
    @superclass nPhysicsJoint

    @classinfo Representation of a contact joint.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphysicsjoint.h"
#include "nphysics/nphymaterial.h"

//-----------------------------------------------------------------------------

class nPhyContactJoint : public nPhysicsJoint
{
public:
    /// constructor
    nPhyContactJoint();

    /// destructor
    ~nPhyContactJoint();

    /// creates the join
    void CreateIn( nPhysicsWorld* world, nPhyJointGroup* group );

    /// sets data contact
    void SetData( nPhyMaterial::idmaterial idA, nPhyMaterial::idmaterial idB );

    /// sets data contact
    void SetData( const nPhyCollide::nContact& contact );

    /// Sets data contact
    void SetData( const vector3& direction );

    /// sets a parameters of the joints
    void SetParam( const phy::jointparameter, const phy::jointaxis, const phyreal );

    /// sets data contact
    void SetData( surfacedata& data );

    /// sets the friction direction
    void SetFrictionDirection( const vector3& fdir );

    /// get contacts normal
    void GetContactNormal( vector3& normal );
    
    /// returns the slipness of the material's value.
    const phyreal GetSlipCoeficient( const bool first = false ) const;

    /// sets the slipness of the material's value.
    void SetSlipCoeficient( const phyreal coeficient, const bool first = false );

    /// scales the object
    void Scale( const phyreal factor );

private:

    // contact data
    contactdata data;

};

//-----------------------------------------------------------------------------
/**
    Sets data contact.

    @param properties contact properties

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyContactJoint::SetData( nPhyMaterial::idmaterial idA, nPhyMaterial::idmaterial idB )
{
    phySetMaterial( this->data, *nPhyMaterial::GetMaterial( idA, idB ) );
}

//-----------------------------------------------------------------------------
/**
    Sets data contact.

    @param properties contact information

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyContactJoint::SetData( const nPhyCollide::nContact& contact )
{
    phySetContact( this->data, contact.GetContact() );
}

//-----------------------------------------------------------------------------
/**
    Sets data contact.

    @param properties contact information

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyContactJoint::SetData( const vector3& direction )
{
    phySetContactDirection( this->data, direction );
}

//-----------------------------------------------------------------------------
/**
    Sets data contact.

    @param properties contact information

    history:
        - 25-Oct-2004   Zombie         created
        - 12-Dec-2004   Zombie         inlined
*/
inline
void nPhyContactJoint::SetData( surfacedata& data )
{
    phySetMaterial( this->data, data );
}

//-----------------------------------------------------------------------------
/**
    Sets the friction direction.

    @param dir friction direction

    history:
        - 18-Apr-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyContactJoint::SetFrictionDirection( const vector3& fdir )
{
    phySetContactDirection( this->data, fdir );
}

//-----------------------------------------------------------------------------
/**
    Get contacts normal.

    @param normal vertor normal

    history:
        - 18-Apr-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyContactJoint::GetContactNormal( vector3& normal )
{
    phyGetContactNormal( this->data, normal );
}

//-----------------------------------------------------------------------------
/**
    Returns the slipness of the material's value.

    @param first true = first, false = second

    @return slipness coeficient

    history:
        - 18-Apr-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
const phyreal nPhyContactJoint::GetSlipCoeficient( const bool first ) const
{
    return phyGetSlipCoeficient( this->data, first );
}

//-----------------------------------------------------------------------------
/**
    Sets the slipness of the material's value.

    @param coeficient slipness coeficient   
    @param first true = first, false = second

    history:
        - 18-Apr-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyContactJoint::SetSlipCoeficient( const phyreal coeficient, const bool first )
{
    phySetSlipCoeficient( this->data, coeficient, first );
}

//------------------------------------------------------------------------------
/**
	Scales the object.
    
    @param factor scale factor

    history:
     - 12-May-2005   Zombie         created
     - 12-May-2005   Zombie         inlined
*/
inline
void nPhyContactJoint::Scale( const phyreal /*factor*/ )
{
    /// reseting attachment
    this->Attach( this->GetBodyA(), this->GetBodyB() );
}

#endif 