#ifndef N_PHYMATERIAL_H
#define N_PHYMATERIAL_H
//-----------------------------------------------------------------------------
/**
    @class nPhyMaterial
    @ingroup NebulaPhysicsSystem
    @brief A physics material abstract representation

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Material
    
    @cppclass nPhyMaterial
    
    @superclass nRoot

    @classinfo A physics material abstract representation.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "nphysics/nphysicsconfig.h"
#include "kernel/ncmdprotonativecpp.h"

//-----------------------------------------------------------------------------

class nPhyMaterial : public nRoot
{
public:
    /// type of material id
    typedef int idmaterial;

    /// type contact
    typedef enum {
        None = 0,
        NormalFriction,
        Bounce,
        SoftnessERP,
        SoftnessCFM,
        Slip,
        PyramidFriction,
        FrictionDirection,
        End,
        Num = End,
        Invalid
    } typeContact;

    /// constructor
    nPhyMaterial();

    /// destructor
    ~nPhyMaterial();

    /// returns the ID
    const idmaterial Id() const;

    /// checks if a type belongs to this material
    bool IsType( typeContact type ) const;

#if defined(_DEBUG) || !defined(NGAME)
    /// DEBUG: checks that a material is correct
    static bool Validate( const int material );
#endif

    /// returns a material type trough the id
    static nPhyMaterial* GetMaterial( idmaterial id );

    /// returns a material combination of two
    static surfacedata* GetMaterial( idmaterial idA, idmaterial idB );

    /// begin:scripting

    /// DEBUG: Sets the material name
    void SetMaterialName(const nString&);
    /// DEBUG: Gets the material name
    const nString& GetMaterialName() const;
    /// Sets the normal friction coeficient
    void SetCoeficientNormalFriction(phyreal);
    /// Gets the normal friction coeficient
    phyreal GetCoeficientNormalFriction() const;
    /// Sets the bounce coeficient (0..1)
    void SetCoeficientBounce(phyreal);
    /// Gets the bounce coeficient (0..1)
    phyreal GetCoeficientBounce() const;
    /// Sets the softness trough ERP (Error reduction parameter)
    void SetCoeficientSoftnessERP(phyreal);
    /// Gets the softness trough ERP (Error reduction parameter)
    phyreal GetCoeficientSoftnessERP() const;
    /// Sets the softness trough CFM (Constraint force mixing)
    void SetCoeficientSoftnessCFM(phyreal);
    /// Gets the softness trough CFM (Constraint force mixing)
    phyreal GetCoeficientSoftnessCFM() const;
    /// Sets the slip coeficient
    void SetCoeficientSlip(phyreal);
    /// Gets the slip coeficient
    phyreal GetCoeficientSlip() const;
    /// Sets the pyramid friction coeficient
    void SetCoeficientPyramidFriction(phyreal);
    /// Gets the pyramid friction coeficient
    phyreal GetCoeficientPyramidFriction() const;
    /// Sets the bounce minimun velocity (0..8) default 0 m/s
    void SetMinVelocityBounce(phyreal);
    /// Gets the bounce minimun velocity (0..8) default 0 m/s
    phyreal GetMinVelocityBounce() const;
    /// enables friction direction
    void EnableFrictionDirection();
    /// disables friction direction
    void DisableFrictionDirection();
    /// updates the table of materials
    void Update();
    /// sets the pyramid friction coeficient to infinity
    void SetCoeficientNormalFrictionInf();
    /// sets the bounce coeficient to infinity
    void SetCoeficientBounceInf();
    /// sets the softness trough ERP (Error reduction parameter) to Infinity
    void SetCoeficientSoftnessERPInf();
    /// sets the softness trough CFM (Constaint force mixing) to Infinity
    void SetCoeficientSoftnessCFMInf();
    /// sets the slip coeficient to Infinity
    void SetCoeficientSlipInf();
    /// sets the pyramid friction coeficient to Infinity
    void SetCoeficientPyramidFrictionInf();
    /// sets the bounce minimun velocity  to Infinity
    void SetMinVelocityBounceInf();

    /// end:scripting

    /// creates the table of material contacts
    static void UpdateTableMaterialContacts();

    /// persists all the materials
    static const bool PersistMaterials( nPersistServer* server );

#ifndef NGAME
    /// return if the object it's meant to be removed
    bool GetToBeRemoved() const;

    /// set to be removed
    void SetToBeRemoved( bool has );

#endif

private:

#ifndef NGAME
    bool hasToBeRemoved;
#endif

    /// DEBUG: name of the material
    nString nameMaterial;
    
    /// stores the unique id of the material
    idmaterial id;

    /// stores the flags representing the properties of the material
    unsigned int flags;

    /// stores the friction coeficient (normal & pyramid)
    phyreal frictionCoeficient;

    /// stores bounce coeficient
    phyreal bounceCoeficient;

    /// stores softness ERP coeficient
    phyreal softnessERPCoeficient;

    /// stores softness CFM coeficient
    phyreal softnessCFMCoeficient;

    /// store slip coeficient
    phyreal slipCoeficient;

    /// stores velocity for bouncing
    phyreal velocityBouncing;

    /// sets the flag for a type of contact
    void SetFlag( typeContact type );

    /// unsets the flag for a type of contact
    void UnsetFlag( typeContact type );

    /// operator to mix two materials
    const surfacedata operator + ( const nPhyMaterial& material ) const; 

    /// fills a surface data information
    void Fill( surfacedata& data ) const;

    /// type of container of materials
    typedef nKeyArray<nPhyMaterial*> tContainerMaterials;

    /// stores all the created materials
    static tContainerMaterials listMaterials;

    /// stores the number of materials stored
    static int MaterialsSize;

    /// initial space in the list of materials
    static const int NumInitialMaterialsSpace = 10;

    /// growth pace
    static const int NumMaterialsGrowthPace = 10;

    /// table with the contain of all the materials + (None)
    static surfacedata* materialsTable;

    /// object persistency
    bool SaveCmds(nPersistServer *ps);
};

//-----------------------------------------------------------------------------
/**
    Returns a material type trough the id.

    @param idA material id
    @param idB material id

    @return material data

    history:
        - 29-Sep-2004   David Reyes   created
        - 27-Jan-2005   David Reyes   inline

*/
inline
surfacedata* nPhyMaterial::GetMaterial( idmaterial idA, idmaterial idB )
{
#ifndef NGAME
    n_assert2( Validate( idA ), "Not valid material id" );
    n_assert2( Validate( idB ), "Not valid material id" );
#endif

    return materialsTable + (MaterialsSize*idB) + idA;
}

