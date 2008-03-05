#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nswitchNode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nswitchnode.h"
#include "nscene/nscenegraph.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nSwitchNode, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nSwitchNode::nSwitchNode() :
    channelVarHandle(nVariable::InvalidHandle)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSwitchNode::~nSwitchNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Sets the "animation channel" which drives this animation.
    This could be something like "time", but the actual names are totally
    up to the application. The actual channel value will be pulled from
    the render context provided in the Animate() method.
*/
void
nSwitchNode::SetChannel(const char *name)
{
    n_assert(name);
    this->channelVarHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
}

//------------------------------------------------------------------------------
/**
    Return the animation channel which drives this animation.
*/
const char *
nSwitchNode::GetChannel()
{
    if (nVariable::InvalidHandle == this->channelVarHandle)
    {
        return 0;
    }
    else
    {
        return nVariableServer::Instance()->GetVariableName(this->channelVarHandle);
    }
}

//------------------------------------------------------------------------------
/**
    Set specific child node for given index.
*/
void
nSwitchNode::SetIndexNode(int index, const char *name)
{
    n_assert(name);

    for (int i = 0; i < this->indexNodes.Size(); i++)
    {
        if (this->indexNodes[i].childIndex == index)
        {
            this->indexNodes[i].nodeName = name;
            return;
        }
    }

    IndexNode newIndexNode;
    newIndexNode.childIndex = index;
    newIndexNode.nodeName = name;
    newIndexNode.refChild.invalidate();
    this->indexNodes.Append(newIndexNode);
}

//------------------------------------------------------------------------------
/**
    Get specific child node for given index.
*/
const char *
nSwitchNode::GetIndexNode(int index)
{
    for (int i = 0; i < this->indexNodes.Size(); i++)
    {
        if (this->indexNodes[i].childIndex == index)
        {
            return this->indexNodes[i].nodeName.Get();
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Attach selected LOD to scene graph.
*/
void
nSwitchNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);
    n_assert(this->channelVarHandle != nVariable::InvalidHandle);
    ncDictionary *varContext = entityObject->GetComponent<ncDictionary>();
    
    // get the sample time from the render context
    nVariable* var = varContext->GetVariable(this->channelVarHandle);
    int childIndex;
    if (var == 0)
    {
        //n_error("nSwitchNode::Attach: channel Variable '%s' not found in render context!\n", 
        //        nVariableServer::Instance()->GetVariableName(this->channelVarHandle));
        childIndex = 0;
    }
    else
    {
        childIndex = var->GetInt();
    }
    
    // if there is a explicit index, give it priority
    for (int i = 0; i < this->indexNodes.Size(); i++)
    {
        if (this->indexNodes[i].childIndex == childIndex)
        {
            if (!this->indexNodes[i].refChild.isvalid())
            {
                this->indexNodes[i].refChild = (nSceneNode*) this->Find(this->indexNodes[i].nodeName.Get());
                n_assert(this->indexNodes[i].refChild.isvalid());
            }
            this->indexNodes[i].refChild->Attach(sceneGraph, entityObject);
            return;
        }
    }

    int index;
    nSceneNode *curChild;
    nSceneNode *childToAttach = (nSceneNode*) this->GetHead();
    for (curChild = (nSceneNode*) this->GetHead(), index = 0;
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc(), index++)
    {
        childToAttach = curChild;
        if (index == childIndex)
        {
            break;
        }
    }
    childToAttach->Attach(sceneGraph, entityObject);
}
