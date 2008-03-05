#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatial.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatial.h"
#include "nspatial/nspatialvisitor.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "nspatial/ncspatialbatch.h"
#include "kernel/npersistserver.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialoccluder.h"
#include "nspatial/ncspatialportal.h"
#include "zombieentity/ncloaderclass.h"
#include "entity/nobjectinstancer.h"
#include "nspatial/ncspatialindoor.h"
#include "entity/nentityobjectserver.h"
#include "nscene/ncscene.h"
#include "zombieentity/nctransform.h"
#include "kernel/ncmdprotonativecpp.h"
#include "zombieentity/ncsubentity.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatial, nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatial)    
    NSCRIPT_ADDCMD_COMPOBJECT('RSBX', void, SetBBox, 6, (float, float, float, float, float, float), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RGBX', void, GetBBox, 0, (), 2, (vector3&, vector3&));    
    NSCRIPT_ADDCMD_COMPOBJECT('RSOB', void, SetOriginalBBox, 6, (float, float, float, float, float, float), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RGOB', void, GetOriginalBBox, 0, (), 2, (vector3&, vector3&));    
    NSCRIPT_ADDCMD_COMPOBJECT('RSCI', void, SetCellId, 1, (int), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RSSI', void, SetSpaceId, 1, (nEntityObjectId), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RSMT', void, SetModelType, 1, (int), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RGMT', int, GetModelType, 0, (), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RSCT', void, SetSpatialCategory, 1, (int), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RGCT', int, GetSpatialCategory, 0, (), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RSSP', void, SetTestSphereProperties, 1, (float), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RSFP', void, SetTestFrustumProperties, 4, (float, float, float, float), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('CSFO', void, SetTestFrustumOrthoProperties, 4, (float,float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSSC', void, SetIsShadowCaster, 1, (bool), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RGSC', bool, GetIsShadowCaster, 0, (), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RGIB', bool, IsBatched, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('R_LB', void, LeaveBatch, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGTG', void, GoToGlobal, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RCBG', void, ComeBackFromGlobal, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RG_G', bool, GetGlobal, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RS_G', void, SetGlobal, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGDT', ncSpatial::DetermineSpaceFlag,  GetDetermineSpaceFlag, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSDT', void, SetDetermineSpaceFlag, 1, (ncSpatial::DetermineSpaceFlag), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('R_RT', void, RemoveTemporary, 0, (), 0, ());    
    NSCRIPT_ADDCMD_COMPOBJECT('RRFT', void, RestoreFromTempArray, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSpatial::ncSpatial():
m_spatialComponentType(0),
m_cell(0),
m_cellId(-1),
m_spaceId(nEntityObjectServer::IDINVALID),
m_octElement(0),
m_indoorSpace(0),
m_transformIndoor(true),
m_testModel(0),
shadowModels(0),
m_shadowCaster(false),
visibleShadows(0),
m_occluders(0),
m_batch(0),
m_determineSpace(ncSpatial::DS_NOT_INIT),
m_frameId(-1),
m_lastCam(nEntityObjectServer::IDINVALID),
currentLod(0),
useOriginalBBox(false),
spatialCat(-1),
associatedToPortal(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSpatial::~ncSpatial()
{
    this->Destroy();

    if (this->m_spatialComponentType != ncSpatial::N_SPATIAL_CAMERA)
    {
        n_verify2( this->RemoveFromSpaces(),
            "miquelangel.rujula: can't remove entity from the spatial spaces where it's inserted!");
    }
}

//------------------------------------------------------------------------------
/**
    destroy the spatial comp
*/
void
ncSpatial::Destroy()
{
    if (this->m_occluders)
    {
        this->m_occluders->Reset();
        n_delete(this->m_occluders);
        this->m_occluders = 0;
    }

    if (this->m_testModel)
    {
        n_delete(this->m_testModel);
        this->m_testModel = 0;
    }

    if (this->shadowModels)
    {
        this->RemoveShadowModels();
        n_delete(this->shadowModels);
        this->shadowModels = 0;
    }

    this->ClearLinks();
}

//------------------------------------------------------------------------------
/**
    clear all the scene links of this entity
*/
void
ncSpatial::ClearLinks()
{
    ncScene *sceneComp = this->GetComponent<ncScene>();
    if ( sceneComp )
    {
        ncSpatialLight *spatialLight;
        int numLinks(sceneComp->GetNumLinks());
        nEntityObject *link;
        for ( int i(0); i < numLinks; ++i )
        {
            link = sceneComp->GetLinkAt(i);
            spatialLight = link->GetComponentSafe<ncSpatialLight>();
            spatialLight->RemoveLinkedEntity(this->GetEntityObject());
        }

        sceneComp->ClearLinks();
    }
}

//------------------------------------------------------------------------------
/**
    recalculate the light links
*/
void 
ncSpatial::CalculateLightLinks()
{
    nSpatialServer::Instance()->GetVisibilityVisitor().CalculateLightLinks(this);
}

//------------------------------------------------------------------------------
/**
    get the cell id the entity is in (just while loading)
*/
int
ncSpatial::GetCellId() const
{
    if ( this->m_cell )
    {
        return this->m_cell->GetId();
    }
    else
    {
        return this->m_cellId;
    }
}

//------------------------------------------------------------------------------
/**
    get the space id the entity is in
*/
nEntityObjectId 
ncSpatial::GetSpaceId() const
{
    if ( this->m_cell )
    {
        n_assert(this->m_cell->GetParentSpace());
        return this->m_cell->GetParentSpace()->GetEntityObject()->GetId();
    }
    else
    {
        return this->m_spaceId;
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    remove the entity from the spaces
*/
void
ncSpatial::DoEnterLimbo()
{
    if (this->IsBatched())
    {
        this->LeaveBatch();
    }
    else
    {
        n_verify( this->RemoveFromSpaces() );
    }

    if (this->m_occluders)
    {
        for (int i = 0; i < this->m_occluders->Size(); ++i)
        {
            this->entityObject->SendToLimbo((*this->m_occluders)[i]);
        }
    }
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    get space the entity is in
*/
ncSpatialSpace *
ncSpatial::GetSpace()
{
    if (this->m_cell)
    {
        return this->m_cell->GetParentSpace();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    remove the entity from the spaces
*/
bool 
ncSpatial::RemoveFromSpaces()
{
    if (!this->IsAssociatedToPortal())
    {
        ncSpatialSpace *space = this->GetSpace();

        if (space)
        {
            bool result(space->RemoveEntity(this->GetEntityObject()));
            if ( result )
            {
                this->Destroy();
            }

            return result;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    says if the entity was visible for the current camera in this frame
*/
bool 
ncSpatial::IsVisibleBy(nEntityObjectId currentCam)
{
    return (this->GetFrameId() == nSpatialServer::Instance()->GetFrameId() &&
            this->GetLastCam() == currentCam);
}

//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncSpatial::InitInstance(nObject::InitInstanceMsg initType)
{
    n_assert2(nSpatialServer::Instance(), "miquelangel.rujula: spatial server not initialized!");
    ncSpatialClass* classComp = this->GetClassComponentSafe<ncSpatialClass>();

    this->SetIsShadowCaster(this->GetClassComponent<ncSpatialClass>()->GetShadowCaster());

    this->spatialCat = nSpatialServer::Instance()->GetCategory(this->GetEntityObject());

    if (initType == nObject::NewInstance)
    {
        bool determineSpace = ( !this->GetEntityObject()->IsA("neindoorbrush") && 
                                !this->GetComponent<ncSpatialPortal>() &&
                                !this->IsBatched() );
        if ( determineSpace )
        {
            this->SetDetermineSpaceFlag(ncSpatial::DS_TRUE);
        }
        else
        {
            this->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
        }
        
        // create default test model, if any
        switch (classComp->GetTestModelType())
        {
            case nSpatialModel::SPATIAL_MODEL_SPHERE:
                this->m_testModel = n_new(nSpatialSphereModel());
                break;

            case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
                this->m_testModel = n_new(nSpatialFrustumModel());
                break;
        }

        // is shadow caster by default
        this->SetIsShadowCaster(classComp->GetShadowCaster());
    }
    else
    {
        // @todo ma.garcias
        // create test model with loaded parameters

        if ( this->m_determineSpace == ncSpatial::DS_NOT_INIT )
        {
            bool determineSpace = ( !this->GetEntityObject()->IsA("neindoorbrush") && 
                                    !this->GetComponent<ncSpatialPortal>() &&
                                    !this->IsBatched() );
            if ( determineSpace )
            {
                this->SetDetermineSpaceFlag(ncSpatial::DS_TRUE);
            }
            else
            {
                this->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
            }
        }
    }

#ifndef NGAME
    if (initType != nObject::ReloadedInstance)
    {
        // Register to EnterLimbo signal
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncSpatial::DoEnterLimbo, 0 );
        // Update the BBox is edit the class
        this->GetEntityClass()->BindSignal( NSIGNAL_OBJECT(nObject, ObjectModified), this, &ncSpatial::ClassChanges, 0 );
    }
#endif

    if (initType != nObject::ReloadedInstance)
    {
        ncTransform* trComp = this->GetComponent<ncTransform>();

        if (trComp)
        {
            if ( this->GetType() == ncSpatial::N_SPATIAL_PORTAL )
            {
                this->UpdateBBox(trComp->GetTransform());
            }
            else
            {
                trComp->DoUpdateComponent(ncTransform::cSpatial);
            }
        }
        else
        {
            // set the original bbox
            this->m_bbox = classComp->GetOriginalBBox();
            this->bboxCenter = this->m_bbox.center();
            this->bboxExtents = this->m_bbox.extents();
        }

        // if this class has occluders, create and store them
        if (classComp->GetHasOccluders())
        {
            // create the occluders array
            this->m_occluders = n_new(nArray<nEntityObject*>(2, 2));

            // load the occluders in the instancer
            ncLoaderClass *loaderClass = classComp->GetComponentSafe<ncLoaderClass>();
            nString fileName = loaderClass->GetResourceFile();
            fileName.Append("/spatial/occluders.n2");
            nObjectInstancer *instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->Load(fileName.Get()));
            n_assert(instancer);
            if (instancer)
            {
                // get matrix information to also transform the occluders
                const matrix44 &mat = this->GetComponent<ncTransform>()->GetTransform();
                vector3 scale;
                quaternion quat;
                vector3 pos;
                mat.get(scale, quat, pos);
                nEntityObject *occluder = 0;
                ncTransform *trComp = 0;
                for (int i = 0; i < instancer->Size(); i++)
                {
                    occluder = static_cast<nEntityObject*>(instancer->At(i));
                    // add the occluder to the array
                    this->m_occluders->Append(occluder);
                    
                    // update the occluder's transform 
                    trComp = this->m_occluders->At(i)->GetComponent<ncTransform>();
                    trComp->DisableUpdate(ncTransform::cSpatial);
                    trComp->SetPosition(pos);
                    trComp->SetScale(scale);
                    trComp->EnableUpdate(ncTransform::cSpatial);
                    trComp->SetQuat(quat);
                }
                instancer->Release();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Update the bbox when change the class
*/
#ifndef NGAME
void 
ncSpatial::ClassChanges(nObject* /*emitter*/)
{
    ncTransform *transform = this->GetComponent<ncTransform>();
    if ( transform )
    {
        this->UpdateBBox( transform->GetTransform() );
    }

    this->SetIsShadowCaster(this->GetClassComponent<ncSpatialClass>()->GetShadowCaster());
}
#endif

//------------------------------------------------------------------------------
/**
    set the entity visible by the given camera
*/
void 
ncSpatial::SetVisibleBy(nEntityObjectId currentCam)
{
    this->SetLastCam(currentCam);
    this->SetFrameId(nSpatialServer::Instance()->GetFrameId());
}

//------------------------------------------------------------------------------
/**
    set the entity as linked to the given light in this frame
*/
void 
ncSpatial::SetLinkedTo(nEntityObjectId currentLight)
{
    this->m_lastLinkLight = currentLight;
    this->m_linkLightFrameId = nSpatialServer::Instance()->GetFrameId();
}

//------------------------------------------------------------------------------
/**
    says if the entity was linked to the given light in this frame
*/
bool 
ncSpatial::IsLinkedTo(nEntityObjectId currentLight)
{
    return (this->m_linkLightFrameId == nSpatialServer::Instance()->GetFrameId() &&
            this->m_lastLinkLight == currentLight);
}

//------------------------------------------------------------------------------
/**
    visibility visitor processing for an entity with a basic spatial component
*/
void
ncSpatial::Accept(nVisibleFrustumVisitor &visitor)
{
    n_assert_return2(this->m_cell,,
              "miquelangel.rujula: trying to accept a spatial component that has no cell!");
    
    this->m_cell->AppendVisEntity(this->GetEntityObject());
    visitor.Visit(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
    visibility visitor processing for an entity with a basic spatial component
*/
void
ncSpatial::Accept(nVisibleFrustumVisitor &visitor, ncSpatialCell *cell)
{
    n_assert_return(cell, );

    cell->AppendVisEntity(this->GetEntityObject());
    visitor.Visit(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
    update entity's transformation 
*/
void 
ncSpatial::Update(const matrix44 &worldMatrix)
{
#ifndef NGAME
    ncSpatialIndoor *indoor = this->GetIndoorSpace();
    if (indoor && this->m_transformIndoor)
    {
        // it means that this entity is a 'facade'. Move its correponding 'indoor'
        indoor->SetTransformFacade(false);
        indoor->Update(worldMatrix);
        indoor->SetTransformFacade(true);
    }
#endif // !NGAME

    nSpatialServer::Instance()->GetSpatialVisitor().Visit(this, worldMatrix);

    // if this entity has occluders, move them too
    if (this->m_occluders)
    {
        // get matrix information to also transform the occluders
        vector3 scale;
        quaternion quat;
        vector3 pos;
        worldMatrix.get(scale, quat, pos);

        ncTransform *trComp = 0;
        for (int i = 0; i < this->m_occluders->Size(); ++i)
        {
            trComp = this->m_occluders->At(i)->GetComponent<ncTransform>();
            trComp->DisableUpdate(ncTransform::cSpatial);
            trComp->SetPosition(pos);
            trComp->SetScale(scale);
            trComp->EnableUpdate(ncTransform::cSpatial);
            trComp->SetQuat(quat);
        }
    }
}

//------------------------------------------------------------------------------
/**
    get instance's original bbox
*/
const bbox3&
ncSpatial::GetOriginalBBox()
{
    ncSpatialClass* spatialClass = this->GetClassComponent<ncSpatialClass>();

    if (this->useOriginalBBox || !spatialClass)
    {
        return this->m_originalBBox;
    }
    
    return spatialClass->GetOriginalBBox();
}

//------------------------------------------------------------------------------
/**
    returns the center and the extents of the original bounding box
*/
void
ncSpatial::GetOriginalBBox(vector3& v0, vector3& v1)
{
    ncSpatialClass* spatialClass = this->GetClassComponent<ncSpatialClass>();

    if (this->useOriginalBBox || !spatialClass)
    {
        v0 = this->m_originalBBox.center();
        v1 = this->m_originalBBox.extents();
    }
    else
    {
        v0 = spatialClass->GetOriginalBBox().center();
        v1 = spatialClass->GetOriginalBBox().extents();
    }
}

//------------------------------------------------------------------------------
/**
    Apply a transformation in world coordinates to the original bounding box and
    update the current component's bounding box.

    If the component's entity is inserted in any space it passes the bounding box
    to that space coordinates. But if it's not inserted yet, the resulting 
    bounding box will remain in world coordinates.
*/
void
ncSpatial::UpdateBBox(const matrix44 &worldMatrix)
{
    this->m_bbox = this->GetOriginalBBox();

    this->m_bbox.transform(worldMatrix);

    if (this->refBatch.isvalid())
    {
        this->m_batch->UpdateBBox();
    }

    this->bboxCenter = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();
}

//------------------------------------------------------------------------------
/**
    set the type of test model and create it
*/
void
ncSpatial::SetModelType(int type)
{
    nSpatialModel *spatialModel = 0;
    switch (type)
    {
        case nSpatialModel::SPATIAL_MODEL_SPHERE:
            spatialModel = n_new(nSpatialSphereModel);
            break;

        case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
            spatialModel = n_new(nSpatialFrustumModel);
            break;

        default:
            break;
    }

    this->SetTestModel(spatialModel);
}

//------------------------------------------------------------------------------
/**
    get the type of test model
*/
int
ncSpatial::GetModelType() const
{
    if (this->m_testModel)
    {
        return this->m_testModel->GetType();
    }
    else
    {
        return nSpatialModel::SPATIAL_MODEL_NONE;
    }
}

//------------------------------------------------------------------------------
/**
    set test model
*/
void
ncSpatial::SetTestModel(nSpatialModel *testModel)
{
    if (this->m_testModel)
    {
        n_delete(this->m_testModel);
    }
    this->m_testModel = testModel;
}

//------------------------------------------------------------------------------
/**
    find if there's a shadow generated for the given light id
*/
nSpatialShadowModel *
ncSpatial::FindShadowFor(nEntityObjectId lightId)
{
    if ( this->shadowModels )
    {
        for ( int i(0); i < this->shadowModels->Size(); ++i )
        {
            if ( (*this->shadowModels)[i]->GetLightId() == lightId )
            {
                return (*this->shadowModels)[i];
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    add a shadow model
*/
void 
ncSpatial::AddShadowModel(nSpatialShadowModel *model)
{
    n_assert(model);
    n_assert(this->GetIsShadowCaster());
    n_assert(model->GetLightId() != nEntityObjectServer::IDINVALID);
    n_assert(this->shadowModels);
    n_assert(!this->FindShadowFor(model->GetLightId()));

    this->shadowModels->Append(model);

    // set this entity as the one that casted the shadow
    model->SetCaster(this);
}

//------------------------------------------------------------------------------
/**
    remove a shadow model
*/
void 
ncSpatial::RemoveShadowModel(nSpatialShadowModel *model)
{
    n_assert(model);
    n_assert(model->GetLightId() != nEntityObjectServer::IDINVALID);

    if ( !this->shadowModels )
    {
        return;
    }

    int index(this->shadowModels->FindIndex(model));
    n_assert(index != -1);
    this->shadowModels->EraseQuick(index);
    n_delete(model);
}

//------------------------------------------------------------------------------
/**
    get the shadow models array
*/
const nArray<nSpatialShadowModel*> *
ncSpatial::GetShadowModels() const
{
    return this->shadowModels;
}

//------------------------------------------------------------------------------
/**
    remove all the shadow models
*/
void 
ncSpatial::RemoveShadowModels()
{
    if ( this->shadowModels )
    {
        for ( int i(0); i < this->shadowModels->Size(); ++i )
        {
            n_delete((*this->shadowModels)[i]);
        }

        this->shadowModels->Reset();
    }
}

//------------------------------------------------------------------------------
/**
    update the shadow model for the given light
*/
nSpatialShadowModel * 
ncSpatial::UpdateShadowModel(ncSpatialLight* light)
{
    n_assert(this->GetIsShadowCaster());

    // calculate cylinder radius
    float radius(this->m_bbox.diagonal_size() * 0.5f);

    // cylinder's p1 will be bounding box's center
    const vector3 &p1 = this->bboxCenter;

    // calculate cylinder's p2. It will be in the light - entity direction
    ncTransform* lightTr = light->GetComponent<ncTransform>();
    vector3 dir( lightTr->GetPosition() - p1 );
    dir.norm();
    dir *= this->GetClassComponentSafe<ncSpatialClass>()->GetShadowLength();
    vector3 p2(p1 - dir);

    // set shadow model's new properties
    nEntityObjectId lightId = light->GetEntityObject()->GetId();
    if ( !this->shadowModels )
    {
        this->shadowModels = n_new(nArray<nSpatialShadowModel*>(2,2));
    }

    nSpatialShadowModel *model = this->FindShadowFor(lightId);
    if ( model )
    {
        model->SetCylinder(p1, p2, radius);
    }
    else
    {
        model = n_new(nSpatialShadowModel(p1, p2, radius));
        n_assert(model->GetLightId() == nEntityObjectServer::IDINVALID);
        model->SetLightId(lightId);
        this->AddShadowModel(model);
    }

    return model;
}

//------------------------------------------------------------------------------
/**
    set the sphere's radius and position (catch from the transform component)
*/
void
ncSpatial::SetTestSphereProperties(float radius)
{
    if (this->m_testModel && (this->m_testModel->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE))
    {
        nSpatialSphereModel *sphereModel = static_cast<nSpatialSphereModel*>(this->m_testModel);
        ncTransform* trComp = this->GetComponentSafe<ncTransform>();
        sphereModel->Set(trComp->GetPosition(), radius);
    }
}

//------------------------------------------------------------------------------
/**
    set the frustum test model's properties
    backward persistence compatibility
*/
void
ncSpatial::SetTestFrustumProperties(float aov, float aspect, float nearp, float farp)
{
    if (this->m_testModel && (this->m_testModel->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM))
    {
        nSpatialFrustumModel *frustumModel = static_cast<nSpatialFrustumModel*>(this->m_testModel);
        ncTransform* trComp = this->GetComponentSafe<ncTransform>();

        frustumModel->SetCamera(nCamera2(aov, aspect, nearp, farp));
        //This line is not compatible with ncSpatialLight
        frustumModel->SetTransform(trComp->GetTransform());
    }
}
//------------------------------------------------------------------------------
/**
    set the frustum test model's properties
*/
void
ncSpatial::SetTestFrustumOrthoProperties(float width, float height, float nearp, float farp)
{
    if (this->m_testModel && (this->m_testModel->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM))
    {
        nSpatialFrustumModel *frustumModel = static_cast<nSpatialFrustumModel*>(this->m_testModel);
        ncTransform* trComp = this->GetComponentSafe<ncTransform>();
        nCamera2 camera;
        camera.SetOrthogonal( width, height, nearp, farp);
        frustumModel->SetCamera(camera);
        //This line is not compatible with ncSpatialLight
        frustumModel->SetTransform(trComp->GetTransform());
    }
}

//------------------------------------------------------------------------------
/**
    set the frame id that the entity was visible for the last time
*/
void 
ncSpatial::SetFrameId(int id)
{
    this->m_frameId = id;
}

//------------------------------------------------------------------------------
/**
    set the batch's spatial component this entity belongs to
*/
void 
ncSpatial::SetBatch(ncSpatialBatch* batch)
{
    this->m_batch = batch;
    if (batch)
    {
        this->refBatch = batch->GetEntityObject();
        this->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
    }
    else
    {
        this->refBatch.invalidate();
        this->SetDetermineSpaceFlag(ncSpatial::DS_TRUE);
    }
}

//------------------------------------------------------------------------------
/**
    leave batch
*/
void 
ncSpatial::LeaveBatch()
{
    if (this->refBatch.isvalid())
    {
        this->m_batch->RemoveSubentity(this->GetEntityObject());
        
        bool determineSpace = ( !this->GetEntityObject()->IsA("neindoorbrush") && 
                                !this->GetComponent<ncSpatialPortal>());

        if ( determineSpace )
        {
            this->SetDetermineSpaceFlag(ncSpatial::DS_TRUE);
        }
        else
        {
            this->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
        }
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    remove temporary from the cell this entity is in
*/
void 
ncSpatial::RemoveTemporary()
{
    if (this->m_cell)
    {
        this->m_cell->RemoveTempEntity(this->GetEntityObject());
    }
}

//------------------------------------------------------------------------------
/**
    restore this entity from the temporary array of the cell is in
*/
void 
ncSpatial::RestoreFromTempArray()
{
    if (this->m_cell)
    {
        this->m_cell->RestoreTempEntity(this->GetEntityObject());
    }
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    send the entity to the global space
*/
void 
ncSpatial::GoToGlobal()
{
    if ( this->m_cell )
    {
        n_verify(this->m_cell->RemoveEntity(this->GetEntityObject()));
        this->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
        nSpatialServer::Instance()->InsertGlobalEntity(this->GetEntityObject());
    }
}

//------------------------------------------------------------------------------
/**
    com back from global space
*/
void 
ncSpatial::ComeBackFromGlobal()
{
    n_assert( this->m_cell );
    
    n_verify(this->m_cell->RemoveEntity(this->GetEntityObject()));
    this->SetDetermineSpaceFlag(ncSpatial::DS_TRUE);
    nSpatialServer::Instance()->InsertEntity(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
    set as global
*/
void 
ncSpatial::SetGlobal(bool gl)
{
    if ( gl && !this->GetGlobal() )
    {
        this->GoToGlobal();
    }
    else if ( !gl && this->GetGlobal() )
    {
        this->ComeBackFromGlobal();
    }
}

//------------------------------------------------------------------------------
/**
    get if is a global
*/
bool 
ncSpatial::GetGlobal() const
{
    return this->m_cell && (this->m_cell->GetType() == ncSpatialCell::N_GLOBAL_CELL);
}

//------------------------------------------------------------------------------
/**
    set if it's a shadow caster
*/
void 
ncSpatial::SetIsShadowCaster(bool value)
{
    this->m_shadowCaster = value;

    if ( !this->m_shadowCaster && this->shadowModels )
    {
        this->RemoveShadowModels();
        n_delete(this->shadowModels);
        this->shadowModels = 0;
    }
}

//------------------------------------------------------------------------------
/**
    copy the necessary information from another entity
*/
void 
ncSpatial::operator=(const ncSpatial &sc)
{
    this->m_spatialComponentType = sc.m_spatialComponentType;
    this->m_bbox = sc.m_bbox;
    this->m_cell = sc.m_cell;
    this->bboxCenter = sc.bboxCenter;
    this->bboxExtents = sc.bboxExtents;
    this->m_octElement = sc.m_octElement;
    this->m_indoorSpace = sc.m_indoorSpace;

    // set the test model
    nSpatialModel *newModel = 0;
    if ( sc.m_testModel )
    {
        switch( sc.m_testModel->GetType() )
        {
            case nSpatialModel::SPATIAL_MODEL_SPHERE:
                newModel = n_new(nSpatialSphereModel(static_cast<nSpatialSphereModel*>(sc.m_testModel)->GetSphere()));
                break;

            case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
                {
                    nSpatialFrustumModel *frustumModel = static_cast<nSpatialFrustumModel*>(sc.m_testModel);
                    newModel = n_new(nSpatialFrustumModel(frustumModel->GetCamera(), frustumModel->GetTransform()));
                }
                break;

            default:
                break;
        }
    }

    if (this->m_testModel)
    {
        n_delete(this->m_testModel);
    }
    this->m_testModel = newModel;


    // set the shadow models
    nArray<nSpatialShadowModel*> *newShadowModels = 0;
    if ( sc.shadowModels )
    {
        newShadowModels = n_new(nArray<nSpatialShadowModel*>(2, 2));
        newShadowModels = sc.shadowModels;
    }

    if ( this->shadowModels )
    {
        this->RemoveShadowModels();
        n_delete(this->shadowModels);
    }
    this->shadowModels = newShadowModels;
    
    this->m_cellId = sc.m_cellId;
    this->m_determineSpace = sc.m_determineSpace;
    this->m_frameId = sc.m_frameId;
    this->m_lastCam = sc.m_lastCam;
    this->m_originalBBox = sc.m_originalBBox;
    this->m_spaceId = sc.m_spaceId;
    this->SetIsShadowCaster(sc.m_shadowCaster);
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatial::SaveCmds(nPersistServer *ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        ncSubentity * subentity = this->GetComponent<ncSubentity>();
        bool isSubentity = subentity && subentity->IsSubentity();

        if( ! isSubentity )
        {
            if (nEntityObjectServer::Instance()->GetDataVersion() == 0)
            {
                // --- setbbox ---
                vector3 v0, v1;
                this->GetBBox(v0, v1);
                if (!ps->Put(this->GetEntityObject(), 'RSBX', v0.x, v0.y, v0.z, v1.x, v1.y, v1.z))
                {
                    return false;
                }
            }
        }

        //--- setdeterminespace ---
        if (!ps->Put(this->GetEntityObject(), 'RSDT', this->GetDetermineSpaceFlag()))
        {
            return false;
        }
        
        if (this->useOriginalBBox)
        {
            //--- setoriginalbbox ---
            vector3 v0, v1;
            this->GetOriginalBBox(v0, v1);
            if (!ps->Put(this->GetEntityObject(), 'RSOB', v0.x, v0.y, v0.z, v1.x, v1.y, v1.z))
            {
                return false;
            }
        }
        
        if (this->m_testModel)
        {
            nSpatialSphereModel *sphereModel = 0;
            nSpatialFrustumModel *frustumModel = 0;
            switch (this->m_testModel->GetType())
            {
                case nSpatialModel::SPATIAL_MODEL_SPHERE:
                    // --- setmodeltype ---
                    if (!ps->Put(this->GetEntityObject(), 'RSMT', nSpatialModel::SPATIAL_MODEL_SPHERE))
                    {
                        return false;
                    }

                    // --- settestsphereproperties ---
                    sphereModel = static_cast<nSpatialSphereModel*>(this->m_testModel);
                    if (!ps->Put(this->GetEntityObject(), 'RSSP', sphereModel->GetSphere().r))
                    {
                        return false;
                    }
                    break;

                case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
                    // --- setmodeltype ---
                    if (!ps->Put(this->GetEntityObject(), 'RSMT', nSpatialModel::SPATIAL_MODEL_FRUSTUM))
                    {
                        return false;
                    }
                    
                    // --- settestfrustumproperties ---
                    frustumModel = static_cast<nSpatialFrustumModel*>(this->m_testModel);
                    {
                        if ( frustumModel->GetCamera().GetType() == nCamera2::Perspective )
                        {                               
                            if (!ps->Put(this->GetEntityObject(), 'RSFP', 
                                frustumModel->GetCamera().GetAngleOfView(),
                                frustumModel->GetCamera().GetAspectRatio(),
                                frustumModel->GetCamera().GetNearPlane(),
                                frustumModel->GetCamera().GetFarPlane()))
                            {
                                return false;
                            }
                        } else
                        {
                            if (!ps->Put(this->GetEntityObject(), 'CSFO', 
                                frustumModel->GetCamera().GetWidth(),
                                frustumModel->GetCamera().GetHeight(),
                                frustumModel->GetCamera().GetNearPlane(),
                                frustumModel->GetCamera().GetFarPlane()))
                            {
                                return false;
                            }
                        }
                    }

                    break;

                default:
                    break;
            }
        }


        if( ! isSubentity )
        {
            //FIXME miquelangel.rujula -find some other way to do this
            if (!this->GetEntityObject()->IsA("nernswaypoint"))
            {
                int cellId = -1;
                int spaceId = -1;
                if (this->m_cell)
                {
                    cellId = this->m_cell->GetId();
                    if (this->m_cell->GetParentSpace() && this->m_cell->GetParentSpace()->GetEntityObject())
                    {
                        spaceId = this->m_cell->GetParentSpace()->GetEntityObject()->GetId();
                    }
                }

                //--- setcellid ---
                if (!ps->Put(this->GetEntityObject(), 'RSCI', cellId))
                {
                    return false;
                }

                //--- setspaceid ---
                if (!ps->Put(this->GetEntityObject(), 'RSSI', spaceId))
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}
