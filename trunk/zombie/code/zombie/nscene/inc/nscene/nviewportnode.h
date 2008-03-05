#ifndef N_VIEWPORTNODE_H
#define N_VIEWPORTNODE_H
//------------------------------------------------------------------------------
/**
    @class nViewportNode
    @ingroup SceneNodes

    @brief A viewport node is responsible for beginning and ending
    render operation, triggering render of the rest of the scene.

    See also @ref N2ScriptInterface_nviewportnode

    (C) 2002 RadonLabs GmbH
*/

#include "nscene/nrenderpathnode.h"
#include "variable/nvariable.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"

class nConServer;
class nGuiServer;
//------------------------------------------------------------------------------
class nViewportNode : public nRenderPathNode
{
public:
    /// constructor
    nViewportNode();
    /// destructor
    virtual ~nViewportNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);
    /// attach object to scene graph
    virtual void Attach(nSceneGraph *sceneGraph, nEntityObject *entityObject);
    /// render object for a given pass
    virtual bool Render(nSceneGraph *sceneGraph, nEntityObject *entityObject);

    /// set background color
    void SetBgColor(const vector4& c);
    /// get background color
    const vector4& GetBgColor() const;
    /// override other attached viewport at render
    void SetOverride(bool override);
    /// override other attached viewport at render
    bool GetOverride() const;

    /// invalidate any other viewport(s) in the view pass
    void ApplyOverride(nSceneGraph*, nEntityObject*);
    /// restore any other viewport(s) in the view pass
    void RenderOverride(nSceneGraph*, nEntityObject*);

protected:
    vector4 bgColor;
    int viewPassIndex;
    int rtgtPassIndex;
    bool override;
    int overridenVarIndex;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewportNode::SetBgColor(const vector4& c)
{
    this->bgColor = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nViewportNode::GetBgColor() const
{
    return this->bgColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nViewportNode::SetOverride(bool b)
{
    this->override = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nViewportNode::GetOverride() const
{
    return this->override;
}

//------------------------------------------------------------------------------
#endif 
