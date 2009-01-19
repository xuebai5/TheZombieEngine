#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nskingeometrynode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nskingeometrynode.h"
#include "nscene/nscenegraph.h"
#include "anim2/nanimationserver.h"

#include "entity/nentity.h"
#include "animcomp/nccharacter.h"

nNebulaScriptClass(nSkinGeometryNode, "ngeometrynode");

//------------------------------------------------------------------------------
/**
*/
nSkinGeometryNode::nSkinGeometryNode() :
    animStateVarHandle(nVariable::InvalidHandle),
    skeletonLevel(-1)
{
    //this->SetMeshUsage(nMesh2::WriteOnce | nMesh2::NeedsVertexShader);
}

//------------------------------------------------------------------------------
/**
*/
nSkinGeometryNode::~nSkinGeometryNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryNode::Initialize()
{
    n_assert(this->parent->IsA(kernelServer->FindClass("ngeometrynode")));
    this->refParentNode = (nGeometryNode *) this->parent;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nSkinGeometryNode::LoadResources()
{
    if (this->refParentNode->AreResourcesValid() || this->refParentNode->LoadResources())
    {
        return nGeometryNode::LoadResources();
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nSkinGeometryNode::UnloadResources()
{
    nGeometryNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Begin defining the joint palette of a skin fragment.
*/
void
nSkinGeometryNode::BeginJointPalette(int numJoints)
{
    this->GetJointPalette().BeginJoints(numJoints);
}

//------------------------------------------------------------------------------
/**
    Add up to 8 joints to a fragments joint palette starting at a given
    palette index.
*/
void
nSkinGeometryNode::SetJointIndices(int paletteIndex, int ji0, int ji1, int ji2, int ji3, int ji4, int ji5, int ji6, int ji7)
{
    nCharJointPalette& pal = this->GetJointPalette();
    int numJoints = pal.GetNumJoints();
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji0);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji1);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji2);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji3);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji4);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji5);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji6);
    if (paletteIndex < numJoints) pal.SetJointIndex(paletteIndex++, ji7);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryNode::SetJointIndex(int paletteIndex, int jointIndex)
{
    nCharJointPalette& pal = this->GetJointPalette();
    pal.SetJointIndex(paletteIndex, jointIndex);
}

//------------------------------------------------------------------------------
/**
    Finish defining the joint palette of a skin fragment.
*/
void
nSkinGeometryNode::EndJointPalette()
{
    this->GetJointPalette().EndJoints();
}

//------------------------------------------------------------------------------
/**
    Get joint palette size of a skin fragment.
*/
int
nSkinGeometryNode::GetJointPaletteSize()
{
    return this->GetJointPalette().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
    Get a joint index from a fragment's joint index.
*/
int
nSkinGeometryNode::GetJointIndex(int paletteIndex)
{
    return this->GetJointPalette().GetJointIndexAt(paletteIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryNode::SetStateChannel(const char* name)
{
    n_assert(name);
    this->animStateVarHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
}

//------------------------------------------------------------------------------
/**
    Returns the name of the animation variable which drives this animation. 
    This variable exists within the variable server located at 
    @c /sys/servers/variable.
*/
const char*
nSkinGeometryNode::GetStateChannel()
{
    if (nVariable::InvalidHandle == this->animStateVarHandle)
    {
        return 0;
    }
    else
    {
        return nVariableServer::Instance()->GetVariableName(this->animStateVarHandle);
    }
}

//------------------------------------------------------------------------------
/**
*/
nVariable::Handle
nSkinGeometryNode::GetAnimStateVarHandle()
{
    return this->animStateVarHandle;
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryNode::SetSkeletonLevel(int level)
{
    this->skeletonLevel = level;
}

//------------------------------------------------------------------------------
/**
*/
int
nSkinGeometryNode::GetSkeletonLevel()
{
    return this->skeletonLevel;
}

//------------------------------------------------------------------------------
/**
    Call my skin animator.
*/
void
nSkinGeometryNode::InvokeAnimators(nEntityObject* entityObject)
{
    nGeometryNode::InvokeAnimators(entityObject);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSkinGeometryNode::Apply(nSceneGraph* sceneGraph)
{
    if (nGeometryNode::Apply(sceneGraph))
    {
        return this->refParentNode->Apply(sceneGraph);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Computes and assign the joint palette then reuse the implementation
    for actual geometry rendering in the parent node.
*/
bool
nSkinGeometryNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);
    
    // this animates and renders the surfacea
    nGeometryNode::Render(sceneGraph, entityObject);

    #ifndef NGAME
    if (sceneGraph->GetShaderIndex() == this->wireframeShaderIndex)
    {
        nShader2* curShader = nGfxServer2::Instance()->GetShader();
        if (curShader->IsParameterUsed(nShaderState::isSkinned))
        {
            curShader->SetInt(nShaderState::isSkinned, 1);//true
        }
    }
    #endif

    // get character data
    nCharacter2* curCharacter = 0;
    ncCharacter* charComp = entityObject->GetComponent<ncCharacter>();
    if (charComp)
    {
        nTime curTime = entityObject->GetComponent<ncScene>()->GetTime();

        /// @todo ma.garcias -
        /// in case the level for the skeleton wasn't assigned when exported,
        /// it is fixed properly in ncAssetClass resource loading
        if (this->GetSkeletonLevel() != -1)
        {
            curCharacter = charComp->UpdateAndGetCharacter(this->GetSkeletonLevel(), curTime);
        }
        else
        {
            curCharacter = charComp->UpdateAndGetCharacter(sceneGraph->GetCurrentLod(), curTime);
        }
        
        n_assert(curCharacter);
        
        nShader2 *shader = nGfxServer2::Instance()->GetShader();
        n_assert(shader);
        
        // the following code comes from nCharSkinRenderer
        static const int maxJointPaletteSize = 72;
        static matrix44 jointArray[maxJointPaletteSize];
        
        // extract the current joint palette from the skeleton in the
        // right format for the skinning shader
        int paletteSize = this->jointPalette.GetNumJoints();
        n_assert(paletteSize <= maxJointPaletteSize);
        int paletteIndex;
        for (paletteIndex = 0; paletteIndex < paletteSize; paletteIndex++)
        {
            const nCharJoint& joint = curCharacter->GetSkeleton().GetJointAt(this->jointPalette.GetJointIndexAt(paletteIndex));
            jointArray[paletteIndex] = joint.GetSkinMatrix44();
        }
        
        // transfer the joint palette to the current shader
        if (shader->IsParameterUsed(nShaderState::JointPalette))
        {
            shader->SetMatrixArray(nShaderState::JointPalette, jointArray, paletteSize);
        }
        
        // finally, render the geometry
        return this->refParentNode->Render(sceneGraph, entityObject);
    }
    return false;
}
