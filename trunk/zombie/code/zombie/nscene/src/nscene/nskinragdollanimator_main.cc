#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nskinragdollanimator_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nskinragdollanimator.h"
#include "nscene/nskingeometrynode.h"
#include "nscene/nragdollcharanimator.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "zombieentity/ncdictionary.h"

#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"

#include "nphysics/ncphyhumragdoll.h"
#include "nphysics/ncphyragdoll.h"
#include "nphysics/nphysicsjoint.h"
#include "nphysics/nphyhingejoint.h"
#include "nphysics/nphyuniversaljoint.h"

nNebulaScriptClass(nSkinRagdollAnimator, "nskingeometryanimator");

//------------------------------------------------------------------------------
/**
*/
nSkinRagdollAnimator::nSkinRagdollAnimator() :
    charAnimVarIndex(-1),
    refScriptServer("/sys/servers/script"),
    ragdollJoints(10)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSkinRagdollAnimator::~nSkinRagdollAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinRagdollAnimator::EntityCreated(nEntityObject* entityObject)
{
    nSkinGeometryAnimator::EntityCreated(entityObject);
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    ncDictionary *varContext = entityObject->GetComponent<ncDictionary>();

    nVariable::Handle varHandle;
    varHandle = nVariableServer::Instance()->GetVariableHandleByName("ragdoll");
    const char *name = varContext->GetVariable(varHandle)->GetString();
    varHandle = nVariableServer::Instance()->GetVariableHandleByName("ragdollpos");
    vector4 pos = varContext->GetVariable(varHandle)->GetVector4();

    nString command("makeragdoll('");
    command += name;
    command += "', ";
    command += pos.x;
    command += ", ";
    command += pos.y;
    command += ", ";
    command += pos.z;
    command += ")";
    nString scriptResult;
    this->refScriptServer->Run(command.Get(), scriptResult);
    
    nString path("/usr/physics/world/ragdoll");
    path += name;
    ncPhyHumRagDoll *phyHumRagdoll = (ncPhyHumRagDoll *) kernelServer->Lookup(path.Get());
    n_assert(phyHumRagdoll);
    
    nRagdollCharAnimator *charAnimator = n_new(nRagdollCharAnimator);
    //charAnimator->SetPhyRagdoll(phyHumRagdoll); WON'T WORK!!!! FIXME ANTONIA
    charAnimator->SetRagdollJoints(&this->ragdollJoints);
    this->charAnimVarIndex = renderContext->AddLocalVar(nVariable(0, charAnimator));
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinRagdollAnimator::EntityDestroyed(nEntityObject* entityObject)
{
    ncScene *renderContext = entityObject->GetComponent<ncScene>();

    nVariable var = renderContext->GetLocalVar(this->charAnimVarIndex);
    nCharacterAnimator* charAnimator = (nCharacterAnimator*) var.GetObj();
    n_assert(charAnimator);

    n_delete(charAnimator);
    nSkinGeometryAnimator::EntityDestroyed(entityObject);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinRagdollAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);

    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    nVariableContext& varContext = entityObject->GetComponent<ncDictionary>()->VarContext();
    
    // update the source node with the new char skeleton state
    static nClass *nSkinGeometryNodeClass = nKernelServer::Instance()->FindClass("nskingeometrynode");
    nSkinGeometryNode *skinGeometryNode = 0;
    n_assert2(sceneNode->IsA(nSkinGeometryNodeClass), "nSkinRagdollAnimator::Animate: can't cast sceneNode!\n");
    skinGeometryNode = (nSkinGeometryNode *) sceneNode;
    
    // retrieve the character for this instance from the render context
    nVariable& characterVar = renderContext->GetLocalVar(this->characterVarIndex);
    nCharacter2* curCharacter = (nCharacter2*) characterVar.GetObj();
    n_assert(curCharacter);
    
    // check if I am already uptodate for this frame
    nVariable& frameIdVar = renderContext->GetLocalVar(this->frameIdVarIndex);
    uint frameId = frameIdVar.GetInt();
    uint curFrameId = renderContext->GetFrameId();
    if (frameId != curFrameId)
    {
        frameIdVar.SetInt(curFrameId);
        
        // ... perform required voodoo here ...
        nVariable var = renderContext->GetLocalVar(this->charAnimVarIndex);
        nCharacterAnimator* charAnimator = (nCharacterAnimator*) var.GetObj();
        n_assert(charAnimator);

        // theoretically the character skeleton should be already computed in here
        int numJoints = curCharacter->GetSkeleton().GetNumJoints();
        int jointIndex;

        if (charAnimator->Animate(curCharacter, &varContext))
        {
            for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
            {
                nCharJoint& joint = curCharacter->GetSkeleton().GetJointAt(jointIndex);
                //joint.Evaluate();
                joint.EvaluateWorldCoord();
            }
        }
        else
        {
            for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
            {
                nCharJoint& joint = curCharacter->GetSkeleton().GetJointAt(jointIndex);
                joint.SetTranslate(joint.GetPoseTranslate());
                joint.SetRotate(joint.GetPoseRotate());
                joint.SetScale(joint.GetPoseScale());
                joint.EvaluateWorldCoord();
                //joint.Evaluate();
            }
        }
    }

    // update the target node
    //skinGeometryNode->SetCharSkeleton(&curCharacter->GetSkeleton());
}
