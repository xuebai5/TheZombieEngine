#ifndef N_PHYSPACE_H
#define N_PHYSPACE_H
//-----------------------------------------------------------------------------
/**
    @class nPhySpace
    @ingroup NebulaPhysicsSystem
    @brief A geometry space interface

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Space Interface
    
    @cppclass nPhySpace
    
    @superclass nPhysicsGeom

    @classinfo A geometry space interface.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"
#include "util/ntag.h"

//-----------------------------------------------------------------------------
class nPhySpace : public nPhysicsGeom 
{
public:
    /// constructor
    nPhySpace();
            
    /// destructor
    virtual ~nPhySpace();

    /// adds a geometry to the space
    virtual void Add( nPhysicsGeom* geometry );

    /// removes a geometry to the space
    virtual void Remove( nPhysicsGeom* geometry );

    /// sets the world where the space belongs
    nPhysicsWorld* GetWorld() const;

    /// sets the world of this space
    void SetWorld( nPhysicsWorld* world );

    /// enables this space and the geometries within
    void Enable();

    /// disables this space and the geometries within
    void Disable();

    /// returns a geometry by index
    virtual nPhysicsGeom* GetGeometry( int index );

#ifndef NGAME
    /// draws the geometries contained in this space
    virtual void Draw( nGfxServer2* server );
#endif

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// sets the position of this geometry
    virtual void SetPosition( const vector3& newposition );

    /// sets the euler orientation
    virtual void SetOrientation( phyreal ax, phyreal ay, phyreal az );

    /// returns current position
    virtual void GetPosition( vector3& position ) const;

    /// returns current orientation
    virtual void GetOrientation( matrix33& orientation ) const;

    /// returns current angles
    virtual void GetOrientation( phyreal& ax, phyreal& ay, phyreal& az );

    /// sets space's name
    void SetName( const nString& name);

    /// gets the space name in tag
    const nString& GetName() const;

    /// gets the space name tag
    const nTag& GetTagName() const;

    /// hold out of space
    void HoldOutOfSpace();

    /// reconnects the geometry to the space
    void ReconnectHoldSpace();

    /// returns the number of geometries contained in the space
    virtual int GetNumGeometries() const;

protected:

    /// creates the geometry
    virtual void Create();

private:
    /// stores the holded parent space
    spaceid holdedSpaceID;

    /// stores the world's space
    nPhysicsWorld* spaceWorld;

    /// tag space name
    nTag tagSpaceName;

    /// space name
    nString spaceName;
};

//-----------------------------------------------------------------------------
/**
    Sets the world where the space belongs.

    @return world

    history:
        - 21-Mar-2005   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
nPhysicsWorld* nPhySpace::GetWorld() const
{
    return this->spaceWorld;
}

//-----------------------------------------------------------------------------
/**
    Sets the space name.

    @param name space's name

    history:
        - 30-May-2005   David Reyes    created
*/
inline
void nPhySpace::SetName( const nString& name )
{
    this->spaceName = name;
    this->tagSpaceName = name;
}

//-----------------------------------------------------------------------------
/**
    Gets the space name in tag.

    @return space's name

    history:
        - 30-May-2005   David Reyes    created
*/
inline
const nString& nPhySpace::GetName() const
{
    return this->spaceName;
}

//-----------------------------------------------------------------------------
/**
    Gets the space name tag.

    @return a tag

    history:
        - 30-May-2005   David Reyes    created
*/
inline
const nTag& nPhySpace::GetTagName() const
{
    return this->tagSpaceName;
}

//-----------------------------------------------------------------------------
/**
    Hold out of space.

    history:
        - 27-Jun-2005   David Reyes    created
        - 02-Jul-2005   David Reyes    inlined
*/
inline
void nPhySpace::HoldOutOfSpace()
{
    if( this->SpaceId() != NoValidID )
    {
        this->holdedSpaceID = this->SpaceId();
        phyRemoveFromSpace( geomid(this->holdedSpaceID), this->geomID );
    }
}

//-----------------------------------------------------------------------------
/**
    Reconnects the geometry to the space.

    history:
        - 27-Jun-2005   David Reyes    created
        - 02-Jul-2005   David Reyes    inlined
*/
inline
void nPhySpace::ReconnectHoldSpace()
{
    if( this->holdedSpaceID != NoValidID )
    {
        phyAddToSpace( geomid(this->holdedSpaceID), this->geomID );
    }
}

//-----------------------------------------------------------------------------
/**
    Returns a geometry by index.

    @param index index of the geometry to be retrieved

    history:
        - 23-Sep-2005   David Reyes    created
        - 08-Mar-2006   David Reyes    inlined
*/
inline
nPhysicsGeom* nPhySpace::GetGeometry( int index )
{
    return static_cast<nPhysicsGeom*>( phyRetrieveDataGeom(phyGetGeometryFromSpace(reinterpret_cast<dSpaceID>(this->Id()),index)));
}

//-----------------------------------------------------------------------------
/**
    Returns the number of geometries contained in the space.

    @return number of geometries within the space

    history:
        - 02-Jul-2005   David Reyes    created
        - 08-Mar-2006   David Reyes    inlined
*/
inline
int nPhySpace::GetNumGeometries() const
{
    return phyGetNumGeometriesWithinASpace( reinterpret_cast<spaceid>(this->Id()) );
}

#endif 
