#ifndef N_FXOBJECT_H
#define N_FXOBJECT_H
//------------------------------------------------------------------------------
/**
    @class nFXObject
    @ingroup SpecialFX
    @author MA Garcias <ma.garcias@yahoo.es>

    (C) 2006 Conjurer Services, S.A.
*/
#include "kernel/nobject.h"
#include "kernel/ncmdprotonativecpp.h"
#include "mathlib/transform44.h"

class nSceneGraph;
//------------------------------------------------------------------------------
class nFXObject : public nObject
{
public:
    /// constructor
    nFXObject();
    /// destructor
    virtual ~nFXObject();
    /// object persistency
    virtual bool SaveCmds(nPersistServer* ps);

    /// update the effect for current frame
    virtual void Trigger(nTime curTime);

    /// reset the effect to the initial state
    virtual void Restart();

    /// stop the effect
    virtual void Stop();

    /// load the effect
    virtual bool Load();

    /// attach the decal to the current scene
    virtual bool Render(nSceneGraph*);

    /// get if effect is loaded
    bool IsLoaded() const;

    /// get effect alive (when expired, or destroyed)
    bool IsAlive();

    /// get transform object
    transform44& GetTransform();

    /// set start random
    void SetStartRandom(bool random);
    /// get start random
    bool GetStartRandom() const;

    /// get effect key
    int GetKey();

    /// set position (for test only, not intended full use from script)
    void SetFXPosition(const vector3&);
    /// get position (for test only, not intended full use from script)
    const vector3& GetFXPosition() const;

    /// set position (for test only, not intended full use from script)
    void SetFXScale(const float);
    /// get position (for test only, not intended full use from script)
    float GetFXScale() const;

    /// set rotation (for test only, not intended full use from script)
    void SetFXRotation(const quaternion&);
    /// get rotation (for test only, not intended full use from script)
    const quaternion& GetFXRotation() const;

    /// set entity class of the effect
    void SetFXClass(const char *);
    /// get entity class of the effect
    const char * GetFXClass();
    /// set life time
    void SetLifeTime(float);
    /// get life time
    float GetLifeTime();
    /// set unique effect
    void SetUnique(bool);
    /// get unique effect
    bool IsUnique();

protected:
    friend class nFXServer;

    /// insert node into list of objects of the same class
    void InsertNodeAfter(nFXObject*);
    /// remove node from list of objects of the same class
    void RemoveNode();
    /// get next effect of the same type
    nFXObject* GetNext();

    int key;
    static int nextKey;
    float lifetime;
    transform44 transform;
    nString fxClassName;
    nTime aliveTime;
    nNode fxNode;/// to keep fx of the same time on a list
    bool unique;
    bool alive;
    bool loaded;
    bool randomTimeOffset;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
nFXObject::GetKey()
{
    return this->key;
}

//------------------------------------------------------------------------------
/**
*/
inline
transform44&
nFXObject::GetTransform()
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXObject::SetFXClass(const char *className)
{
    this->fxClassName = className;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nFXObject::GetFXClass()
{
    return this->fxClassName.IsEmpty() ? 0 : this->fxClassName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXObject::SetLifeTime(float lifetime)
{
    this->lifetime = lifetime;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nFXObject::GetLifeTime()
{
    return this->lifetime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXObject::SetUnique(bool value)
{
    this->unique = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nFXObject::IsUnique()
{
    return this->unique;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nFXObject::IsLoaded() const
{
    return this->loaded;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nFXObject::IsAlive()
{
    return this->alive;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXObject::InsertNodeAfter(nFXObject* fxObject)
{
    n_assert(fxObject);
    this->fxNode.InsertAfter(&fxObject->fxNode);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nFXObject::RemoveNode()
{
    this->fxNode.Remove();
}

//------------------------------------------------------------------------------
/**
*/
inline
nFXObject*
nFXObject::GetNext()
{
    if (this->fxNode.GetSucc())
    {
        return (nFXObject*) this->fxNode.GetSucc()->GetPtr();
    }
    return 0;
}

//------------------------------------------------------------------------------
#endif /*N_FXOBJECT_H*/
