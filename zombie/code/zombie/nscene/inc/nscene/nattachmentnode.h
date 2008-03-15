#ifndef N_ATTACHMENTNODE_H
#define N_ATTACHMENTNODE_H
//------------------------------------------------------------------------------
/**
    @class nAttachmentNode
    @ingroup Scene

    @brief The purpose of nAttachmentNode is to provide an external point of access
    to a skeleton's joint transformation information.  The user specifies either a
    joint name or joint index, and the nAttachmentNode grabs the specified joint data,
    its transformation mirroring the joint's.

    The transformations performed on the nAttachmentNode itself are relative to the
    target bone's origin.  Calling GetTransform on nAttachmentNode will get you the
    world space matrix of the bone origin offset by the specified transformations.

    Please note that nAttachmentNode assumes its parent is the target nSkinShapeNode.

    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/nscenenode.h"
#include "nscene/ntransformnode.h"
#include "nscene/nskingeometryanimator.h"
#include "nscene/nskingeometrynode.h"
#include "ncharacter/ncharskeleton.h"
#include "ncharacter/ncharacter2.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "mathlib/transform44.h"

//------------------------------------------------------------------------------
class nAttachmentNode : public nTransformNode
{
public:
    /// constructor
    nAttachmentNode();
    /// destructor
    virtual ~nAttachmentNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// update transform and render into scene server
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    
    /// updates the final transformation, combing local matrix with joint matrix data if necessary
    virtual void UpdateFinalTransform(nCharSkeleton& skeleton);

    /// specifies the target joint by name
    virtual void SetJointByName(const char *jointName);
    /// specifies the target joint by index
    virtual void SetJointByIndex(unsigned int newIndex);

protected:
    /// get parent skin shape node
    nSkinGeometryNode *GetParentSkinGeometryShape();

    matrix44 finalMatrix;
    bool isFinalDirty;
    bool isJointSet;
    int jointIndex;
};

//------------------------------------------------------------------------------
/**
    Get the final transformation matrix that represents 
    the bone's position in object space

    @return the transformation matrix requested
*/
inline
nSkinGeometryNode *
nAttachmentNode::GetParentSkinGeometryShape()
{
    n_assert(this->parent->GetClass() == kernelServer->FindClass("nskingeometrynode"));
    return (nSkinGeometryNode *) this->parent;
}

//------------------------------------------------------------------------------
#endif
