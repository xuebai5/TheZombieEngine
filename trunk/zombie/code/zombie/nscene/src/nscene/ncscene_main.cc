#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscene_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscene.h"
#include "nscene/ncsceneclass.h"
#include "nscene/ncscenelight.h"
#include "animcomp/nccharacter.h"
#include "nspatial/ncspatiallight.h"

#ifndef NGAME
#include "zombieentity/ncdictionary.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncScene,nComponentObject);

//------------------------------------------------------------------------------
/**
*/
ncScene::ncScene() :
    passEnabledFlags(0xfffffff),
    sceneClass(0),
    time(0.0f),
    frameid(0),
    plugArray(0,1),
    attachIndex(0),
    attachFlags(AllFlags),
    maxMaterialLevel(0),
    hiddenEntity(false),
    linksAreDirty(false)
    #ifndef NGAME
   ,lastTimeRendered(0)
    #endif //NGAME
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncScene::~ncScene()
{
    n_assert(this->GetNumLinks() == 0);
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::InitInstance(nObject::InitInstanceMsg initType)
{
    this->sceneClass = this->GetEntityObject()->GetClassComponent<ncSceneClass>();

    if (this->sceneClass)
    {
        if (initType == nObject::NewInstance)
        {
            this->passEnabledFlags = sceneClass->GetPassEnabledFlags();
        }

        this->maxMaterialLevel = sceneClass->GetMaxMaterialLevel();
    }

    #ifndef NGAME
    if (!nKernelServer::Instance()->Lookup("/usr/scene/bbox"))
    {
        nKernelServer::Instance()->New("nscenenode", "/usr/scene/bbox");
    }
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::SetVectorOverride(const char *name, vector4 value)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        this->shaderOverrides.SetArg(param, nShaderArg(value));
    }
}

//------------------------------------------------------------------------------
/**
*/
vector4
ncScene::GetVectorOverride(const char *name)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        const nFloat4& val = this->shaderOverrides.GetArg(param).GetFloat4();
        return vector4(val.x, val.y, val.z, val.w);
    }
    return vector4();
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::SetFloatOverride(const char *name, float value)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        this->shaderOverrides.SetArg(param, nShaderArg(value));
    }
}

