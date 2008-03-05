#ifndef NC_TRANSFORM_H
#define NC_TRANSFORM_H
//------------------------------------------------------------------------------
/**
    @class ncTransform
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Entity component to hold the entity global transform.
    Also, it synchronizes the global transform across several components.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "mathlib/transform44.h"
#include "mathlib/polar.h"

//------------------------------------------------------------------------------
class ncScene;
class ncSpatial;
class ncSpatialCamera;
class ncSpatialPortal;
class ncSpatialCamera;
class ncSpatialSpace;
class ncSpatialIndoor;
class ncPhysicsObj;
class ncCharacter;
class ncSound;
class ncNetwork;

//------------------------------------------------------------------------------
class ncTransform : public nComponentObject
{

    NCOMPONENT_DECLARE(ncTransform, nComponentObject);

public:
    /// Enumerates all components to be update
    typedef enum {
        cPhysics        = 1 << 0,
        cSpatial        = 1 << 1,
        cScene          = 1 << 2,
        cSuperEntity    = 1 << 3,
        cCharacter      = 1 << 4,
        allComponents   = ~0
    } compUpdate;

    /// constructor
    ncTransform();
    /// destructor
    virtual ~ncTransform();
    /// component persistency
    bool SaveCmds(nPersistServer* ps);
    /// set rotation in euler angles
    void SetEuler(const vector3& euler);
    /// get rotation in euler angles
    const vector3& GetEuler();
    /// set rotation using polar coordinates
    void SetPolar(const polar2& angles);
    /// get rotation using polar coordinates
    const polar2& GetPolar();
    /// get current matrix
    const matrix44& GetTransform();

    /// set position vector
    void SetPosition(const vector3&);
    /// get position vector
    const vector3& GetPosition() const;
    /// set euler rotation
    void SetEuler(float, float, float);
    /// get euler rotation
    void GetEuler(float&, float&, float&);
    /// set quaternion rotation
    void SetQuat(const quaternion&);
    /// get quaternion rotation
    const quaternion& GetQuat();
    /// set scale vector
    void SetScale(const vector3&);
    /// get scale vector
    const vector3& GetScale();
    /// set position vector
    void SetLockViewer(bool);
    /// get position vector
    bool GetLockViewer();
#ifndef NGAME
    /// get euler rotation YXZ , this is util for edition of any objects
    void GetEulerYXZ(float&, float&, float&);
    /// set euler rotation YXZ , this is util for edition of any objects
    void SetEulerYXZ(float, float, float);
#endif

#ifndef NGAME
#endif

    /// enables physics update
    void EnableUpdate( const compUpdate update );
    /// disables physics update
    void DisableUpdate( const compUpdate update );
    /// the retun the update comoponents are enabled
    compUpdate GetUpdateFlag() const;
    /// returns if a component is mean to be updated
    const bool UpdateComponent( const compUpdate update ) const;
    /// force updating a specific component
    void DoUpdateComponent( const compUpdate update );

    /// initialize component pointers from entity object
    void InitInstance( nObject::InitInstanceMsg /*initType*/ );

private:

    /// compute euler transform from final transform component
    void UpdateEulerFromTransform();
    /// compute quaternion from final transform component
    void UpdateQuatFromTransform();
    /// synchronize transform across different components
    void UpdateComponents( const int componentFlags );

    int frameId; // FIXME frameid should be moved somewhere else

    transform44 transform;

    vector3 computedEuler;
    quaternion computedQuat;
    bool dirtyEuler;            ///< local (computed) euler needs recomputing
    bool dirtyQuat;             ///< local (computed) quaternion needs recomputing
    bool lockViewer;

    ncScene *sceneComp;
    ncSpatial *spatialComp;
    ncSpatialPortal *spatialPortalComp;
    ncSpatialCamera *spatialCameraComp;
    ncSpatialSpace *spatialSpaceComp;
    ncSpatialIndoor *spatialIndoorComp;
    ncPhysicsObj* physicsComp;
    ncCharacter * characterComp;
