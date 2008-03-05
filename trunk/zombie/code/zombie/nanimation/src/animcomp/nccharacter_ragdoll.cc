#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nccharacter_ragdoll.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/nchumragdoll.h"
#include "animcomp/nchumragdollclass.h"
#include "animcomp/ncfourleggedragdoll.h"
#include "animcomp/ncfourleggedragdollclass.h"
#include "ncragdoll/ncragdoll.h"
#include "ncragdoll/ncragdollclass.h"
#include "entity/nentityobjectserver.h"
#include "nragdollmanager/nragdollmanager.h"

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::LoadRagdoll()
{
    ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();

    if (charClass && charClass->GetRagDollManagerId() != -1)
    {
        this->InitializeRagdoll(this->GetEntityClass());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetRagdollEntityId(nEntityObjectId entityObjectId)
{
    this->refRagdoll = nEntityObjectServer::Instance()->GetEntityObject(entityObjectId);
    n_assert(this->refRagdoll.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::InitializeRagdoll(nEntityClass* eClass)
{
    ncCharacterClass *charClass = eClass->GetComponent<ncCharacterClass>();
    
    this->refRagdoll = nRagDollManager::Instance()->PopRagDoll(charClass->GetRagDollManagerId())->GetEntityObject();

    n_assert(this->refRagdoll.isvalid());

    // load ragdoll resource
    ncHumRagdoll *ragComp = this->refRagdoll->GetComponent<ncHumRagdoll>();
    ncFourLeggedRagdoll *rag4Comp = this->refRagdoll->GetComponent<ncFourLeggedRagdoll>();
    if (ragComp)
    {
        ragComp->Load(this->GetEntityObject());
        this->humanoidRagdoll = true;
    }
    if (rag4Comp)
    {
        rag4Comp->Load(this->GetEntityObject());
        this->humanoidRagdoll = false;
    }
    
}

//------------------------------------------------------------------------------
/**
*/
nEntityObjectId
ncCharacter::GetRagdollEntityId() const
{
    if (this->refRagdoll.isvalid())
    {
        return this->refRagdoll->GetId();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetRagdollActive(bool activate)
{
    if (this->refRagdoll.isvalid()  && activate)
    {
        this->ragdollActive = activate;
        
        // get data from active character and set to ragdollentityobject
        ncRagDoll *ragComp = this->refRagdoll->GetComponent<ncRagDoll>();
        n_assert(ragComp);

        ragComp->SetParentObject(this->GetEntityObject());
        ragComp->CreateRagdoll();

        nPhysicsServer::Instance()->GetDefaultWorld()->Add(ragComp->GetEntityObject());

        ncRagDollClass* ragClass = this->refRagdoll->GetClassComponent<ncRagDollClass>();
        n_assert(ragClass);

        quaternion quatz(0.0f, 0.0f,  0.707f, 0.707f);
        quaternion quaty(0.0f, 0.707f,0.0f,   0.707f);
        quaternion halfz(0.0f, 0.0f,  1.0f,   0.0f);

        int lodLevel = this->GetPhysicsSkelIndex();
        
        for (int i = 0; i< ragClass->GetNumRagdollJoints(); i++)
        {
            //get joint pos, rot, scale
            vector3 pos, scale, midPos;
            quaternion quat;
            int lodJointIndex = ragClass->GetGfxLodJointCorrespondenceByIndex(lodLevel, i);
            int gfxRagJointIndex = ragClass->GetGfxRagJointCorrespondenceByIndex(lodLevel, i);

            nCharJoint& charJoint = this->character[lodLevel]->GetSkeleton().GetJointAt(lodJointIndex);
            charJoint.GetWorldTransformation(pos, quat, scale);
            midPos = charJoint.GetMidPoint();
            if (this->humanoidRagdoll || i!=0)
            {
                quat = quat*quatz*quaty;
            }
            else
            {
                // scavenger body has a different orientation
                quat = quat*quatz*quaty*halfz;
            }

            ragComp->SetJointData(gfxRagJointIndex , midPos, pos, quat, scale);
        }

        ncHumRagdoll* humRag = 0;
        ncFourLeggedRagdoll* ragfourlegged = 0;
        if (this->humanoidRagdoll)
        {
            humRag = this->refRagdoll->GetComponent<ncHumRagdoll>();
            n_assert(humRag);
            humRag->InitializeRagdoll();
        }
        else
        {
            ragfourlegged = this->refRagdoll->GetComponent<ncFourLeggedRagdoll>();
            n_assert(ragfourlegged);
            ragfourlegged->InitializeRagdoll();
        }

        // set position, rotation and scale to new entity
        // set correct position/rotation to ragdoll (entitypos + bodypos), idem for rotation
        ncTransform* charTransComp = this->GetComponentSafe<ncTransform>();
        ncTransform* ragTransComp = this->refRagdoll->GetComponentSafe<ncTransform>();

        vector3 ragdollBodyPos = ragTransComp->GetPosition();
        quaternion ragdollBodyQuat = ragTransComp->GetQuat();

        ragTransComp->SetPosition(charTransComp->GetPosition() + ragdollBodyPos);
        ragTransComp->SetQuat(charTransComp->GetQuat()*ragdollBodyQuat);

        // set default pos, rot, scale to charentity because all has to be controlled by the ragdoll entity
        // only render has to be performed here.
        charTransComp->SetPosition(vector3(0, 0, 0));
        charTransComp->SetQuat(quaternion(0, 0, 0, 1));
        charTransComp->SetScale(vector3(1, 1, 1));

        this->DettachAll();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::GetRagdollActive() const
{
    return this->ragdollActive;
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2 *
ncCharacter::GetRagdollCharacter()
{
    // load ragdoll resource
    ncRagDoll *ragComp = this->refRagdoll->GetComponent<ncRagDoll>();
    n_assert(ragComp);

    return ragComp->GetCharacter();
}