//------------------------------------------------------------------------------
/**
*/
float
ncScene::GetFloatOverride(const char *name)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        return this->shaderOverrides.GetArg(param).GetFloat();
    }
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::SetIntOverride(const char *name, int value)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        this->shaderOverrides.SetArg(param, nShaderArg(value));
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncScene::GetIntOverride(const char *name)
{
    nShaderState::Param param = nShaderState::StringToParam(name);
    if (param != nShaderState::InvalidParameter)
    {
        return this->shaderOverrides.GetArg(param).GetInt();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
const bbox3&
ncScene::GetLocalBox()
{
    if (this->IsValid())
    {
        return this->refRootNode->GetLocalBox();
    }
    else
    {
        static const bbox3 dummyBox;
        return dummyBox;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::SetPassEnabled(const char *pass, bool enabled)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(pass);
    int passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
    if (enabled)
    {
        this->passEnabledFlags |= (1<<passIndex);
    }
    else
    {
        this->passEnabledFlags &= ~(1<<passIndex);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncScene::GetPassEnabled(const char *pass)
{
    nFourCC fourcc = nVariableServer::StringToFourCC(pass);
    int passIndex = nSceneServer::Instance()->GetPassIndexByFourCC(fourcc);
    return (this->passEnabledFlags & (1<<passIndex)) != 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncScene::AddLink(nEntityObject* link)
{
    n_assert(link);
    n_assert(link->GetComponent<ncScene>());
    if (this->linkArray.FindIndex(link) == -1)
    {
        NLOG(sceneLog, (0, "ncScene::AddLink( self: 0x%x (%s), link: 0x%x (%s) )", 
             this->GetEntityObject()->GetId(), this->GetEntityObject()->GetClass()->GetName(),
             link->GetId(), link->GetClass()->GetName()))

        this->linkArray.Append(link);
        this->linksAreDirty = true;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    remove a link to another render context
*/
void 
ncScene::RemoveLink(nEntityObject* link)
{
    int index(this->linkArray.FindIndex(link));
    n_assert(index != -1);
    if (index != -1)
    {
        NLOG(sceneLog, (0, "ncScene::RemoveLink( self: 0x%x (%s), link: 0x%x (%s) )",
             this->GetEntityObject()->GetId(), this->GetEntityObject()->GetClass()->GetName(),
             link->GetId(), link->GetClass()->GetName()))

        this->linkArray.EraseQuick(index);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::SortLinks()
{
    if (this->linksAreDirty)
    {
        this->linkArray.QSort(PrioritySorter);
        this->linksAreDirty = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
int __cdecl
ncScene::LightSorter(nEntityObject* const* e0, nEntityObject* const* e1)
{
    const ncSceneLight* light0 = (*e0)->GetComponent<ncSceneLight>();
    const ncSceneLight* light1 = (*e1)->GetComponent<ncSceneLight>();
    return light0->GetLightPriority() - light1->GetLightPriority();
}

//------------------------------------------------------------------------------
/**
    qsort() hook for Render()
    Sort geometry nodes by its surface.
*/
int __cdecl
ncScene::PrioritySorter(const void *p0, const void *p1)
{
    const ncSceneLight* light0 = (*(nEntityObject**)(p0))->GetComponent<ncSceneLight>();
    const ncSceneLight* light1 = (*(nEntityObject**)(p1))->GetComponent<ncSceneLight>();
    int diff = light0->GetLightPriority() - light1->GetLightPriority();
    if (!diff)
    {
        diff = (*(nEntityObject**)(p1))->GetId() - (*(nEntityObject**)(p0))->GetId();
    }
    return diff;
}

//------------------------------------------------------------------------------
/**
    Initialize this instance.
    
    @todo allow dynamically changing root node for the object.
    - use editor-only signal OnSceneClassChanged ?
*/
bool
ncScene::Load()
{
    n_assert(!this->IsValid());

    // @todo move to its own ::LoadPlugs() method
    // create plug structure
    ncSceneClass* sceneClass = this->GetEntityObject()->GetClassComponent<ncSceneClass>();
    if (sceneClass)
    {
        this->plugArray.SetFixedSize(sceneClass->GetPlugs().Size());
        for (int i = 0; i < sceneClass->GetPlugs().Size(); i++)
        {
            this->plugArray[i] = 0; //invalid entityObjectId
        }
    }
    else
    {
        this->plugArray.SetFixedSize(0);
    }

    // plug plugged entities
    for (int index = 0; !this->slotName.Empty() && (index < this->slotName.Size()); index++)
    {
        this->Plug(this->slotName[index].Get(), this->entityObjId[index]);
    }

    this->slotName.Clear();
    this->entityObjId.Clear();

    // copy shader overrides from class
    if (this->sceneClass)
    {
        this->shaderOverrides.SetParams(sceneClass->GetShaderOverrides());
    }

    // initialize root node
    nSceneNode *rootNode = sceneClass->GetRootNode();
    if (rootNode)
    {
        rootNode->EntityCreated(this->GetEntityObject());
        rootNode->PreloadResources();

        this->refRootNode = rootNode;
    }

    // initialize always on top
    //if (sceneClass && sceneClass->GetAlwaysOnTop())
    //{
    //    this->attachFlags |= AlwaysOnTop;
    //}

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncScene::Unload()
{
    if (this->IsValid())
    {
        this->refRootNode->EntityDestroyed(this->GetEntityObject());
        this->refRootNode.invalidate();
        this->localVarArray.Reset();
    }
}

//------------------------------------------------------------------------------
/**
    Render the entity. This does no visibility check, you should do this
    beforehand and don't render the object when it's not visible (either
    use the this object's IsVisible() method, or a hierarchical culling
    method, for instance through using the nQuadTree class.
*/
void
ncScene::Render(nSceneGraph *sceneGraph)
{
    if (!this->IsValid())
    {
        #ifndef NGAME
        if (this->sceneClass)
        {
            static int bboxPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('bbox'));
            static nAutoRef<nSceneNode> refBoxNode("/usr/scene/bbox");
            //add oriented dummy box to a debug stream
            sceneGraph->AddGroup(bboxPassIndex, refBoxNode.get(), this->GetEntityObject());
        }
        #endif
    }
    else
    {
        #ifndef NGAME
        //apply edited class properties
        if (this->sceneClass && sceneClass->GetLastEditedTime() > this->lastTimeRendered)
        {
            this->SetMaxMaterialLevel(this->sceneClass->GetMaxMaterialLevel());
            this->shaderOverrides.SetParams(sceneClass->GetShaderOverrides());
            this->lastTimeRendered = this->GetTime();
        }
        #endif

        //TODO- move to an overridable ::DoRender(nSceneGraph*) method
        this->SortLinks();
        sceneGraph->Attach(this->GetEntityObject());
    }
}

//------------------------------------------------------------------------------
/**
    Called only at scene render to automatically render entities dependent
    on this (attachments, plugs), updating its transform to fit the current
    level of detail accordingly.
*/
void
ncScene::RenderAttachedEntities(nSceneGraph* sceneGraph)
{
    //UGLY- attach attached entities if they have not been attached this frame
    ncCharacter* character = this->GetComponent<ncCharacter>();
    if (character)
    {
        for (int index = 0; index < character->GetNumberAttachments(); ++index)
        {
            nEntityObjectId eoid = character->GetAttachedEntity(index);
            if (eoid != 0)
            {
                nRefEntityObject refAttachedEntity(eoid);
                if (refAttachedEntity.isvalid())
                {
                    ncScene* sceneComponent = refAttachedEntity->GetComponent<ncScene>();
                    if (sceneComponent && sceneComponent->GetRootNode() && sceneComponent->GetFrameId() != this->GetFrameId())
                    {
                        sceneComponent->SetFrameId(this->GetFrameId());
                        sceneComponent->SetTime(this->GetTime());
                        sceneComponent->Render(sceneGraph);
                    }
                }
            }
        }
    }

    // render entities in plugs
    for (int plugIndex = 0; plugIndex < this->plugArray.Size(); ++plugIndex)
    {
        nEntityObjectId entityId = this->plugArray[plugIndex];
        if (entityId != 0)
        {
            nEntityObject* plugObject = nEntityObjectServer::Instance()->GetEntityObject(entityId);
            n_assert_if(plugObject)
            {
                ncScene* sceneComponent = plugObject->GetComponent<ncScene>();
                if (sceneComponent && sceneComponent->GetRootNode() && sceneComponent->GetFrameId() != this->GetFrameId())
                {
                    sceneComponent->SetFrameId(this->GetFrameId());
                    sceneComponent->SetTime(this->GetTime());
                    sceneComponent->Render(sceneGraph);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Called only at scene render to adjust the transform of attached
    entities to fit the skeleton they're attached to.
*/
void
ncScene::UpdateParentTransform()
{
    n_assert(this->refParentEntity.isvalid());

    ncCharacter* charComp = this->refParentEntity->GetComponent<ncCharacter>();
    if (charComp)
    {
        // CAREFUL!!! if parent attached after attached entity, wrong lod for 1 frame
        ncScene* parentSceneComp = this->refParentEntity->GetComponentSafe<ncScene>();
        int lodIndex = parentSceneComp->GetAttachIndex();

        if (charComp->GetLastUpdatedCharacterIndex() != lodIndex ||
            charComp->GetLastUpdatedTime() < this->GetTime())
        {
            charComp->SetUpdateAttachments(false);
            charComp->UpdateCharacter(lodIndex, this->GetTime());
            charComp->SetUpdateAttachments(true);

            // get the transform from the attach point
            bool fperson = charComp->GetFPersonSkelIndex() == lodIndex;

            // update transform of attached entity
            vector3 pos;
            quaternion quat;
            charComp->GetAttachmentTransforms(this->GetEntityObject(), pos, quat, fperson);

            ncTransform* transform = this->GetComponentSafe<ncTransform>();
            transform->DisableUpdate(ncTransform::cSpatial);
            transform->SetPosition(pos);
            transform->SetQuat(quat);
            transform->EnableUpdate(ncTransform::cSpatial);

            //transform->DoUpdateComponent(ncTransform::cSpatial);
        }
    }
    
    /// @todo - update transform of plugged entities as well ?
}

#ifndef NGAME
//------------------------------------------------------------------------------
void
ncScene::SetLinearFogDistances(float end, float start)
{
    ncDictionary* varContext = this->GetComponent<ncDictionary>();
    if (varContext)
    {
        varContext->SetVectorVariable("FogLinearDistances", vector4(end, start, 0.0f, 0.0f));
    }
}

//------------------------------------------------------------------------------
void
ncScene::GetLinearFogDistances(float& end, float& start)
{
    ncDictionary* varContext = this->GetComponent<ncDictionary>();
    if (varContext)
    {
        const vector4& v = varContext->GetVectorVariable("FogLinearDistances");
        end = v.x;
        start = v.y;
    }
}

//------------------------------------------------------------------------------
void
ncScene::SetLayeredFogDistances(float top, float end, float range)
{
    ncDictionary* varContext = this->GetComponent<ncDictionary>();
    if (varContext)
    {
        varContext->SetVectorVariable("FogLayeredDistances", vector4(top, end, range, 0.0f));
    }
}

//------------------------------------------------------------------------------
void
ncScene::GetLayeredFogDistances(float& top, float& end, float& range)
{
    ncDictionary* varContext = this->GetComponent<ncDictionary>();
    if (varContext)
    {
        const vector4& v = varContext->GetVectorVariable("FogLayeredDistances");
        top = v.x;
        end = v.y;
        range = v.z;
    }
}

//------------------------------------------------------------------------------
void
ncScene::SetWaveFrequency(float frequency)
{
    this->GetShaderOverrides().SetArg(nShaderState::Frequency, nShaderArg(frequency));
}

//------------------------------------------------------------------------------
void
ncScene::GetWaveFrequency(float& frequency)
{
    if (this->GetShaderOverrides().IsParameterValid(nShaderState::Frequency))
        frequency = this->GetShaderOverrides().GetArg(nShaderState::Frequency).GetFloat();
}

//------------------------------------------------------------------------------
void
ncScene::SetWaveAmplitude(float amplitude)
{
    this->GetShaderOverrides().SetArg(nShaderState::Amplitude, nShaderArg(amplitude));
}

//------------------------------------------------------------------------------
void
ncScene::GetWaveAmplitude(float& amplitude)
{
    if (this->GetShaderOverrides().IsParameterValid(nShaderState::Frequency))
        amplitude = this->GetShaderOverrides().GetArg(nShaderState::Amplitude).GetFloat();
}

//------------------------------------------------------------------------------
void
ncScene::SetWaveVelocity(float x, float y)
{
    this->GetShaderOverrides().SetArg(nShaderState::Velocity, nShaderArg(vector4(x, y, 0.0f, 0.0f)));
}


//------------------------------------------------------------------------------
void
ncScene::GetWaveVelocity(float& x, float& y)
{
    if (this->GetShaderOverrides().IsParameterValid(nShaderState::Velocity))
    {     
        x = this->GetShaderOverrides().GetArg(nShaderState::Velocity).GetVector4().x;
        y = this->GetShaderOverrides().GetArg(nShaderState::Velocity).GetVector4().y;
    }
}

//------------------------------------------------------------------------------
void
ncScene::SetTexScale(float x, float y)
{
    this->GetShaderOverrides().SetArg(nShaderState::TexGenS, nShaderArg(vector4(x, y, 0.0f, 0.0f)));
}

//------------------------------------------------------------------------------
void
ncScene::GetTexScale(float& x, float& y)
{
    if (this->GetShaderOverrides().IsParameterValid(nShaderState::Velocity))
    {
        const vector4& texGenS = this->GetShaderOverrides().GetArg(nShaderState::Velocity).GetVector4();
        x = texGenS.x;
        y = texGenS.y;
    }
}

//------------------------------------------------------------------------------
void
ncScene::SetFresnel(float fresnelBias, float fresnelPower)
{
    this->GetShaderOverrides().SetArg(nShaderState::FresnelBias, nShaderArg(fresnelBias));
    this->GetShaderOverrides().SetArg(nShaderState::FresnelPower, nShaderArg(fresnelPower));
}

//------------------------------------------------------------------------------
void
ncScene::GetFresnel(float& fresnelBias, float& fresnelPower)
{
    if (this->GetShaderOverrides().IsParameterValid(nShaderState::FresnelBias))
        fresnelBias = this->GetShaderOverrides().GetArg(nShaderState::FresnelBias).GetFloat();
    if (this->GetShaderOverrides().IsParameterValid(nShaderState::FresnelPower))
        fresnelPower = this->GetShaderOverrides().GetArg(nShaderState::FresnelPower).GetFloat();
}

#endif