//-----------------------------------------------------------------------------
/**
    Sets the pyramid friction coeficient to infinity.

    history:
        - 04-May-2004   David Reyes   created

*/
inline
void nPhyMaterial::SetCoeficientNormalFrictionInf()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetCoeficientNormalFriction( phyInfinity );
}

//-----------------------------------------------------------------------------
/**
    Sets the bounce coeficient to infinity.

    history:
        - 04-May-2004   David Reyes   created

*/
inline
void nPhyMaterial::SetCoeficientBounceInf()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetCoeficientBounce( phyInfinity );
}

//-----------------------------------------------------------------------------
/**
    Sets the softness trough ERP (Error reduction parameter) to Infinity.

    history:
        - 04-May-2004   David Reyes   created

*/
inline
void nPhyMaterial::SetCoeficientSoftnessERPInf()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetCoeficientSoftnessERP( phyInfinity );
}

//-----------------------------------------------------------------------------
/**
    Sets the softness trough CFM (Constaint force mixing) to Infinity.

    history:
        - 04-May-2004   David Reyes   created

*/
inline
void nPhyMaterial::SetCoeficientSoftnessCFMInf()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetCoeficientSoftnessCFM( phyInfinity );
}

//-----------------------------------------------------------------------------
/**
    Sets the slip coeficient to Infinity.

    history:
        - 04-May-2004   David Reyes   created

*/
inline
void nPhyMaterial::SetCoeficientSlipInf()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetCoeficientSlip( phyInfinity );
}

