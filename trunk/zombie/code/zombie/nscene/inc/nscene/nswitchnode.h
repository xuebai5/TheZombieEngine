#ifndef N_SWITCHNODE_H
#define N_SWITCHNODE_H
//------------------------------------------------------------------------------
/**
    @class nSwitchNode
    @ingroup SceneNodes
    
    @brief A node that selects one of its children at Attach time
    depending on a selector value passed in the specified variable
    in the render context.
    
    (C) 2004 Conjurer Services, S.A.
*/
#include "nscene/nscenenode.h"
#include "variable/nvariable.h"
#include "kernel/nref.h"

//------------------------------------------------------------------------------
class nSwitchNode : public nSceneNode
{
public:
    /// constructor
    nSwitchNode();
    /// destructor
    virtual ~nSwitchNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// called by nSceneServer when object is attached to scene
    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    
    /// set the variable handle which drives the switch
    void SetChannel(const char* name);
    /// get the variable which drives the switch
    const char *GetChannel();
    /// explicitly set the node for a given index
    void SetIndexNode(int index, const char *name);
    /// get the explicit node for a given index
    const char *GetIndexNode(int index);

protected:
    struct IndexNode
    {
        int childIndex;
        nString nodeName;
        nRef<nSceneNode> refChild;
    };
    nArray<IndexNode> indexNodes;
    nVariable::Handle channelVarHandle;
};

//------------------------------------------------------------------------------
#endif
