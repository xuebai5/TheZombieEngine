#ifndef NC_PHYINDOOR_H
#define NC_PHYINDOOR_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyIndoor
    @ingroup NebulaPhysicsSystem
    @brief Represents an indoor.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Indoor
    
    @cppclass ncPhyIndoor
    
    @superclass ncPhyCompositeObj

    @classinfo Represents an indoor.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphycompositeobj.h"

//-----------------------------------------------------------------------------

class nPhyGeomSpace;

//-----------------------------------------------------------------------------
#ifdef NGAME
class ncPhyIndoor : public ncPhysicsObj
{

    NCOMPONENT_DECLARE(ncPhyIndoor,ncPhysicsObj);

#else
class ncPhyIndoor : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyIndoor,ncPhyCompositeObj);

#endif

public:
    /// type of container for the indoor spaces
    typedef nKeyArray<nPhySpace*> tSpaces;

    /// constructor
    ncPhyIndoor();

    /// destructor
    ~ncPhyIndoor();

    /// enables the indoor
    void Enable();

    /// disables the indoor
    void Disable();

    /// creates the object
    void Create( nPhysicsWorld* world );

    /// sets the indoor's name
    void SetIndoorName( const nString& name );

    /// adds an object to this indoor
    void Add( ncPhysicsObj* object );

    /// removes an object to this indoor
    void Remove( ncPhysicsObj* object );

    /// inits the object
    void InitInstance(nObject::InitInstanceMsg initType);

    /// returns the indoor space
    nPhySpace* GetIndoorSpace();

    /// returns the list of sub-spaces
    tSpaces& GetSubSpaces();
    
    /// adds a space to the indoor
    void Add( nPhySpace* space );

    /// rearranges spaces
    void RearrangeSpaces();

    /// returns the current indoor that is  loading, if any indoor is not loading return null
    static ncPhyIndoor* Loading();

    /// Initilize this for a begin to run the instancier
    void BeginRunInstancier();

    /// Loads a objects from an instancier
    void RunInstancier( const nString& filename );

    /// ends the first load of objects
    void EndRunInstancier();

    /// returns the bounding box of this geometry
    void GetAABB( nPhysicsAABB& boundingbox );

    /// persists the objects. (does nothing for this component).
    bool SaveCmds(nPersistServer* ps);

#ifdef NGAME
    /// sets position of the full indoor.
    void SetPosition( const vector3& newposition );
    /// sets the rotation of this physic object.
    void SetRotation( const matrix33& newrotation );

#endif

private:
    /// stores the last loading indoor
    static ncPhyIndoor* lastLoading;

    /// number of initial geometries space
    static const int NumInitialSpaces = 4;

    /// growth pace
    static const int NumGrowthSpaces = 4;

    /// stores the indoor space
    nPhySpace* indoorSpace;

    /// creates the indoor's space
    void CreateSpace();

    /// container for the indoor spaces
    tSpaces spaces;

    /// type of container for the indoor spaces
    typedef nKeyArray<ncPhysicsObj*> tObjects;

    /// number of initial geometries space
    static const int NumInitialObjects = 4;

    /// growth pace
    static const int NumGrowthObjects = 4;

    /// container for the indoor objects
    tObjects objectsContainer;

#ifdef NGAME
    /// stores the indoor last position
    vector3 position;

    /// stores the indoor last orientation
    matrix33 orientation;
#endif

};

//-----------------------------------------------------------------------------
/**
    Returns the indoor space.

    @return space

    history:
        - 01-Jun-2005   David Reyes    created
*/
inline
nPhySpace* ncPhyIndoor::GetIndoorSpace()
{
    return this->indoorSpace;
}

//-----------------------------------------------------------------------------
/**
    Returns the list of sub-spaces.

    @return sub spaces list

    history:
        - 01-Jun-2005   David Reyes    created
*/
inline
ncPhyIndoor::tSpaces& ncPhyIndoor::GetSubSpaces()
{
    return this->spaces;
}

#endif