//-----------------------------------------------------------------------------
/**
    Sets the pyramid friction coeficient to Infinity.

    history:
        - 04-May-2004   David Reyes   created

*/
inline
void nPhyMaterial::SetCoeficientPyramidFrictionInf()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetCoeficientPyramidFriction( phyInfinity );
}

//-----------------------------------------------------------------------------
/**
    Sets the bounce minimun velocity  to Infinity.

    history:
        - 04-May-2004   David Reyes   created

*/
inline
void nPhyMaterial::SetMinVelocityBounceInf()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetMinVelocityBounce( phyInfinity );
}

//-----------------------------------------------------------------------------
/**
    DEBUG: Sets the material name.

    @param name material name

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetMaterialName( const nString& name )
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->nameMaterial = name;
}

//-----------------------------------------------------------------------------
/**
    DEBUG: Sets the material name.

    @return unique id of the material

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
const nPhyMaterial::idmaterial nPhyMaterial::Id() const
{
    return this->id;
}

//-----------------------------------------------------------------------------
/**
    Sets the normal friction coeficient.

    @param coeficient friction coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetCoeficientNormalFriction( phyreal coeficient )
{
    n_assert2( coeficient >= 0, "Negative normal friction coeficient" );

#ifndef NGAME
    this->SetObjectDirty(true);
#endif

    if( coeficient == phyreal(0) )
    {
        this->UnsetFlag( NormalFriction );
        return;
    }

    this->SetFlag( NormalFriction );
    this->UnsetFlag( PyramidFriction );

    this->frictionCoeficient = coeficient;
}

//-----------------------------------------------------------------------------
/**
    Sets the bounce coeficient (0..1).

    @param coeficient bounce coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetCoeficientBounce( phyreal coeficient )
{
    n_assert2( coeficient >= 0, "Negative Bounce coeficient" );
    n_assert2( coeficient <= 1, "Bounce coeficient bigger than one" );

#ifndef NGAME
    this->SetObjectDirty(true);
#endif

    this->SetFlag( Bounce );

    this->bounceCoeficient = coeficient;
}

//-----------------------------------------------------------------------------
/**
    Sets the softness trough ERP (Error reduction parameter).

    @param coeficient softness ERP coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetCoeficientSoftnessERP( phyreal coeficient )
{
    n_assert2( coeficient >= 0, "Negative softness ERP coeficient" );
    
#ifndef NGAME
    this->SetObjectDirty(true);
#endif

    this->SetFlag( SoftnessERP );

    this->softnessERPCoeficient = coeficient;
}

//-----------------------------------------------------------------------------
/**
    Sets the softness trough CFM (Constaint force mixing).

    @param coeficient softness CFM coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetCoeficientSoftnessCFM( phyreal coeficient )
{
    n_assert2( coeficient >= 0, "Negative softness CFM coeficient" );
    
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetFlag( SoftnessCFM );

    this->softnessCFMCoeficient = coeficient;
}

//-----------------------------------------------------------------------------
/**
    Sets the slip coeficient.

    @param coeficient slip coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetCoeficientSlip( phyreal coeficient )
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    n_assert2( coeficient >= 0, "Negative slip coeficient" );

    this->SetFlag( Slip );

    this->slipCoeficient = coeficient;
}

//-----------------------------------------------------------------------------
/**
    Sets the pyramid friction coeficient.

    @param coeficient Pyramid Friction coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetCoeficientPyramidFriction( phyreal coeficient )
{
    n_assert2( coeficient >= 0, "Negative pyramid friction coeficient" );

#ifndef NGAME
    this->SetObjectDirty(true);
#endif

    if( coeficient == phyreal(0) )
    {
        this->UnsetFlag( PyramidFriction );
        return;
    }

    this->SetFlag( PyramidFriction );
    this->UnsetFlag( NormalFriction );

    this->frictionCoeficient = coeficient;
}

//-----------------------------------------------------------------------------
/**
    Sets the bounce minimun velocity (0..8) default 0.

    @param velocity minimun velocity to bounce

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetMinVelocityBounce( phyreal velocity )
{
    n_assert2( velocity >= 0, "Negative bouncing velocity" );

#ifndef NGAME
    this->SetObjectDirty(true);
#endif

    this->SetFlag( Bounce );
    
    this->velocityBouncing = velocity;
}

//-----------------------------------------------------------------------------
/**
    Checks if a type belongs to this material.

    @param type contact type

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
bool nPhyMaterial::IsType( typeContact type ) const
{
    n_assert2( type >= NoValidID, "No valid type" );
    n_assert2( type < End, "No valid type" );

    return this->flags & (1 << int(type-1)) ? true : false;
}

//-----------------------------------------------------------------------------
/**
    Sets the flag for a type of contact.

    @param type type that will be added to the flags list

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::SetFlag( typeContact type )
{
    n_assert2( type != NoValidID, "No valid type" );
    n_assert2( type < End, "No valid type" );

    this->flags |= ( 1 << int(type-1) );
}

//-----------------------------------------------------------------------------
/**
    Unsets the flag for a type of contact.

    @param type type that will be removed to the flags list

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::UnsetFlag( typeContact type )
{
    n_assert2( type != NoValidID, "No valid type" );
    n_assert2( type < End, "No valid type" );

    this->flags &= ~( 1 << int(type-1) );
}

//-----------------------------------------------------------------------------
/**
    Gets the normal friction coeficient.

    @return normal friction coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
phyreal nPhyMaterial::GetCoeficientNormalFriction() const
{
    if( this->IsType( NormalFriction ) )
        return this->frictionCoeficient;
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Gets the bounce coeficient (0..1).

    @return bounce friction coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
phyreal nPhyMaterial::GetCoeficientBounce() const
{
    return this->bounceCoeficient;
}

//-----------------------------------------------------------------------------
/**
    Gets the softness trough ERP (Error reduction parameter).

    @return softness ERP coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
phyreal nPhyMaterial::GetCoeficientSoftnessERP() const
{
    return this->softnessERPCoeficient;
}

//-----------------------------------------------------------------------------
/**
    Gets the softness trough CFM (Error reduction parameter).

    @return softness CFM coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
phyreal nPhyMaterial::GetCoeficientSoftnessCFM() const
{
    return this->softnessCFMCoeficient;
}

//-----------------------------------------------------------------------------
/**
    Gets the slip coeficient.

    @return softness CFM coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
phyreal nPhyMaterial::GetCoeficientSlip() const
{
    return this->slipCoeficient;
}

//-----------------------------------------------------------------------------
/**
    Gets the pyramid friction coeficient.

    @return pyramid friction coeficient

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
phyreal nPhyMaterial::GetCoeficientPyramidFriction() const
{
    if( this->IsType( PyramidFriction ) )
        return this->frictionCoeficient;
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Gets the bounce minimun velocity (0..8) default 0.

    @return minimun velocity for bouncing

    history:
        - 28-Sep-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
phyreal nPhyMaterial::GetMinVelocityBounce() const
{
    return this->velocityBouncing;
}

//-----------------------------------------------------------------------------
/**
    Enables friction direction.

    history:
        - 22-Apr-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::EnableFrictionDirection()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->SetFlag( FrictionDirection );
}

//-----------------------------------------------------------------------------
/**
    Disables the friction direction.

    history:
        - 22-Apr-2004   David Reyes   created
        - 12-May-2005   David Reyes   inlined
*/
inline
void nPhyMaterial::DisableFrictionDirection()
{
#ifndef NGAME
    this->SetObjectDirty(true);
#endif
    this->UnsetFlag( FrictionDirection );
}

#endif