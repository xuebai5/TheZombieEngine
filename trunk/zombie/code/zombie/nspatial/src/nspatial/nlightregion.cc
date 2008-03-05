#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nlightregion.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/nlightregion.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/nspatialmodels.h"
#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
/**
    Destructor
*/
nLightRegion::~nLightRegion()
{
    this->RemoveFromCells();

    if ( this->parentLight )
    {
        this->parentLight->RemoveLightRegion(this); 
        this->parentLight = 0;
    }

    if ( this->frustumClipper )
    {
        n_delete(this->frustumClipper);
    }
}

//------------------------------------------------------------------------------
/**
    Create the light's frustum clipper
*/
void 
nLightRegion::CreateFrustumClipper(nCamera2 &camera, const matrix44 &viewMatrix)
{
    if ( this->frustumClipper )
    {
        n_delete(this->frustumClipper);
    }

    this->frustumClipper = n_new(nFrustumClipper(camera, viewMatrix));
}

//------------------------------------------------------------------------------
/**
    Set an already built frustum clipper
*/
void 
nLightRegion::SetClipper(const nFrustumClipper &newClipper)
{
    if ( this->frustumClipper )
    {
        n_delete(this->frustumClipper);
    }

    this->frustumClipper = n_new(nFrustumClipper(newClipper));
}

//------------------------------------------------------------------------------
/**
    Autoremove from a cell
*/
void
nLightRegion::RemoveFromCell(ncSpatialCell *cell)
{
    if ( !this->cells.Empty() )
    {
        // remove the cell from the cells array
        int cellIndex(this->cells.FindIndex(cell));
        n_assert(cellIndex != -1);
        this->cells.EraseQuick(cellIndex);

        // remove the region from the cell
        nArray<nLightRegion*> &cellRegions = cell->GetLightRegions();
        int regIndex(cellRegions.FindIndex(this));
        n_assert( regIndex != -1 );

        cellRegions.EraseQuick(regIndex);
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Draw the light region, for debug
*/
void 
nLightRegion::Draw()
{
    if ( this->frustumClipper && this->prevLightRegion )
    {
        static vector4 color;
        nSpatialModel *model = this->GetParentLight()->GetTestModel();
        if ( !model )
        {
            // put omni color
            color.set(1.f, 1.f, 0.f, 1.f);
        }
        else
        {
            // put not omni color
            if ( model->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM )
            {
                color.set(0.f, 1.f, 0.2f, 1.f);
            }
            else
            {
                color.set(0.f, 1.f, 1.f, 1.f);
            }
        }

        this->frustumClipper->VisualizePlanesSet(color);
    }
#if 0
    else
    {
        nGfxServer2 *gfxServer = nGfxServer2::Instance();
        nGfxServer2::ShapeType shapeType = nGfxServer2::Box;
        matrix44 m;
        n_assert(this->parentLight);
        nSpatialModel *testModel = this->parentLight->GetTestModel();
        if ( testModel )
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
                    frustumClipper.VisualizeFrustum(nGfxServer2::Instance(), vector4(1.f, 0.f, 0.f, 1.f));
                    return;
                }
            }
        }
        else
        {
            // if there is no test model, draw the box shape for the light
            shapeType = nGfxServer2::Box;
            n_assert(this->parentLight);
            const vector3& nodeCenter(this->parentLight->GetComponentSafe<ncTransform>()->GetPosition());
            m.scale(vector3(1.f, 1.f, 1.f));
            m.set_translation(nodeCenter);
        }

        gfxServer->BeginShapes();
        gfxServer->DrawShape(shapeType, m, vector4(1.f, 0.f, 0.f, 0.5f));
        gfxServer->EndShapes();
    }
#endif 
}
#endif // !NGAME
