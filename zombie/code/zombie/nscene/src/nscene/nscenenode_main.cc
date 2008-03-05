#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nscenenode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nscenenode.h"
#include "nscene/nanimator.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nSceneNode, "nroot");

//------------------------------------------------------------------------------
/**
*/
nSceneNode::nSceneNode() :
    animatorArray(0, 4),
    resourcesValid(false),
    renderPri(0),
    hints(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode::~nSceneNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This calls UnloadResources() if the object is going to die (this can't
    be put into the destructor, because virtual methods don't work when
    called from the destructor).
*/
bool
nSceneNode::Release()
{
    if (1 == this->refCount)
    {
        this->UnloadResources();
    }
    return nRoot::Release();
}

//------------------------------------------------------------------------------
/**
    This method makes sure that all resources needed by this object
    are loaded. The method does NOT recurse into its children.

    Subclasses should expect that the LoadResources() method can be 
    called on them although some or all of their resources are valid.
    Thus, a check should exist, whether the resource really needs to
    be reloaded.

    @return     true, if resource loading was successful
*/
bool
nSceneNode::LoadResources()
{
#ifdef _DEBUG
    // char buf[N_MAXPATH];
    // n_printf("-> Loading resources for scene node '%s'\n", this->GetFullName(buf, sizeof(buf)));
#endif
    // load animator resource
    int numAnimators = this->GetNumAnimators();
    if (numAnimators > 0)
    {
        kernelServer->PushCwd(this);
        int i;
        for (i = 0; i < numAnimators; i++)
        {
            nAnimator* animator = this->animatorArray[i].get();
            if (!animator->AreResourcesValid())
            {
                animator->LoadResources();
            }
        }
        kernelServer->PopCwd();
    }

    this->resourcesValid = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    This method makes sure that all resources used by this object are
    unloaded. The method does NOT recurse into its children. 

    If you ovverride this method, be sure to call the overridden version
    in your destructor.

    @return     true, if resources have actually been unloaded
*/
void
nSceneNode::UnloadResources()
{
#ifdef _DEBUG
    // char buf[N_MAXPATH];
    // n_printf("-> Unloading resources for scene node '%s'\n", this->GetFullName(buf, sizeof(buf)));
#endif
    /*
    if (this->refInstanceStream.isvalid())
    {
        this->refInstanceStream->Release();
        n_assert(!this->refInstanceStream.isvalid());
    }
    */
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
    Recursively preload required resources. Call this method after loading
    or creation and before the first rendering. It will load all required
    resources (textures, meshes, animations, ...) from disk and thus
    prevent stuttering during rendering.
*/
void
nSceneNode::PreloadResources()
{
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    // recurse...
    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        curChild->PreloadResources();
    }
}

//------------------------------------------------------------------------------
/**
    Called by the client app when a new render context has been created for
    this scene node hierarchy. Scene node hierarchies must not contain
    frame-persistent data, since one hierarchy can be reused multiple times
    per frame. All frame-persistent data must be stored in nRenderContext 
    objects, which must be communicated to the scene node hierarchy when
    it is rendered. nRenderContext objects are the primary way to
    communicate data from the client app to a scene node hierarchy (i.e.
    time stamps, velocity, etc...).

    The EntityCreated() method should be called when a new
    'game object' which needs rendering has been created by the application.

    @param  renderContext   pointer to a nRenderContext object    
*/
void
nSceneNode::EntityCreated(nEntityObject* entityObject)
{
    n_assert(entityObject);

    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        curChild->EntityCreated(entityObject);
    }
}

//------------------------------------------------------------------------------
/**
    Called by the client app when a render context for this scene node
    hierarchy should be destroyed. This is usually the case when the
    game object associated with this scene node hierarchy goes away.

    The method will be invoked recursively on all child and depend nodes
    of the scene node object.

    @param  renderContext   pointer to a nRenderContext object    

    - 20-Jul-04     floh    oops, recursive routine was calling ClearLocalVars!
*/
void
nSceneNode::EntityDestroyed(nEntityObject* entityObject)
{
    n_assert(entityObject);

    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        curChild->EntityDestroyed(entityObject);
    }
}

//------------------------------------------------------------------------------
/**
    Attach the object to the scene if necessary. This method is either
    called by the nSceneGraph, or by another nSceneNode object at 
    scene construction time. If the nSceneNode needs rendering it should
    call the appropriate nSceneGraph method to attach itself to the scene.

    The method will be invoked recursively on all child and depend nodes
    of the scene node object.

    @param  sceneGraph      pointer to the nSceneGraph object
    @param  renderContext   pointer to the nRenderContext object
*/
void
nSceneNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    #if __NEBULA_STATS__
    //sceneGraph->profAttachSubnodes.StartAccum();
    #endif

    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        #if __NEBULA_STATS__
        //sceneGraph->profAttachSubnodes.StopAccum();
        #endif

        curChild->Attach(sceneGraph, entityObject);

        #if __NEBULA_STATS__
        //sceneGraph->profAttachSubnodes.StartAccum();
        #endif
    }

    #if __NEBULA_STATS__
    //sceneGraph->profAttachSubnodes.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    Render the node's operation. This should be implemented by a subclass.
    The method will only be called by nSceneGraph at the proper render pass.
