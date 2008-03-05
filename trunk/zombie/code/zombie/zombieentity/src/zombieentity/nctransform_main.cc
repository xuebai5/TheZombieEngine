#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nctransform_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nctransform.h"
#include "nscene/ncscene.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialindoor.h"
#include "nphysics/ncphysicsobj.h"
#include "animcomp/nccharacter.h"
#include "zombieentity/ncsuperentity.h"
#ifndef __ZOMBIE_EXPORTER__
#include "ncsound/ncsound.h"
#include "ncnetwork/ncnetwork.h"
#ifndef NGAME
#include "ntrigger/nctriggershape.h"
#endif
#endif
#ifndef NGAME
#include "mathlib/eulerangles.h"
#endif
#include "nphysics/ncphypickableobj.h"
#include "zombieentity/ncsubentity.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncTransform, nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTransform)
    NSCRIPT_ADDCMD_COMPOBJECT('SPOS', void, SetPosition, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GPOS', const vector3&, GetPosition, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SEUL', void, SetEuler, 3, (float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GEUL', void, GetEuler, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('SQUT', void, SetQuat, 1, (const quaternion&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GQUT', const quaternion&, GetQuat, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SSCL', void, SetScale, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GSCL', const vector3&, GetScale, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSLV', void, SetLockViewer, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GSLV', bool, GetLockViewer, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('GYXZ', void, GetEulerYXZ, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('SYXZ', void, SetEulerYXZ, 3, (float, float, float), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncTransform::ncTransform() :
    sceneComp(0),
    spatialComp(0),
    spatialPortalComp(0),
    physicsComp(0),
    characterComp(0),
    frameId(0),
    updateFlags( int(~0) ),
    spatialCameraComp( 0 ),
    spatialSpaceComp( 0 ),
    spatialIndoorComp( 0 ),
#ifndef __ZOMBIE_EXPORTER__
    soundComp(0),
    networkComp( 0 ),
#endif
    dirtyEuler(false),
    dirtyQuat(false),
    active(false),
    lockViewer(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncTransform::~ncTransform()
{
    // empty 
}

//------------------------------------------------------------------------------
/**
    instance initialization
*/
void
ncTransform::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    this->sceneComp = this->entityObject->GetComponent<ncScene>();
    this->spatialComp = this->entityObject->GetComponent<ncSpatial>();
    this->spatialPortalComp = this->entityObject->GetComponent<ncSpatialPortal>();
    this->spatialCameraComp = this->entityObject->GetComponent<ncSpatialCamera>();
    this->physicsComp = this->entityObject->GetComponent<ncPhysicsObj>();
    this->spatialIndoorComp = this->entityObject->GetComponent<ncSpatialIndoor>();
    this->characterComp = this->GetComponent<ncCharacter>();
#ifndef __ZOMBIE_EXPORTER__
    this->soundComp = this->GetComponent<ncSound>();
    this->networkComp = this->GetComponent<ncNetwork>();
#endif

    this->active = true;


}

//------------------------------------------------------------------------------
/**
    update euler from transform - VERY INEFFICIENT
*/
void
ncTransform::UpdateEulerFromTransform()
{
    if (this->dirtyEuler)
    {
        const matrix44& m = this->GetTransform();
        matrix33 m33(m.x_component(), m.y_component(), m.z_component());
        this->computedEuler = m33.to_euler();

        this->dirtyEuler = false;
    }
}

//------------------------------------------------------------------------------
/**
    update quaternion from transform
*/
void
ncTransform::UpdateQuatFromTransform()
{
    if (this->dirtyQuat)
    {
        this->computedQuat = this->transform.getmatrix().get_quaternion();
        this->computedQuat.normalize();

        this->dirtyQuat = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncTransform::SetLockViewer(bool lockViewer)
{
    this->lockViewer = lockViewer;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTransform::GetLockViewer()
{
    return this->lockViewer;
}

//------------------------------------------------------------------------------
/**
    update component transform
*/
void
ncTransform::DoUpdateComponent( const compUpdate update )
{
    this->UpdateComponents( update );
}

//------------------------------------------------------------------------------
/**
    update component transform
*/
void
ncTransform::UpdateComponents( const int componentFlags )
{
    if (!this->active)
    {
        return;
    }

    if (this->sceneComp && (componentFlags & cScene) )
    {
        this->sceneComp->Update(this->transform.getmatrix());
    }

    if (this->spatialIndoorComp)
    {
        this->spatialIndoorComp->Update(this->transform.getmatrix());
    }

    if (componentFlags & cSpatial)
    {
        if (this->spatialCameraComp)
        { 
            this->spatialCameraComp->Update(this->GetTransform(), this->frameId); 
            this->frameId++;
        }
        else if (this->spatialPortalComp)
        {
            this->spatialPortalComp->Update(this->GetTransform());
        }
        else if (this->spatialComp)
        {
            this->spatialComp->Update(this->GetTransform());
        }
    }

    if (componentFlags & cPhysics)
    {
        if (this->physicsComp)
        {
            matrix44 orientation(this->GetQuat());

            orientation.set_translation(this->GetPosition());

            this->physicsComp->Update(orientation);

            this->physicsComp->Scale(this->GetScale().x);

#if !defined(NGAME) && !defined(__ZOMBIE_EXPORTER__)
            if ( this->GetComponent<ncTriggerShape>() )
            {
                this->GetComponentSafe< ncPhyPickableObj >()->UpdateWithSpatialOrScene();
            }
#endif
        }
    }

    if (componentFlags & cCharacter)
    {
        if (this->characterComp)
        {
            this->characterComp->UpdateAttachments();
        }
    }

    ncSuperentity *superentity = this->GetComponent<ncSuperentity>();
    if (superentity)
    {
        if (componentFlags & cSuperEntity)
        {
            this->DisableUpdate(cSuperEntity);
            superentity->UpdateSubentities();
            this->EnableUpdate(cSuperEntity);
        }
    }

#ifndef __ZOMBIE_EXPORTER__
    if ( this->soundComp )
    {
        this->soundComp->GetPositionFromEntity();
    }

#ifndef NGAME
    if( this->networkComp )
    {
        this->networkComp->SetModified();
    }
#endif
#endif
}

// commands

//------------------------------------------------------------------------------
/**
*/
bool
ncTransform::SaveCmds(nPersistServer* ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        ncSubentity * subentity = this->GetComponent<ncSubentity>();
        bool isSubentity = subentity && subentity->IsSubentity();
        bool isCloned = ps->GetSaveMode() == nPersistServer::SAVEMODE_CLONE;
        //when clone subentity need this transform, put this object in the world
        //but when save to disk, the global transform depends on superentity place
        bool saveGlobalTransform =  (!isSubentity) || isCloned ; 

        if( saveGlobalTransform )  
        {
            // --- setposition ---
            static const vector3 nullVec;
            const vector3& pos = this->GetPosition();
#ifndef NGAME
            if( ps->GetSaveType() == nPersistServer::SAVETYPE_RELOAD )
            {
                ps->Put(this->entityObject, 'SPOS', pos.x, pos.y, pos.z);
            }
            else 
#endif
            if (!pos.isequal(nullVec, 0.0f))
            {
                ps->Put(this->entityObject, 'SPOS', pos.x, pos.y, pos.z);
            }
        }

        if( saveGlobalTransform )
        {
            // --- setquat ---
            static const quaternion identQuat;
            const quaternion& quat = this->GetQuat();
#ifndef NGAME
            if( ps->GetSaveType() == nPersistServer::SAVETYPE_RELOAD )
            {
                ps->Put(this->entityObject, 'SQUT', quat.x, quat.y, quat.z, quat.w);
            }
            else 
#endif
            if (!quat.isequal(identQuat, 0.0f))
            {
                ps->Put(this->entityObject, 'SQUT', quat.x, quat.y, quat.z, quat.w);
            }
        }

        if( saveGlobalTransform )
        {
        // --- setscale ---
            static const vector3 oneVec(1.0f, 1.0f, 1.0f);
            const vector3& scale = this->GetScale();
#ifndef NGAME
            if( ps->GetSaveType() == nPersistServer::SAVETYPE_RELOAD )
            {
                ps->Put(this->entityObject, 'SSCL', scale.x, scale.y, scale.z);
            }
            else 
#endif
            if (!scale.isequal(oneVec, 0.0f))
            {
                ps->Put(this->entityObject, 'SSCL', scale.x, scale.y, scale.z);
            }
        }

        // --- setlockviewer ---
        if (this->GetLockViewer())
        {
            ps->Put(this->entityObject, 'MSLV', true);
        }
    }

    return true;
}


#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncTransform::SetEulerYXZ(float y, float x, float z)
{
    matrix33 matrix;
    matrix.rotate_y( n_deg2rad(y) );
    matrix.rotate_x( n_deg2rad(x) );
    matrix.rotate_z( n_deg2rad(z) );

    this->SetEuler( matrix.to_euler() );
}
#endif //NGAME

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncTransform::GetEulerYXZ(float& y, float& x, float& z)
{
    nEulerAngles euler;
    const matrix44& m44=this->transform.getmatrix();
    matrix33 m33(m44.x_component(), m44.y_component(), m44.z_component());
    euler.Set( m33 ,EulOrdYXZs);
    y = n_rad2deg(euler.y); 
    x = n_rad2deg(euler.x);
    z = n_rad2deg(euler.z);
}
#endif //NGAME