#ifndef N_SWINGSHAPENODE_H
#define N_SWINGSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nSwingShapeNode

    Extends nShapeNode and provides the rotation matrix needed for
    the swing and leaves shaders.

    (C) 2003 RadonLabs GmbH
*/
#include "nscene/nsurfacenode.h"
#include "variable/nvariable.h"

//------------------------------------------------------------------------------
class nSwingSurfaceNode : public nSurfaceNode
{
public:
    /// constructor
    nSwingSurfaceNode();
    /// destructor
    virtual ~nSwingSurfaceNode();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// override shader parameter rendering
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// set the max swing angle
    void SetSwingAngle(float f);
    /// get the swing angle
    float GetSwingAngle() const;
    /// set the swing time period
    void SetSwingTime(float t);
    /// get the swing time period
    float GetSwingTime() const;

private:
    /// compute a permuted swing angle
    float ComputeAngle(const vector3& pos, nTime time) const;

    nVariable::Handle timeVarHandle;
    nVariable::Handle windVarHandle;
    float swingAngle;
    float swingTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSwingSurfaceNode::SetSwingAngle(float f)
{
    this->swingAngle = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSwingSurfaceNode::GetSwingAngle() const
{
    return this->swingAngle;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSwingSurfaceNode::SetSwingTime(float t)
{
    this->swingTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSwingSurfaceNode::GetSwingTime() const
{
    return this->swingTime;
}

//------------------------------------------------------------------------------
#endif
