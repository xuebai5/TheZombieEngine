#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  nfxobject_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/nfxobject.h"

nNebulaScriptClass(nFXObject, "nobject");

int nFXObject::nextKey = 0;

//------------------------------------------------------------------------------
/**
*/
nFXObject::nFXObject() :
    key(nextKey++),
    lifetime(0.0f),
    unique(false),
    loaded(false),
    alive(false),
    randomTimeOffset(false)
{
    this->fxNode.SetPtr(this);
}

//------------------------------------------------------------------------------
/**
*/
nFXObject::~nFXObject()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nFXObject::Load()
{
    this->loaded = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nFXObject::SetFXPosition(const vector3& pos)
{
    this->transform.settranslation(pos);
}

//------------------------------------------------------------------------------
/**
*/
const vector3&
nFXObject::GetFXPosition() const
{
    return this->transform.gettranslation();
}

//------------------------------------------------------------------------------
/**
*/
void
nFXObject::SetFXScale(const float s)
{
    this->transform.setscale(vector3(s,s,s));
}

//------------------------------------------------------------------------------
/**
*/
float
nFXObject::GetFXScale() const
{
    return this->transform.getscale().x;
}


//------------------------------------------------------------------------------
/**
*/
void
nFXObject::SetFXRotation(const quaternion& rot)
{
    this->transform.setquatrotation(rot);
}

//------------------------------------------------------------------------------
/**
*/
const quaternion&
nFXObject::GetFXRotation() const
{
    return this->transform.getquatrotation();
}

//------------------------------------------------------------------------------
/**
*/
void 
nFXObject::SetStartRandom( bool random )
{
    this->randomTimeOffset = random;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nFXObject::GetStartRandom() const
{
    return this->randomTimeOffset;
}

//------------------------------------------------------------------------------
/**
    trigger effect as active- update time, etc.
    subclasses should override this method to handle specifics
*/
void
nFXObject::Trigger(nTime curTime)
{
    if (!this->alive)
    {
        // if it wasn't active, set as active, record current time as offset
        this->alive = true;
        this->aliveTime = curTime;
        this->Restart();
    }
    // check time elapsed, set as dead if effect expired
    if ((curTime - this->aliveTime) > this->lifetime)
    {
        this->alive = false;
    }
}

//------------------------------------------------------------------------------
/**
    restart effect-
    subclasses should override this method to perform any operations
    specific to resetting the effect: reset state or time variables, etc.
    curTime should be already up-to-date when this is called
*/
void
nFXObject::Restart()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    stop effect-
    subclasses should override this method to perform any custom
    operation to stop the effect: clear states, remove from lists, etc.
    this can be called when the effect self expires, killed by user,
    or because of garbage collection due to maximum allowed active fx.
*/
void
nFXObject::Stop()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    attach the effect to the scene graph-
    override in subclasses to render scene resources to the given scene graph
*/
bool
nFXObject::Render(nSceneGraph* /*sceneGraph*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nFXObject)

    NSCRIPT_ADDCMD('MGFK', int, GetKey, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSFP', void, SetFXPosition, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('MGFP', const vector3&, GetFXPosition, 0, (), 0, ());
    NSCRIPT_ADDCMD('AAEN', void, SetFXRotation, 1, (const quaternion&), 0, ());
    NSCRIPT_ADDCMD('AAEO', const quaternion&, GetFXRotation, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSSC', void, SetFXClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('MGSC', const char *, GetFXClass, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSFL', void, SetLifeTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD('MGFL', float, GetLifeTime, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSFU', void, SetUnique, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('MGFU', bool, IsUnique, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSFS', void, SetFXScale, 1, (const float), 0, ());
    NSCRIPT_ADDCMD('CGFS', float, GetFXScale, 0, (), 0, ());

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool 
nFXObject::SaveCmds(nPersistServer *ps)
{ 
    nCmd *cmd;

    // --- setfxclass ---
    cmd = ps->GetCmd(this, 'MSSC');
    cmd->In()->SetS(this->GetFXClass());
    ps->PutCmd(cmd);

    // --- setlifetime ---
    cmd = ps->GetCmd(this, 'MSFL');
    cmd->In()->SetF(this->GetLifeTime());
    ps->PutCmd(cmd);

    // --- setunique ---
    cmd = ps->GetCmd(this, 'MSFU');
    cmd->In()->SetB(this->IsUnique());
    ps->PutCmd(cmd);

    return true;
}
