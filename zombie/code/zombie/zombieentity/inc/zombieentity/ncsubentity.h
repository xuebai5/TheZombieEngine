#ifndef N_SUBENTITY_H
#define N_SUBENTITY_H
//------------------------------------------------------------------------------
/**
    (C) 2006 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "signals/nsignalnative.h"

//------------------------------------------------------------------------------
class nObjectMagicInstancer;

//------------------------------------------------------------------------------
class ncSubentity : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSubentity, nComponentObject);

public:
    /// Constructor
    ncSubentity();

    /// Destructor
    ~ncSubentity();

      /// Initialize the component
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Set relative position
    void SetRelativePosition(const vector3 &);
    /// Get relative position
    const vector3 & GetRelativePosition() const;
    /// Set relative transform
    void SetRelativeTransform(const quaternion &);
    /// Set relat
    void SetRelativeScale(const vector3 &scale);
    /// Get relative transform
    const quaternion & GetRelativeTransform() const;
    /// Set persistence oid
    void SetPersistenceOID(nEntityObjectId);
    /// Get persistence oid
    nEntityObjectId GetPersistenceOID();

    /// return true if entity object is a subentity
    bool IsSubentity() const;

    /// @name Signals interface
    //@{
    NSIGNAL_DECLARE( 'JUER', void, UpdateEntityReferences, 0, (), 0, ());
    //@}

    /// Get entity to wich this subentity belongs
    nEntityObject* GetSuperentity();

    /// Set entity to wich this subentity belongs
    void SetSuperentity( nEntityObject* );
#ifndef NGAME
    /// Update relative position of subentity from current global position
    void UpdateRelativePosition();
#endif
    /// Update relative position of subentity from current global position
    void UpdatenTrasformFromRelativePosition();

    /// signal to let you know that all the entities have been created
    NSIGNAL_DECLARE('DSEC', void, SubEntitiesCreated, 0, (), 0, ());

private:

    friend class ncPersistHelperSubEntity;

    /// Search an OID in this superentity's list of subentities.
    bool SearchEntity( nEntityObjectId& roid ) const;

    /// Subentity id for persisting
    nEntityObjectId subEntityId;

    /// Relative position of subentity w.r. to owner entity
    vector3 relativePos;

    /// Relative scale of subentity w.r. to owner entity, if the superentity scale is identity this scale is the same of this nctransform
    vector3 relativeScale;

    /// Relative pose of subentity w.r. to owner entity
    quaternion relativeTransform;

    /// Entity to wich this subentity belongs
    nEntityObject* superEntity;
    
};

#endif
