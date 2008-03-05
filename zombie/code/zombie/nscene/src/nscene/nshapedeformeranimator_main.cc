#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshapedeformeranimator_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nshapedeformeranimator.h"
#include "nscene/nblendshapenode.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nShapeDeformerAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nShapeDeformerAnimator::nShapeDeformerAnimator() :
    deformationGrade(0.0f),
    deformationGradeHandle(nVariable::InvalidHandle)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nShapeDeformerAnimator::~nShapeDeformerAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAnimator::Type
nShapeDeformerAnimator::GetAnimatorType() const
{
    return nAnimator::Deformer;
}

//------------------------------------------------------------------------------
/**
    This does the actual work of manipulate the target object.

    @param  sceneNode       object to manipulate (must be of class nBlendShapeNode)
    @param  renderContext   current render context
*/
void
nShapeDeformerAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);

    nVariableContext *varContext = &entityObject->GetComponent<ncDictionary>()->VarContext();
    nVariable* deformGradeVar = varContext->GetVariable(deformationGradeHandle);
    
    if( deformGradeVar )
    {
        this->deformationGrade = deformGradeVar->GetFloat();
    }
    
    n_assert(sceneNode->GetClass() == kernelServer->FindClass("nblendshapenode"));
    nBlendShapeNode* targetNode = (nBlendShapeNode*) sceneNode;
    
    // set weights, the first one always zero
    // the one that deforms, get deformation grade
    targetNode->SetWeightAt(0, 0.0f);
    targetNode->SetWeightAt(1, this->deformationGrade);
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeDeformerAnimator::SetDeformationGrade( float defGrade)
{
    // if range 0-100 -> move to 0-1
    if( defGrade > 1 )
    {
        defGrade = static_cast<float>(defGrade * 0.01);
    }

    this->deformationGrade = defGrade;
}

//------------------------------------------------------------------------------
/**
*/
const float
nShapeDeformerAnimator::GetDeformationGrade( ) const
{
    return this->deformationGrade;
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeDeformerAnimator::SetDeformationChannel( const char* defChannel)
{
    deformationGradeHandle = nVariableServer::Instance()->GetVariableHandleByName( defChannel );
}

//------------------------------------------------------------------------------
/**
*/
const char *
nShapeDeformerAnimator::GetDeformationChannel( ) const
{
    return nVariableServer::Instance()->GetVariableName(this->deformationGradeHandle);
}


//------------------------------------------------------------------------------
