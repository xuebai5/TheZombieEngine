#ifndef N_INGUI_OBJECT_TRANSFORM_GROUP_H
#define N_INGUI_OBJECT_TRANSFORM_GROUP_H
//------------------------------------------------------------------------------
/**
    @file inguiobjecttransformgroup.h
    @class InguiObjectTransformGroup
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Simple subclass for testing transform tools

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "conjurer/inguiobjecttransform.h"
#include "mathlib/transform44.h"

class nGfxObject;
//------------------------------------------------------------------------------
class InguiObjectTransformGroup: public InguiObjectTransform
{
public:

    InguiObjectTransformGroup();
    ~InguiObjectTransformGroup();

    /// Set the objects array to be transformed
    void SetSelection(nArray<InguiObjectTransform>* set);

    /// Begin an operation and set the initial matrix
    virtual void Begin();

    /// End the operation
    virtual void End();

    /// Cancel the operation
    virtual void Cancel();

    /// Get current world transform
    virtual void GetTransform( transform44& t );

    /// Set a group transformation
    virtual void SetTransform( transform44& t );

    /// Get selection centroid position
    void GetCenterPosition( vector3& v );

protected:

    // Matrix representing current group transform
    transform44 groupMatrix;

    // Array of objects
    nArray<InguiObjectTransform>* objects;

};
//------------------------------------------------------------------------------
#endif

