#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatiallight.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatiallight.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/nspatialvisitor.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "zombieentity/nctransform.h"
#include "nspatial/nlightregion.h"
#include "nspatial/ncspatialclass.h"

#ifndef NGAME
#include "nspatial/nspatialmodels.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialLight, ncSpatial);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialLight)
    NSCRIPT_ADDCMD_COMPOBJECT('MSCR', void, SetAffectsOnlyACell, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGCR', bool, AffectsOnlyACell, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSAO', void, SetAffectsOtherSpaceTypes, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGAO', bool, AffectsOtherSpaceTypes, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSCS', void, SetCastShadows, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGCS', bool, GetCastShadows, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RTON', void, TurnOn, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RTOF', void, TurnOff, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RILO', bool, IsOn, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('MSLO', void, SetLightOn, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGLO', bool, GetLightOn, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSLU', void, SetLightRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGLU', float, GetLightRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSLM', void, SetLightFrustum, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGLM', void, GetLightFrustum, 0, (), 4, (float&, float&, float&, float&));
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncSpatialLight::ncSpatialLight() : 
ncSpatial(),
affectsOnlyACell(false),
affectsOtherSpaceTypes(true),
castShadows(false),
isOn(true),
zOffset(0.f)
{
    this->m_spatialComponentType = ncSpatial::N_SPATIAL_LIGHT;
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSpatialLight::~ncSpatialLight()
{
    this->Destroy();
}

//------------------------------------------------------------------------------
/**
    destroy the spatial comp
*/
void
ncSpatialLight::Destroy()
{
    // destroy all the regions of this light
    this->DestroyLightRegions();
}

//------------------------------------------------------------------------------
/**
    Initialize the component
*/
void 
ncSpatialLight::InitInstance(nObject::InitInstanceMsg initType)
{
    ncSpatial::InitInstance(initType);

    if (initType == nObject::NewInstance)
    {
        ncSpatialClass* classComp = this->GetClassComponentSafe<ncSpatialClass>();
        if ( classComp->GetTestModelType() ==  nSpatialModel::SPATIAL_MODEL_FRUSTUM )
        {
            // Is very important the frustrum is in a perfect unit cube.
            // create default test model, if any
            nSpatialFrustumModel* frustrum = static_cast<nSpatialFrustumModel*>(this->m_testModel);
            frustrum->SetCamera(nCamera2(90.f, 1.f, 0.00001f, 0.5f));
        }
    }

    // flush the light
    this->Flush();
}

//------------------------------------------------------------------------------
/**
    makes the light to reset
*/
void
ncSpatialLight::Flush()
{
    ncTransform *trComp = this->GetComponent<ncTransform>();
    if ( trComp )
    {
        trComp->DoUpdateComponent(ncTransform::cSpatial);
    }
}

//------------------------------------------------------------------------------
/**
    destroy all the regions of this light
*/
void
ncSpatialLight::DestroyLightRegions()
{
    while ( !this->lightRegions.Empty() )
    {
        n_delete(this->lightRegions.Back());
    }

    n_assert(this->lightRegions.Empty());

    this->UnlinkFromEntities();
}

//------------------------------------------------------------------------------
/**
    unlink from the entities
*/
void 
ncSpatialLight::UnlinkFromEntities()
{
    ncScene *sceneComp;
    ncSpatial *spatialComp;
    nEntityObject *lightEntity = this->GetEntityObject();
    for ( int i(0); i < this->linkedEntities.Size(); ++i )
    {
        // remove the link from the entity
        sceneComp = this->linkedEntities[i]->GetComponentSafe<ncScene>();
        sceneComp->RemoveLink(lightEntity);
    }

    if ( this->GetCastShadows() )
    {
        nSpatialShadowModel *model;
        for ( int i(0); i < this->linkedEntities.Size(); ++i )
        {
            // destroy the shadow if the entity is a shadow caster
            spatialComp = this->linkedEntities[i]->GetComponent<ncSpatial>();
            if ( spatialComp && spatialComp->GetIsShadowCaster() )
            {
                model = spatialComp->FindShadowFor(lightEntity->GetId());
                if ( model )
                {
                    spatialComp->RemoveShadowModel(model);
                    spatialComp->GetComponentSafe<ncScene>()->SetFlag(ncScene::ShadowVisible, false);
                }
            }
        }
    }

    this->linkedEntities.Reset();
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    draw the light's roi
*/
void 
ncSpatialLight::DrawROI( nShader2* shapeShader )
{
    matrix44 m;
    m.ident();
    nGfxServer2::ShapeType shapeType = nGfxServer2::Box;

    nSpatialModel* testModel = this->GetTestModel();
    if (testModel)
    {
        switch (testModel->GetType())
        {
            case nSpatialModel::SPATIAL_MODEL_SPHERE:
            {
                shapeType = nGfxServer2::Sphere;
                const sphere& s = static_cast<nSpatialSphereModel*>(testModel)->GetSphere();
                m.scale(vector3(s.r, s.r, s.r));
                m.set_translation(s.p);
                break;
            }

            case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
            {
                nSpatialFrustumModel* frustumModel = static_cast<nSpatialFrustumModel*>(testModel);
                nFrustumClipper &frustumClipper = frustumModel->GetFrustum();
                frustumClipper.VisualizeFrustum(nGfxServer2::Instance(), vector4(0.f, 1.f, 0.2f, 1.f));
                return;
            }
        }
    }
    else
    {
        // if there is no test model, draw the box shape for the light
        shapeType = nGfxServer2::Box;
        const bbox3& box = this->GetBBox();
        const vector3& nodeCenter(box.center());
        m.scale(vector3(1.f, 1.f, 1.f));
        m.set_translation(nodeCenter);
    }

    nGfxServer2::Instance()->BeginShapes(shapeShader);
    nGfxServer2::Instance()->DrawShape(shapeType, m, vector4(0.f, 1.f, 0.f, 0.35f));
    nGfxServer2::Instance()->EndShapes();
}
#endif 

//------------------------------------------------------------------------------
/**
    updates its model's transformation
*/
void
ncSpatialLight::Update(const matrix44 &matrix)
{
    nSpatialServer::Instance()->GetSpatialVisitor().Visit(this, matrix);

    if (this->m_testModel)
    {
        // update transform in the test model
        nSpatialSphereModel *sphereModel;
        nSpatialFrustumModel *frustumModel;

        switch (this->m_testModel->GetType())
        {
            case nSpatialModel::SPATIAL_MODEL_SPHERE:
                sphereModel = static_cast<nSpatialSphereModel*>(this->m_testModel);
                sphereModel->Set(matrix.pos_component());
                break;

            case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
                frustumModel = static_cast<nSpatialFrustumModel*>(this->m_testModel);
                if ( this->zOffset < N_TINY )
                {
                    frustumModel->SetTransform(matrix);
                } else
                {
                    matrix44 translate; // move back the camera
                    translate.translate(vector3( 0, 0, this->zOffset ));
                    frustumModel->SetTransform(translate * matrix);
                }
               
                break;

            default:
                break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    set the range of the light: it only affects a cell?
*/
void 
ncSpatialLight::SetAffectsOnlyACell(bool affectsOnlyACell)
{
    this->affectsOnlyACell = affectsOnlyACell;
    this->Flush();
}

//------------------------------------------------------------------------------
/**
    adds a light region to this cell
*/
void 
ncSpatialLight::AddLightRegion(nLightRegion *lightRegion)
{
    n_assert(this->lightRegions.FindIndex(lightRegion) == -1);

    this->lightRegions.Append(lightRegion);
    n_assert(!lightRegion->GetParentLight());
    lightRegion->SetParentLight(this);
}

//------------------------------------------------------------------------------
/**
    remove a light region from this cell
*/
void 
ncSpatialLight::RemoveLightRegion(nLightRegion *lightRegion)
{
    int index(this->lightRegions.FindIndex(lightRegion));
    if ( index != -1 )
    {
        this->lightRegions.EraseQuick(index);
    }
}

//------------------------------------------------------------------------------
/**
    turn on the light
*/
void 
ncSpatialLight::TurnOn()
{
    this->isOn = true;
    this->Flush();
}

//------------------------------------------------------------------------------
/**
    Visibility visitor processing for a basic light.
*/
void 
ncSpatialLight::Accept(nVisibleFrustumVisitor &visitor)
{
    this->SetFrameId(nSpatialServer::Instance()->GetFrameId());
    visitor.Visit(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
    actually, lights, are ignored for spatial queries
*/
void 
ncSpatialLight::Accept(nSpatialVisitor &/*visitor*/)
{
    // Empty
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLight::SetLightOn(bool isOn)
{
    if (isOn)
    {
        this->TurnOn();
    }
    else
    {
        this->TurnOff();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialLight::GetLightOn()
{
    return this->IsOn();
}

//------------------------------------------------------------------------------
/**
    enter limbo
*/
void
ncSpatialLight::DoEnterLimbo()
{
    ncSpatial::DoEnterLimbo();
    this->DestroyLightRegions();
    this->RemoveShadowModels();
}

#endif

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLight::SetLightRadius(float radius)
{
    nSpatialModel* currentModel = this->GetTestModel();
    if (currentModel && (currentModel->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE))
    {
        static_cast<nSpatialSphereModel*>(currentModel)->SetRadius(radius);

        // update the light regions
        const matrix44 &m = this->GetComponentSafe<ncTransform>()->GetTransform();
        nSpatialServer::Instance()->GetSpatialVisitor().Visit(this, m);
    }
}

//------------------------------------------------------------------------------
/**
*/
float
ncSpatialLight::GetLightRadius()
{
    nSpatialModel* currentModel = this->GetTestModel();
    if (currentModel && (currentModel->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE) )
    {
        return static_cast<nSpatialSphereModel*>(currentModel)->GetRadius();
    }
    return 0.f;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLight::SetLightFrustum(float aov, float aspect, float nearp, float farp)
{
    nSpatialModel* currentModel = this->GetTestModel();
    if (currentModel && (currentModel->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM))
    {
        nSpatialFrustumModel* frustumModel = static_cast<nSpatialFrustumModel*>(this->m_testModel);
        frustumModel->SetCamera(nCamera2(aov, aspect, nearp, farp));

        // update the light regions
        const matrix44 &m = this->GetComponentSafe<ncTransform>()->GetTransform();
        nSpatialServer::Instance()->GetSpatialVisitor().Visit(this, m);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLight::GetLightFrustum(float& aov, float& aspect, float& nearp, float& farp)
{
    nSpatialModel* currentModel = this->GetTestModel();
    if (currentModel && (currentModel->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM))
    {
        nSpatialFrustumModel* frustumModel = static_cast<nSpatialFrustumModel*>(this->m_testModel);
        
        aov    = frustumModel->GetCamera().GetAngleOfView();
        aspect = frustumModel->GetCamera().GetAspectRatio();
        nearp  = frustumModel->GetCamera().GetNearPlane();
        farp   = frustumModel->GetCamera().GetFarPlane();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialLight::SaveCmds(nPersistServer * ps)
{
    if (ncSpatial::SaveCmds(ps))
    {
        // --- setaffectsonlyonecell ---
        if (!ps->Put(this->GetEntityObject(), 'MSCR', this->AffectsOnlyACell()))
        {
            return false;
        }

        // --- setaffectsotherspacetypes ---
        if (!ps->Put(this->GetEntityObject(), 'RSAO', this->AffectsOtherSpaceTypes()))
        {
            return false;
        }

        // --- setcastshadows ---
        if (!ps->Put(this->GetEntityObject(), 'RSCS', this->GetCastShadows()))
        {
            return false;
        }

        if (this->IsOn())
        {
            // --- turnon ---
            if (!ps->Put(this->GetEntityObject(), 'RTON'))
            {
                return false;
            }
        }
        else
        {
            // --- turnoff ---
            if (!ps->Put(this->GetEntityObject(), 'RTOF'))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialLight::SetZOffset(float val)
{
    this->zOffset = val;
}

//------------------------------------------------------------------------------
/**
*/
float
ncSpatialLight::GetZOffset() const
{
    return this->zOffset;
}
