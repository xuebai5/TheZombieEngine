#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nattachmentnode_main.cc
//  (C) 2004 Megan Fox
//------------------------------------------------------------------------------
#include "nscene/nattachmentnode.h"
#include "nscene/nscenegraph.h"
#include "animcomp/nccharacter.h"
#include "nanimation/nanimationserver.h"
#include "gfx2/ngfxserver2.h"


nNebulaScriptClass(nAttachmentNode, "ntransformnode");

//------------------------------------------------------------------------------
/**
*/
nAttachmentNode::nAttachmentNode() :
    isFinalDirty(false),
    isJointSet(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAttachmentNode::~nAttachmentNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.
*/
bool
nAttachmentNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);
    
    nCharacter2* curCharacter;

    ncCharacter* charComp = entityObject->GetComponent<ncCharacter>();

    if (charComp)
    {
        nTime curTime = entityObject->GetComponent<ncScene>()->GetTime();
        curCharacter = charComp->UpdateAndGetCharacter(sceneGraph->GetCurrentLod(), curTime);
        n_assert(curCharacter);

        this->InvokeAnimators(entityObject);
        this->UpdateFinalTransform(curCharacter->GetSkeleton());
        
        if (this->GetLockViewer())
        {
            // if lock viewer active, copy viewer position
            const matrix44& viewMatrix = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
            matrix44 m = this->finalMatrix;
            m.M41 = viewMatrix.M41;
            m.M42 = viewMatrix.M42;
            m.M43 = viewMatrix.M43;
            sceneGraph->SetModelTransform(m * sceneGraph->GetModelTransform());
        }
        else
        {
            // default case
            sceneGraph->SetModelTransform(this->finalMatrix * sceneGraph->GetModelTransform());
        }
        
        this->isFinalDirty = true;
        
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Compute the final transformation matrix for the nAttachmentNode
*/
void
nAttachmentNode::UpdateFinalTransform(nCharSkeleton& skeleton)
{
    // A joint must be set for this to do anything at all.  If no joint is set, default
    // to the normal transform value (act as an nTransformNode).
    if (!this->isJointSet)
    {
        this->finalMatrix = this->tform.getmatrix();
        return;
    }
    
    if (this->isFinalDirty)
    {
        // Multiply the local matrix by the joint matrix
        // (which we get through lengthy means via the parent nSkinShapeNode)
        const matrix44& jointMatrix = skeleton.GetJointAt(this->jointIndex).GetMatrix();
        this->finalMatrix = this->tform.getmatrix() * jointMatrix;
        
        this->isFinalDirty = false;
    }
}

//------------------------------------------------------------------------------
/**
    Specifies the target joint by name
    
    @param jointName  the name of the joint index to target
*/
void
nAttachmentNode::SetJointByName(const char *jointName)
{
    if (strcmp(this->parent->GetClass()->GetName(), "nskingeometrynode"))
    {
        n_printf("Error: nAttachmentNode can only function if it is parented by an nSkinShapeNode\n");
        return;
    }
    kernelServer->PushCwd(this->parent);
    nSkinGeometryAnimator *skinGeomAnimator = (nSkinGeometryAnimator *)this->kernelServer->Lookup(((nSkinGeometryNode*)this->parent)->GetAnimatorAt(0));//ATTENTION with at 0!!!
    n_assert(skinGeomAnimator);

    kernelServer->PopCwd();

    unsigned int newIndex = skinGeomAnimator->GetJointByName(jointName);

    if (newIndex != nSkinGeometryAnimator::InvalidIndex)
    {
        this->jointIndex = newIndex;
        this->isJointSet = true;
    }
    else
    {
        n_printf("Error: Unable to find joint of name '%s' on parent nSkinGeometryAnimator\n", jointName);
    }
}

//------------------------------------------------------------------------------
/**
    Specifies the target joint by joint index

    @param newIndex  the joint index to target
*/
void
nAttachmentNode::SetJointByIndex(unsigned int newIndex)
{
    if (strcmp(this->parent->GetClass()->GetName(), "nskingeometrynode"))
    {
        n_printf("Error: nAttachmentNode can only function if it is parented by an nSkinGeometryNode\n");
        return;
    }

    this->jointIndex = newIndex;
    this->isJointSet = true;
}