#ifndef __ZOMBIE_EXPORTER__
    ncSound* soundComp;
    ncNetwork * networkComp;
#endif

    int updateFlags;    ///< stores the flags to update (by default all enabled)
    bool active;

};

//------------------------------------------------------------------------------
/**
*/
inline
void
ncTransform::SetPosition(const vector3& pos)
{
    this->transform.settranslation(pos);
    this->UpdateComponents( this->updateFlags );
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ncTransform::GetPosition() const
{
    return this->transform.gettranslation();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncTransform::SetEuler(float x, float y, float z)
{
    this->transform.seteulerrotation(vector3(n_deg2rad(x), n_deg2rad(y), n_deg2rad(z)));
    this->dirtyQuat = true;
    this->UpdateComponents( this->updateFlags );
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncTransform::GetEuler(float& x, float& y, float& z)
{
    const vector3& euler = this->GetEuler();
    x = n_rad2deg(euler.x);
    y = n_rad2deg(euler.y);
    z = n_rad2deg(euler.z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncTransform::SetEuler(const vector3& euler)
{
    this->transform.seteulerrotation(euler);
    this->dirtyQuat = true;
    this->UpdateComponents( this->updateFlags );
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ncTransform::GetEuler()
{
    if (this->transform.iseulerrotation())
    {
        return this->transform.geteulerrotation();
    }
    else
    {
        this->UpdateEulerFromTransform();
        return this->computedEuler;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncTransform::SetQuat(const quaternion& quat)
{
    this->transform.setquatrotation(quat);
    this->dirtyEuler = true;
    this->UpdateComponents( this->updateFlags );
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion&
ncTransform::GetQuat()
{
    if (this->transform.iseulerrotation())
    {
        this->UpdateQuatFromTransform();
        return this->computedQuat;
    }
    else
    {
        return this->transform.getquatrotation();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncTransform::SetScale(const vector3& scale)
{
    this->transform.setscale(scale);
    this->UpdateComponents( this->updateFlags );
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
ncTransform::GetScale()
{
    return this->transform.getscale();
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
ncTransform::GetTransform()
{
    return this->transform.getmatrix();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncTransform::SetPolar(const polar2& angles)
{
    this->transform.seteulerrotation(vector3(angles.theta, angles.rho, 0.0f));
    this->dirtyQuat = true;
    this->UpdateComponents( this->updateFlags );
}

//------------------------------------------------------------------------------
/**
*/
inline
const polar2&
ncTransform::GetPolar()
{
    static polar2 polar;
    if (this->transform.iseulerrotation())
    {
        const vector3& euler = this->transform.geteulerrotation();
        polar.set(euler.x, euler.y);
        return polar;
    }
    else
    {
        this->UpdateEulerFromTransform();
        polar.set(this->computedEuler.x, this->computedEuler.y);
        return polar;
    }
}

//-----------------------------------------------------------------------------
/**
    Enables physics update.

    history:
        - 11-05-2005   David Reyes    created
*/
inline
void ncTransform::EnableUpdate( const compUpdate update )
{
    this->updateFlags |= update;
}
//-----------------------------------------------------------------------------
/**
    Disables physics update.

    history:
        - 11-05-2005   David Reyes    created
*/
inline
void ncTransform::DisableUpdate( const compUpdate update )
{
    this->updateFlags &= ~update;
}


//-----------------------------------------------------------------------------
/**
    Get  update flags.
    history:
*/
inline
ncTransform::compUpdate
ncTransform::GetUpdateFlag() const
{
    return static_cast<compUpdate>(this->updateFlags);
}

//-----------------------------------------------------------------------------
/**
    Returns if a component is mean to be updated.

    history:
        - 11-05-2005   David Reyes    created
*/
inline
const bool ncTransform::UpdateComponent( const compUpdate update ) const
{
    return this->updateFlags & update ? true : false;
}

//------------------------------------------------------------------------------
#endif
