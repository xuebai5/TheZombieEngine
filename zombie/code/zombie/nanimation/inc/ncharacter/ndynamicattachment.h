#ifndef N_DYNAMICATTACHMENT_H
#define N_DYNAMICATTACHMENT_H
//------------------------------------------------------------------------------
/**
    @class nDynamicAttachment
    @ingroup NebulaCharacterAnimationSystem

    @brief Implements 

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentityobject.h"

//------------------------------------------------------------------------------
class nDynamicAttachment
{
public:
    /// constructor
    nDynamicAttachment();
    /// constructor
    nDynamicAttachment(nString name, int jtIndex, const vector3& translation, const quaternion& rotation, nEntityObject* entityObject = 0);
    /// constructor
    nDynamicAttachment(nString name, nArray<int>& jtIndices, const vector3& translation, const quaternion& rotation, nEntityObject* entityObject = 0);

    /// destructor
    ~nDynamicAttachment();

    /// set entityObjectId
    void SetEntityObject(nEntityObject* eObj);
    /// get entityObjectId
    nEntityObject* GetEntityObject() const;
    
    /// set entityObjectId
    void SetName(nString name);
    /// get entityObjectId
    const nString& GetName() const;
    
    /// set entityObjectId
    void SetJointIndex(int jtIndex);
    /// get entityObjectId
    const int GetJointIndex() const;
    /// set entityObjectId
    void SetJointIndex(int arrayIdx, int jtIndex);
    /// get entityObjectId
    const int GetJointIndex(int arrayIdx) const;

    /// set entityObjectId
    void SetTranslation(const vector3& trans);
    /// get entityObjectId
    const vector3& GetTranslation() const;
    
    /// set entityObjectId
    void SetRotation(const quaternion & quat);
    /// get entityObjectId
    const quaternion& GetRotation() const;
    
    /// set entityObjectId
    void SetMatrix(const matrix44 & m);
    /// get entityObjectId
    const matrix44& GetMatrix() const;

private:
    nString attachName;
    nArray<int> jointIndices;
    vector3 translation;
    quaternion rotation;
    vector3 scale;
    matrix44 matrix;
    nRef<nEntityObject> refEntityObject;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicAttachment::nDynamicAttachment():
    attachName(),
    jointIndices(1,1,-1),
    translation(),
    rotation(),
    scale(),
    matrix()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicAttachment::nDynamicAttachment(nString name, int jtIndex, const vector3 & translation, const quaternion & rotation, nEntityObject* entityObject):
    attachName(name),
    jointIndices(1,1,jtIndex),
    translation(translation),
    rotation(rotation),
    scale(),
    matrix()
    
{
    matrix.set(rotation);
    matrix.set_translation(translation);
    if (entityObject)
    {
        this->refEntityObject = entityObject;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicAttachment::nDynamicAttachment(nString name, nArray<int>& jtIndices, const vector3 & translation, const quaternion & rotation, nEntityObject* entityObject):
    attachName(name),
    jointIndices(jtIndices),
    translation(translation),
    rotation(rotation),
    scale(),
    matrix()
{
    if (entityObject)
    {
        this->refEntityObject = entityObject;
    }
    matrix.set(rotation);
    matrix.set_translation(translation);
}

//------------------------------------------------------------------------------
/**
*/
inline
nDynamicAttachment::~nDynamicAttachment()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nDynamicAttachment::SetEntityObject(nEntityObject* eObj)
{
    this->refEntityObject = eObj;
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject*
nDynamicAttachment::GetEntityObject() const
{
    if (this->refEntityObject.isvalid())
    {
        return this->refEntityObject.get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void  
nDynamicAttachment::SetName(nString name)
{
    this->attachName =  name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString &
nDynamicAttachment::GetName() const
{
    return this->attachName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void  
nDynamicAttachment::SetJointIndex(int jtIndex)
{
    this->jointIndices[0] = jtIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const int  
nDynamicAttachment::GetJointIndex() const
{
    if (this->jointIndices.Size() > 0)
    {
        return this->jointIndices[0];
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void  
nDynamicAttachment::SetJointIndex(int arrayIdx, int jtIndex)
{
    this->jointIndices.Set(arrayIdx, jtIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
const int
nDynamicAttachment::GetJointIndex(int arrayIdx) const
{
    if (this->jointIndices.Size() > arrayIdx)
    {
        return this->jointIndices[arrayIdx];
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void  
nDynamicAttachment::SetTranslation(const vector3& trans)
{
    this->translation = trans;
    this->matrix.set_translation(trans);
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nDynamicAttachment::GetTranslation() const
{
    return this->translation;
}

//------------------------------------------------------------------------------
/**
*/
inline
void  
nDynamicAttachment::SetRotation(const quaternion & quat)
{
    this->rotation = quat;
    this->matrix.set(quat);
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion& 
nDynamicAttachment::GetRotation() const
{
    return this->rotation;
}

//------------------------------------------------------------------------------
/**
*/
inline
void  
nDynamicAttachment::SetMatrix(const matrix44 & m)
{
    this->matrix = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nDynamicAttachment::GetMatrix() const
{
    return this->matrix;
}

//------------------------------------------------------------------------------
#endif
