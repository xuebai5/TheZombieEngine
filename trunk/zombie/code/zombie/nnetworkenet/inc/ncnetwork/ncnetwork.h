#ifndef NCNETWORK_H
#define NCNETWORK_H
//------------------------------------------------------------------------------
/**
    @class ncNetwork
    @ingroup Entities

    Component Object that control the network behaviour.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "util/nstream.h"

//------------------------------------------------------------------------------
class ncNetwork : public nComponentObject
{

    NCOMPONENT_DECLARE(ncNetwork,nComponentObject);

public:
    /// constructor
    ncNetwork();
    /// destructor
    virtual ~ncNetwork();

    /// init instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// frame update of component
    virtual void Update( float frameTime );
    /// tell the update data from / to the network
    virtual void UpdateNetwork( nstream & buffer );
    /// init an instance with network data
    virtual bool InitFromNetwork( nstream & buffer );
    /// set the local role of entity
    void SetLocalRole(const nNetworkRole role );
    /// get the local role of entity
    nNetworkRole GetLocalRole() const;
    /// set the remote role of entity
    void SetRemoteRole(const nNetworkRole role );
    /// get the remote role of entity
    nNetworkRole GetRemoteRole() const;
    /// update a saved state in the entity
    void UpdateState();
    /// set the saved state for update the next time
    void SetSavedState( const char * buffer, int size );

    /// @name Script interface
    //@{
    /// say if the entity is modified
    bool IsModified ();
    /// set that the entity is modified
    void SetModified ();
    /// set that the entity is clean, and not need to network update
    void SetUpdated ();
    //@}

private:
    nNetworkRole localRole;
    nNetworkRole remoteRole;

    char * savedState;
    int stateSize;

    bool updated;

};

//------------------------------------------------------------------------------
/**
    @param role new local role
*/
inline
void
ncNetwork::SetLocalRole( const nNetworkRole role )
{
    this->localRole = role;
}

//------------------------------------------------------------------------------
/**
    @returns the local role
*/
inline
nNetworkRole
ncNetwork::GetLocalRole( ) const
{
    return this->localRole;
}

//------------------------------------------------------------------------------
/**
    @param role new remote role
*/
inline
void
ncNetwork::SetRemoteRole( const nNetworkRole role )
{
    this->remoteRole = role;
}

//------------------------------------------------------------------------------
/**
    @returns the remote role
*/
inline
nNetworkRole
ncNetwork::GetRemoteRole( ) const
{
    return this->remoteRole;
}

//------------------------------------------------------------------------------
/**
    @returns true if entity is modified
*/
inline
bool
ncNetwork::IsModified()
{
    return ! this->updated;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncNetwork::SetUpdated( )
{
    this->updated = true;
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be updated
*/
inline
void
ncNetwork::UpdateNetwork( nstream & /*buffer*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be initialized
*/
inline
bool
ncNetwork::InitFromNetwork( nstream & /*buffer*/ )
{
    return true;
}

//------------------------------------------------------------------------------
#endif//NCNETWORK_H
