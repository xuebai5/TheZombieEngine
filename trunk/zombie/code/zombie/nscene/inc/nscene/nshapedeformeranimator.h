#ifndef N_SHAPEDEFORMERANIMATOR_H
#define N_SHAPEDEFORMERANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nShapeDeformerAnimator
    @ingroup SceneAnimators
 
    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/nanimator.h"

//------------------------------------------------------------------------------
class nShapeDeformerAnimator : public nAnimator
{
public:
    /// constructor
    nShapeDeformerAnimator();
    /// destructor
    virtual ~nShapeDeformerAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);

    /// set deformation grade
    void SetDeformationGrade(float defGrade);
    /// get deformation grade
    const float GetDeformationGrade() const;

    /// return the type of this animator object
    virtual Type GetAnimatorType() const;

    /// set deformation channel
    void SetDeformationChannel(const char* defChannel);
    /// get deformation channel
    const char* GetDeformationChannel() const;

    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

private:
    // [0-1]
    float deformationGrade;
    nVariable::Handle deformationGradeHandle;
};

//------------------------------------------------------------------------------
#endif

