//-----------------------------------------------------------------------------
//  ncphycharacterobj_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphycharacterobj.h"
#include "nphysics/nphygeombox.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/nphysicsconfig.h"
#include "nphysics/ncphysicsobjclass.h"

#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/nctransform.h"

#ifndef __ZOMBIE_EXPORTER__
#include "ncgameplayplayer/ncgameplayplayer.h"
#endif

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyCharacterObj,ncPhyCompositeObj);

//-----------------------------------------------------------------------------
/**
    constructor
*/
ncPhyCharacterObj::ncPhyCharacterObj():
    geometryPointers(8,8),
    invGeomInitialQuat(2,1),
    geomOffset(2,1),
    active(true)
{
    //empty
}

//-----------------------------------------------------------------------------
/**
    destructor
*/
ncPhyCharacterObj::~ncPhyCharacterObj()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    User init instance code.
*/
void 
ncPhyCharacterObj::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhyCompositeObj::InitInstance( initType );
}
    
//-----------------------------------------------------------------------------
/**
    Adds a geometry to the object
    @param geometry  geometry to be added
*/

void
ncPhyCharacterObj::Add( nPhysicsGeom* geometry )
{
    ncPhyCompositeObj::Add( geometry );

    //add pointer to geometry
    this->geometryPointers.Append( geometry );
}

//-----------------------------------------------------------------------------
/**
    gets position and orientation from nccharacter
*/
//void 
//ncPhyCharacterObj::GetBoneData( int boneIndex, vector3& position, quaternion& orientation)
//{
//    ncCharacter* characterComp = this->GetComponent<ncCharacter>();
//    nCharJoint charJoint = characterComp->GetCharacter()->GetSkeleton().GetJointAt(boneIndex);
//    
//    orientation = charJoint.GetWorldQuaternion();
//    position = charJoint.GetTranslate();
//}

//-----------------------------------------------------------------------------
/**
*/
void 
ncPhyCharacterObj::UpdateCharacterCollision()
{
    quaternion quatz(0,0,0.707f,0.707f);
    quaternion hquaty(0,0.707f,0,0.707f);
    quaternion quaty(0,1,0,0);

    ncCharacter* characterComp = this->GetComponent<ncCharacter>();
    n_assert(characterComp);

    int skelIndex = characterComp->GetPhysicsSkelIndex();
    if (skelIndex < 0 ) return;

    nCharSkeleton& skel = characterComp->GetCharacter( skelIndex )->GetSkeleton();

    // loop boxes
    for(int i=0; i<this->geometryPointers.Size(); i++)
    {
        nPhysicsGeom* geometry = this->geometryPointers[i];
        nCharJoint charJoint = skel.GetJointAt(geometry->GetIdTag());

        vector3 position, scale;
        quaternion rotation;

        // update geometry data
        charJoint.GetWorldTransformation( position, rotation, scale);

        //correct rotation
        quaternion finalRotation = rotation*quatz*quaty;

        //get offset
        vector3 offset;
        this->GetGeometryOffset( geometry->GetIdTag(), offset );
        //reset it (can be done when initializing data, not repeated each frame)
        offset = this->invGeomInitialQuat[i].rotate(offset);
        //aplicate current rotation
        position += finalRotation.rotate(offset);


        //super ugly hack til first-third person have the same pivot point!
#ifndef __ZOMBIE_EXPORTER__
        if( characterComp->GetFirstPersonActive() && this->GetComponent<ncGameplayPlayer>() )
        {
            position -= vector3(0,this->GetComponentSafe<ncGameplayPlayer>()->GetCameraHeight(),0);
        }
#endif
        /// end super ugly hack

        this->geometryPointers[i]->SetPosition(position);

        if( charJoint.GetParentJointIndex() != -1)
        {
            this->geometryPointers[i]->SetOrientation(matrix33(finalRotation));
        }
        else
        {
            quaternion rootRotation = rotation*quatz*hquaty;
            this->geometryPointers[i]->SetOrientation(matrix33(rootRotation));
        }
    }

    this->GetComponent<ncTransform>()->SetPosition( this->GetComponent<ncTransform>()->GetPosition() );
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world it's the world where the object will be come to existance

    history:
        - 12-Aug-2004   David Reyes    created
*/
void ncPhyCharacterObj::Create( nPhysicsWorld* world )
{
    this->GetClassComponent<ncPhysicsObjClass>()->SetMobile( true );

    ncPhyCompositeObj::Create( world );

    for( int i=0; i< this->geometryPointers.Size(); i++)
    {
        //get initial orientation
        matrix33 matrix;
        this->geometryPointers[i]->GetOrientation(matrix);
        quaternion quat = matrix.get_quaternion();
        quat.conjugate();
        this->invGeomInitialQuat.Append( quat );
    }

    this->RegisterForPreProcess();
    this->RegisterForPostProcess();

}

//-----------------------------------------------------------------------------
/**
    Function to be process b4 running the simulation.

    history:
        - 20-Sep-2005   David Reyes    created
*/
void ncPhyCharacterObj::PreProcess()
{
    if( !this->active )
    {
        return;
    }
    // Disabling every object belonging to this physics object
    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->Disable();
    }
}

//-----------------------------------------------------------------------------
/**
    Function to be process after running the simulation.

    history:
        - 20-Sep-2005   David Reyes    created
*/
void ncPhyCharacterObj::PostProcess()
{
    if( !this->active )
    {
        return;
    }
    // Enableing every object belonging to this physics object
    for( int index(0); index < this->containerGeometries.Size(); ++index )
    {
        this->containerGeometries.GetElementAt(index)->Enable();
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
ncPhyCharacterObj::SetGeometryOffset( int jointIndex, vector3 & offset)
{
    this->geomOffset.Add( jointIndex, offset );
}

//-----------------------------------------------------------------------------
/**
    find jointIndex offset
*/
void
ncPhyCharacterObj::GetGeometryOffset( int jointIndex, vector3 & offset)
{
    if( this->geomOffset.HasKey( jointIndex ) )
    {
        offset = this->geomOffset.GetElement( jointIndex );
    }
    else
    {
        offset = vector3(0,0,0);
    }
}

//-----------------------------------------------------------------------------
/**
    match patter with linker bone names, return id of the correspondant geometry
*/
const geomid 
ncPhyCharacterObj::GetGeometryId( const nString pattern )
{
    ncCharacter* characterComp = this->GetComponentSafe<ncCharacter>();

    nCharSkeleton& skel = characterComp->GetCharacter( characterComp->GetPhysicsSkelIndex() )->GetSkeleton();

    // loop boxes
    for(int i=0; i<this->geometryPointers.Size(); i++)
    {
        nPhysicsGeom* geometry = this->geometryPointers[i];
        nCharJoint charJoint = skel.GetJointAt(geometry->GetIdTag());

        if(charJoint.GetName().MatchPattern(pattern))
        {
            return geometry->Id();
        }
    }
    n_assert2_always("That pattern doesn't match any collision box bone");
    return NoValidID;
}

//-----------------------------------------------------------------------------
/**
    Deactivates the object.

    history:
        - 27-Jan-2006   David Reyes    created
*/
void ncPhyCharacterObj::Deactivate()
{
    this->active = false;
    this->Disable();
}

//-----------------------------------------------------------------------------
/**
    Deactivates the object.

    history:
        - 27-Jan-2006   David Reyes    created
*/
void ncPhyCharacterObj::Activates()
{
    this->active = true;
    this->Enable();
}
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