*/
bool
nSceneNode::Render(nSceneGraph* /*sceneGraph*/, nEntityObject* /*entityObject*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    get number of render passes
*/
int
nSceneNode::GetNumPasses()
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    get render pass at index
*/
nFourCC
nSceneNode::GetPassAt(int /*index*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    get pass index at index
*/
int
nSceneNode::GetPassIndexAt(int /*index*/)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    Add an animator object to this scene node.
*/
void
nSceneNode::AddAnimator(const char* relPath)
{
    n_assert(relPath);

    nDynAutoRef<nAnimator> newDynAutoRef;
    newDynAutoRef.set(relPath);
    this->animatorArray.Append(newDynAutoRef);
}

//------------------------------------------------------------------------------
/**
    Remove an animator object from this scene node.
*/
void
nSceneNode::RemoveAnimator(const char* relPath)
{
    n_assert(relPath);

    const int numAnimators = this->GetNumAnimators();
    int i;
    for (i = 0; i < numAnimators; i++)
    {
        if (!strcmp( relPath, this->animatorArray[i]->GetName()))
        {
            this->animatorArray[i].set(0); //unset/clear the nDynAutoRef
            this->animatorArray.Erase(i);  //delete the nDynAutoRef from array
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Get number of animator objects.
*/
int
nSceneNode::GetNumAnimators() const
{
    return this->animatorArray.Size();
}

//------------------------------------------------------------------------------
/**
    Get path to animator object at given index.
*/
const char*
nSceneNode::GetAnimatorAt(int index)
{
    return this->animatorArray[index].getname();
}

//------------------------------------------------------------------------------
/**
    Invoke all animators. This method should be called by classes 
    implementing the Render() method from inside this method.
*/
void
nSceneNode::InvokeAnimators(nEntityObject* entityObject)
{
    int numAnimators = this->GetNumAnimators();
    if (numAnimators > 0)
    {
        n_assert(entityObject);

        kernelServer->PushCwd(this);
        int i;
        for (i = 0; i < numAnimators; i++)
        {
            nAnimator* animator = this->animatorArray[i].get();
            animator->Animate(this, entityObject);
        }
        kernelServer->PopCwd();
    }
}

//------------------------------------------------------------------------------
/**
    Returns a valid instance stream object for this scene node hierarchy.
    If no instance stream object exists yet, it will be created and stored.
    The instance stream declaration will be built from all shaders in the
    hierarchy by recursively calling UpdateInstStreamDecl().
*/
/*
nInstanceStream*
nSceneNode::GetInstanceStream()
{
    if (!this->refInstanceStream.isvalid())
    {
        nInstanceStream* instStream = this->refGfxServer->NewInstanceStream(0);
        n_assert(instStream);
        n_assert(!instStream->IsValid());

        // build an instance stream declaration from the hierarchy
        nInstanceStream::Declaration decl;
        this->UpdateInstStreamDecl(decl);

        instStream->SetDeclaration(decl);
        bool success = instStream->Load();
        n_assert(success);
        this->refInstanceStream = instStream;
    }
    return this->refInstanceStream.get();
}
*/

//------------------------------------------------------------------------------
/**
    Recursively build an instance stream declaration from the shaders in
    the scene node hierarchy. Override this method in subclasses with 
    shader handling.
*/
void
nSceneNode::UpdateInstStreamDecl(nInstanceStream::Declaration& decl)
{
    nSceneNode* curChild;
    for (curChild = (nSceneNode*) this->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        curChild->UpdateInstStreamDecl(decl);
    }
}


#ifndef NGAME

//------------------------------------------------------------------------------
/**
*/
nObject*
nSceneNode::GetFirstAnimator() const
{
    if ( this->GetNumAnimators() > 0  )
    {
        return this->animatorArray[0];
    } else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneNode::SetFirstAnimator(nObject* /*animator*/) const
{
    // Empty, is only for property grid, is only read
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneNode::BindDirtyDependence(nObject* receiver)
{
    nRoot::BindDirtyDependence(receiver);
    for ( int idx=0; idx < this->animatorArray.Size() ; ++idx )
    {
        if ( this->animatorArray[idx].isvalid())
        {
            this->animatorArray[idx]->BindDirtyDependence(this);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSceneNode::RecruseSetObjectDirty(bool dirty)
{
    nRoot::RecruseSetObjectDirty(dirty);
    for ( int idx=0; idx < this->animatorArray.Size() ; ++idx )
    {
        if ( this->animatorArray[idx].isvalid())
        {
            this->animatorArray[idx]->RecruseSetObjectDirty(dirty);
        }
    }
}


#endif //!NGAME