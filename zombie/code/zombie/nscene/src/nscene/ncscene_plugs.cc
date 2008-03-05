#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscene_plugs.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscene.h"
#include "nscene/ncsceneclass.h"
#include "animcomp/nccharacter.h"
#include "entity/nentityobjectserver.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
*/
void
ncScene::Plug(const char* attachPoint, nEntityObjectId id)
{
    if (this->plugArray.Size() > 0)
    {
        nEntityObject* pluggedEntity = nEntityObjectServer::Instance()->GetEntityObject(id);
        n_assert_return(pluggedEntity, );

        NLOG(sceneLog, (0, "ncScene::Plug( self: 0x%x (%s), frameId: %d, attachPoint: %s, entity: 0x%x (%s) )", 
             this->GetEntityObject()->GetId(), this->GetEntityClass()->GetName(),
             this->GetFrameId(), attachPoint, id, pluggedEntity->GetClass()->GetName()))

        ncSceneClass* sceneClass = this->GetClassComponent<ncSceneClass>();
        
        int numPlugs = sceneClass->GetPlugs().Size();
        for (int i = 0; i < numPlugs; i++)
        {
            if (sceneClass->GetPlugs()[i].GetName() == attachPoint)
            {
                this->plugArray[i] = id;

                // force immediate update of plugged entities
                ncTransform* transComp = this->GetComponent<ncTransform>();
                transComp->DoUpdateComponent(ncTransform::allComponents);

                // set as parent entity for scene attach
                ncScene* sceneComp = pluggedEntity->GetComponent<ncScene>();
                if (sceneComp)
                {
                    sceneComp->SetParentEntity(this->GetEntityObject());
                }

                break;
            }
        }
    }
    else
    {
        int slotIndex = this->slotName.FindIndex( attachPoint );
        if (slotIndex != -1)
        {
            this->entityObjId[ slotIndex ] = id;
        }
        else
        {
            this->slotName.Append(nString(attachPoint));
            this->entityObjId.Append(id);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::UnPlug(nEntityObjectId id)
{
    int plugIndex = this->plugArray.FindIndex(id);
    if (plugIndex != -1)
    {
        nEntityObject* pluggedEntity = nEntityObjectServer::Instance()->GetEntityObject(id);
        n_assert_return(pluggedEntity, );

        NLOG(sceneLog, (0, "ncScene::UnPlug( self: 0x%x (%s), frameId: %d, entity: 0x%x (%s) )",
             this->GetEntityObject()->GetId(), this->GetEntityClass()->GetName(),
             this->GetFrameId(), id, 
             pluggedEntity->GetClass()->GetName()))

        this->plugArray[plugIndex] = 0;

        ncScene* sceneComp = pluggedEntity->GetComponent<ncScene>();
        if (sceneComp)
        {
            sceneComp->SetParentEntity(0);
        }
    }
    else if (this->plugArray.Empty())
    {
        int slotIndex = this->entityObjId.FindIndex(id);
        if (slotIndex != -1)
        {
            this->slotName.Erase(slotIndex);
            this->entityObjId.Erase(slotIndex);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::Update(const matrix44& transform)
{
    ncSceneClass* sceneClass = this->GetClassComponent<ncSceneClass>();

    nEntityObject* parentEntity = this->GetParentEntity();
    bool updateSpatial = false;
    if (parentEntity)
    {
        ncCharacter* parentCharacter = parentEntity->GetComponent<ncCharacter>();
        if (parentCharacter)
        {
            updateSpatial = parentCharacter->GetUpdateAttachments();
        }
    }

    for (int plugIndex = 0; plugIndex < this->plugArray.Size(); ++plugIndex)
    {
        if (this->plugArray[plugIndex] != 0)
        {
            // find entity
            nEntityObject* plugObject = nEntityObjectServer::Instance()->GetEntityObject(this->plugArray[plugIndex]);

            // get helper data
            const matrix44 m = sceneClass->GetPlugs()[plugIndex].GetMatrix();

            // set position, rotation
            quaternion objQuat = transform.get_quaternion();
            objQuat.normalize();

            quaternion helperQuat = m.get_quaternion();
            helperQuat.normalize();

            quaternion quat = objQuat*helperQuat;
            quat.normalize();

            if (plugObject)
            {
                ncTransform* transComp = plugObject->GetComponent<ncTransform>();
                transComp->DisableUpdate(ncTransform::cSpatial);
                transComp->SetQuat(quat);
                transComp->SetPosition(transform.pos_component() + objQuat.rotate(m.pos_component()));
                transComp->EnableUpdate(ncTransform::cSpatial);

                //if (updateSpatial)
                {
                    transComp->DoUpdateComponent(ncTransform::cSpatial);
                }

            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
const nString
ncScene::GetPlugName(int plugIndex) 
{
    ncSceneClass* sceneClass = this->GetClassComponent<ncSceneClass>();
    
    n_assert( plugIndex < sceneClass->GetPlugs().Size());
    
    return sceneClass->GetPlug(plugIndex).GetName();
}

//------------------------------------------------------------------------------
/**
    @param eid identifier of the entity
    @returns the name of the plug
*/
const nString
ncScene::GetPlugNameEntity(nEntityObjectId eid)
{
    int plugIndex = this->plugArray.FindIndex(eid);
    if(plugIndex != -1)
    {
        return this->GetPlugName(plugIndex);
    }
    
    return "";
}

//------------------------------------------------------------------------------
/**
*/
int
ncScene::GetNumPlugs() 
{
    ncSceneClass* sceneClass = this->GetClassComponent<ncSceneClass>();    
    return sceneClass->GetPlugs().Size();
}

//------------------------------------------------------------------------------
/**
*/
nEntityObjectId
ncScene::GetPluggedEntity(int plugIdx)
{
    return this->plugArray[plugIdx];
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::GetWorldPlugData( const nString& plugName, vector3& pos, quaternion& rotation )
{
    ncSceneClass* sceneClass = this->GetClassComponent<ncSceneClass>();

    const vector3 instancePos = this->GetComponentSafe<ncTransform>()->GetPosition();
    quaternion instanceQuat = this->GetComponentSafe<ncTransform>()->GetQuat();
    instanceQuat.normalize();

    const nArray<nDynamicAttachment>& plugs = sceneClass->GetPlugs();
    for (int plugIndex = 0; plugIndex < plugs.Size(); ++plugIndex)
    {
        if (plugs[plugIndex].GetName() == plugName)
        {
            // get helper data
            const matrix44& m = plugs[plugIndex].GetMatrix();

            quaternion helperQuat = m.get_quaternion();
            helperQuat.normalize();

            rotation = instanceQuat * helperQuat;
            rotation.normalize();

            pos = instancePos + instanceQuat.rotate(m.pos_component());

            break;
        }
    }
}
