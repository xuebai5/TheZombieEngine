//------------------------------------------------------------------------------
//  nccharacter_attachments.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnanimation.h"
#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "nscene/ncscene.h"
#include "zombieentity/nctransform.h"
#include "entity/nentityobjectserver.h"

#include "kernel/nlogclass.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialcell.h"

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::LoadAttachedData()
{
    //attach attached entities
    for (int i=0; (!this->attachJointName.Empty()) && (i< this->attachJointName.Size()); i++)
    {
        this->Attach(this->attachJointName[i].Get(), entityObjId[i]);
    }
    this->attachJointName.Clear();
    this->entityObjId.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::UpdateAttachments()
{
    // for each attachment
    if (!this->character.Empty() && this->character[this->lastUpdatedCharacter]) //instead of n_asset because first time won't exist
    {
        bool fperson = this->GetFirstPersonActive();

        for (int index = 0; index < this->dynamicAttachmentPool.Size(); ++index)
        {
            if (this->dynamicAttachmentPool[index].GetEntityObject())
            {
                // get entityobject
                nEntityObject* attachedObject = this->dynamicAttachmentPool[index].GetEntityObject();
                if (attachedObject)
                {
                    NLOG(sceneLog, (1, "ncCharacter::UpdateAttachments(self: 0x%x (%s), joint: %s, entity: 0x%x (%s))",
                         this->GetEntityObject()->GetId(), this->GetEntityClass()->GetName(),
                         this->dynamicAttachmentPool[index].GetName().Get(),
                         attachedObject->GetId(), attachedObject->GetClass()->GetName()))

                    vector3 pos;
                    quaternion quat;
                    this->GetAttachmentTransforms(index, pos, quat, fperson);

                    NLOG(sceneLog, (1, "                              (pos: (%f, %f, %f), quat: (%f, %f, %f, %f))",
                         pos.x, pos.y, pos.z, quat.x, quat.y, quat.z, quat.w))

                    ncTransform* transform = attachedObject->GetComponent<ncTransform>();
                    transform->DisableUpdate(ncTransform::cSpatial);
                    transform->SetQuat(quat);
                    transform->SetPosition(pos);
                    transform->EnableUpdate(ncTransform::cSpatial);

                    // HACK- ncScene::UpdateParentTransform disables updates of the spatial
                    // component to prevent entities changing cells while updating
                    //if (this->GetUpdateAttachments())
                    {
                        transform->DoUpdateComponent(ncTransform::cSpatial);
                    }

                    ncSpatial* spatial = attachedObject->GetComponent<ncSpatial>();
                    NLOGCOND(sceneLog, spatial && spatial->GetCell(), (1, "                              (cell: %d)",
                             spatial->GetCell()->GetId()))
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::Attach(const char* jointName, nEntityObjectId id)
{
    if (this->character.Size() > 0)
    {
        nEntityObject* entity = nEntityObjectServer::Instance()->GetEntityObject(id);
        n_assert_return(entity,);

        NLOG(sceneLog, (0, "ncCharacter::Attach(self: 0x%x (%s), jointName: %s, entity: 0x%x (%s))",
             this->GetEntityObject()->GetId(), this->GetEntityClass()->GetName(),
             jointName, id, entity->GetClass()->GetName()))

        for (int index = 0; index < this->dynamicAttachmentPool.Size(); index++)
        {
            if (this->dynamicAttachmentPool[index].GetName() == jointName)
            {
                this->dynamicAttachmentPool[index].SetEntityObject(entity);

                ncScene* sceneComp = entity->GetComponent<ncScene>();
                if (sceneComp)
                {
                    sceneComp->SetParentEntity(this->GetEntityObject());
                }
                return;
            }
        }

        NLOG(sceneLog, (0, "ncCharacter::Attach( ERROR: joint not found: '%s' )", jointName))
    }
    else
    {
        // if the character is not initialized, save attachments for later use
        this->attachJointName.Append(jointName);
        this->entityObjId.Append(id);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::Dettach(nEntityObjectId id)
{
    nEntityObject* entity = nEntityObjectServer::Instance()->GetEntityObject(id);
    n_assert_return(entity,);

    NLOG(sceneLog, (0, "ncCharacter::Dettach(self: 0x%x (%s), entity: 0x%x (%s))", 
         this->GetEntityObject()->GetId(), this->GetEntityClass()->GetName(),
         id, entity->GetClass()->GetName()))

    for (int index = 0; index < this->dynamicAttachmentPool.Size(); index++)
    {
        if (this->dynamicAttachmentPool[index].GetEntityObject() == entity)
        {
            this->dynamicAttachmentPool[index].SetEntityObject(0);
            if (entity->GetComponent<ncScene>())
            {
                entity->GetComponent<ncScene>()->SetParentEntity(0);
            }
            return;
        }
    }

    NLOG(sceneLog, (0, "ncCharacter::Dettach( ERROR: attached entity not found: 0x%x )", id))
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::DettachAll()
{
    for (int i=0; i< this->dynamicAttachmentPool.Size(); i++)
    {
        nEntityObject* eObj = this->dynamicAttachmentPool[i].GetEntityObject();
        if (eObj)
        {
            this->Dettach(eObj->GetId());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetNumberAttachments() const
{
    return this->dynamicAttachmentPool.Size();
}

//------------------------------------------------------------------------------
/**
*/
const char*
ncCharacter::GetAttachmentName(int attachNumber) const
{
    return this->dynamicAttachmentPool[attachNumber].GetName().Get();
}

//------------------------------------------------------------------------------
/**
*/
nEntityObjectId
ncCharacter::GetAttachedEntity(int attachIndex) const
{
    nEntityObject* eObj = this->dynamicAttachmentPool[attachIndex].GetEntityObject();
    if (eObj)
    {
        return eObj->GetId();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    get world attachment matrix, given the attached entityobject
*/
void
ncCharacter::GetAttachmentTransforms(nEntityObject* eObj, vector3& pos, quaternion& quat, bool fperson)
{
    int attachmentIndex;
    for (attachmentIndex=0; attachmentIndex< this->dynamicAttachmentPool.Size(); attachmentIndex++)
    {
        if (this->dynamicAttachmentPool[attachmentIndex].GetEntityObject() == eObj)
        {
            break;
        }
    }
    
    //if found, get matrix
    if (attachmentIndex != this->dynamicAttachmentPool.Size())
    {
        this->GetAttachmentTransforms(attachmentIndex, pos, quat, fperson);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::GetAttachmentTransforms(int attIndex, vector3& pos, quaternion& quat, bool fperson)
{
    nArray<nDynamicAttachment> attchArray = this->dynamicAttachmentPool;
    int characterIdx = this->GetPhysicsSkelIndex();
    int jointIdx;
    if (fperson)
    {
        attchArray = this->fpersonAttachmentPool;
        characterIdx = this->firstpersonCharIdx;
        jointIdx = attchArray[attIndex].GetJointIndex(0);
    }
    else
    {
        jointIdx = attchArray[attIndex].GetJointIndex(characterIdx);
    }

    // get jointdata
    n_assert(jointIdx != -1);
    const matrix44& m = this->character[characterIdx]->GetSkeleton().GetJointAt(jointIdx).GetMatrix();

    // get helper world data
    const matrix44 helperWorldM(attchArray[attIndex].GetMatrix() * m);

    // set pos, rot
    ncTransform* characterTransf = this->GetComponent<ncTransform>();
    const matrix44& characterMatrix = characterTransf->GetTransform();
    quaternion charquat(characterMatrix.get_quaternion());
    charquat.normalize();

    quaternion helperWorldQ(helperWorldM.get_quaternion());
    helperWorldQ.normalize();

    quat = charquat * helperWorldQ;
    quat.normalize();
    pos = characterMatrix.pos_component() + charquat.rotate(helperWorldM.pos_component());
}
