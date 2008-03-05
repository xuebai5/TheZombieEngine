#ifndef NC_PHYCHARACTEROBJ_H
#define NC_PHYCHARACTEROBJ_H
//-----------------------------------------------------------------------------
/**
    @class ncPhyCharacterObj
    @ingroup NebulaPhysicsSystem
    @brief An abstract physics object where the geometries within are encapsulated 
        by transforms and share the same character

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
#include "nphysics/ncphycompositeobj.h"

//-----------------------------------------------------------------------------
class ncPhyCharacterObj : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyCharacterObj,ncPhyCompositeObj);

public:
    /// constructor
    ncPhyCharacterObj();
    /// destructor
    ~ncPhyCharacterObj();
    /// save state of the component
    bool SaveCmds(nPersistServer *);
    /// object chunk persistency
    bool SaveChunk(nPersistServer *ps);

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// adds a geometry to the object
    void Add(nPhysicsGeom*);

    /// Get Bone Data
    //void GetBoneData( int boneIndex, vector3& position, quaternion& orientation);
    /// update character
    void UpdateCharacterCollision();

    /// get geometry id from bone name
    const geomid GetGeometryId( const nString pattern );

    /// set geometry-joint offset
    void SetGeometryOffset(int, vector3&);
    /// get geometry-joint offset
    void GetGeometryOffset(int, vector3&);

    /// creates the object
    virtual void Create( nPhysicsWorld* world );

    /// function to be process b4 running the simulation
    void PreProcess();

    /// function to be process after running the simulation.
    void PostProcess();

    /// deactivated the object
    void Deactivate();

    /// deactivated the object
    void Activates();

private:
    nArray<nPhysicsGeom*> geometryPointers;
    nArray<quaternion> invGeomInitialQuat;
    nKeyArray<vector3> geomOffset;

    /// stores if the object it's active
    bool active;

    };

#endif
