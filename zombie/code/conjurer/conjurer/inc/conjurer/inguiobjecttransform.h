#ifndef N_INGUI_OBJECT_TRANSFORM_H
#define N_INGUI_OBJECT_TRANSFORM_H
//------------------------------------------------------------------------------
/**
    @file inguiobjecttransform.h
    @class InguiObjectTransform
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Encapsulates the data needed by the editor to make a transform action

    This class is used calling its methods in this order:
     - First, call ad-hoc methods of derived classes for initing the needed data.
     - Call SetInitialMatrix if needed, to set the base or initial matrix
     - Call Begin
     - Call SetTransform with a relative transform meaning the operation performed.
        This will be combined with the initial matrix to obtain the world matrix and applied to the object
     - Call End to finish the operation, or Cancel to cancel it
     - Call GetTransform whenever it's needed to obtain the world matrix

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "entity/nentity.h"
#include "mathlib/transform44.h"
#include "entity/nrefentityobject.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nFloatMap;

//------------------------------------------------------------------------------
class InguiObjectTransform
{
public:

    enum TerrainCollisionType {
        colNever,
        colLessEqual,
        colSetAlways
    };

    InguiObjectTransform();
    ~InguiObjectTransform();

    /// Begin an operation and set the initial matrix
    virtual void Begin();

    /// End the operation
    virtual void End();

    /// Cancel the operation
    virtual void Cancel();

    /// Set the initial matrix
    void SetInitialMatrix( transform44& m );

    /// Get the initial matrix
    void GetInitialMatrix( transform44& m );

    /// Get the initial position
    void GetInitialPosition( vector3& m );

    /// Get object's current world transform
    virtual void GetTransform( transform44& t );

    /// Set a transformation to be combined with the original matrix, and apply to the object
    virtual void SetTransform( transform44& t );

    /// Set entity object
    void SetEntity( nEntityObject *entity );

    /// Get entity object
    nEntityObject* GetEntity();

    /// Set reference point
    void SetReferencePoint( vector3 ref );

    /// Set outdoor for collision heightmap and collision type
    void SetOutdoor( nEntityObject * outdoor, TerrainCollisionType colType );

    /// Set containing indoor entity
    void SetContainingIndoor( nEntityObject* indoor );

    /// Get containing indoor entity
    nEntityObject* GetContainingIndoor();

protected:

    /// Reference to entity being transformed
    nRefEntityObject refEntity;

    /// Initial matrix 
    transform44 initialMatrix;

    /// Point of reference for transformation
    vector3 refPoint;

    /// Collision heightmap
    nFloatMap* collisionHeightmap;

    /// Indoor entity containing this entity, or NULL
    nEntityObject* indoor;

    /// Terrain collision type
    TerrainCollisionType terrainCollisionType;

private:
};

//------------------------------------------------------------------------------

#endif
