#ifndef N_WORLDINTERFACE_H
#define N_WORLDINTERFACE_H

//------------------------------------------------------------------------------
/**
    @class nWorldInterface
    @ingroup Framework

    Interface used from the scripts to access the world with high level commands.

    (C) 2005 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nentityobjectserver.h"

class nEntityObject;

//------------------------------------------------------------------------------
class nWorldInterface : public nRoot
{
public:
    // Default constructor
    nWorldInterface();
    // Destructor
    ~nWorldInterface();

    /// Return singleton world interface
    static nWorldInterface* Instance();

    /// Remove an entity from the world and delete it
    void DeleteEntity( nEntityObject* entity ) const;

    /// Get an entity by its name as assigned by the level
    nEntityObject* GetLevelEntity(const char*) const;
    /// Get an entity by its name as assigned by the gameplay
    nEntityObject* GetGameEntity(const char*) const;
    /// Create a new entity and place it in the world ready to be used
    nEntityObject* NewEntity(const char*, vector3) const;
    /// Create a new entity and place it in the world ready to be used
    nEntityObject* NewLocalEntity(const char*, vector3, bool, nEntityObject*) const;
    /// Create a new entity and place it in the world ready to be used
    nEntityObject* NewServerEntity(const char*, vector3, nEntityObject*) const;
    /// Remove an entity from the world and delete it
    void DeleteEntityById(nEntityObjectId) const;

    /// Add the entity to the world
    void AddToWorld(nEntityObject *) const;
    /// Remove the entity from the world
    void RemoveFromWorld(nEntityObject *) const;

    NSIGNAL_DECLARE( 'EECR', void, EntityCreated, 1, (nEntityObject*), 0, () );

private:

    /// Private method for creating an entity
    nEntityObject* CreateEntity( const char* className, vector3 position, nEntityObjectServer::nEntityObjectType entityScope, nEntityObject* superEntity ) const;

    /// Singleton instance
    static nWorldInterface* instance;

};

//------------------------------------------------------------------------------
inline
nWorldInterface* nWorldInterface::Instance()
{
    n_assert( nWorldInterface::instance );
    return nWorldInterface::instance;
}

//------------------------------------------------------------------------------
#endif // N_WORLDINTERFACE_H
