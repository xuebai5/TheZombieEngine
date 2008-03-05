#ifndef ncSuperentity_H
#define ncSuperentity_H

#include "entity/nentity.h"
#include "entity/nobjectinstancer.h"

//------------------------------------------------------------------------------
class ncSuperentity : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSuperentity, nComponentObject);

public:
    /// Constructor
    ncSuperentity();

    /// Destructor
    ~ncSuperentity();

    /// Init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Add subentity to this entity
    bool AddSubentity( nEntityObject* subEnt );

    /// Load subentities from associated object instancer 
    void LoadSubentities();

    /// Save subentities to associated object instancer 
    void SaveSubentities();

    /// Get subentities array. Creates it if it's not been created yet
    nObjectInstancer* GetSubentities();

    /// Update subentities global position with relative position and superentity position.
    void UpdateSubentities();

    /// Delete subentities instancer
    void DeleteSubentities();

    /// Update all superentities of same class
    void UpdateAllSuperentities();

    /// Load ncLoader components of all subentities
    void LoadSubentitiesLoaderComponents();

    /// Update physics AABB
    void UpdateAABB();

    /// return the last superEntity that read its subentities
    static nEntityObject* GetCurrentSuperEntity();

#ifndef NGAME
    /// returns if the entity has been loaded or instanciated
    const bool IsLoaded() const;

    /// sets if an entity has been loaded or instanciated
    void SetIsLoaded( const bool is );
        
#endif//!NGAME

private:

    /// Get OID for new subentity
    nEntityObjectId GetNewSubentityOID();

    /// Release subentities instancer
    void ReleaseSubentities();

    /// Get subentities object instancer path
    void GetSubentitiesPath( nString& str);

    /// subentities instancer
    nObjectInstancer* subentities;

#ifndef NGAME
    /// stores if the superentity it's been loaded or instanciated
    bool loaded;
#endif//!NGAME

    // use put it for load sub entities
    static nEntityObject* currentSuperEntity;

};

//------------------------------------------------------------------------------
#endif 
