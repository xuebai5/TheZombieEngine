#include "precompiled/pchnspatial.h"
#include "nspatial/nspatiallight.h"

//------------------------------------------------------------------------------
/**
    updates its model's transformation
*/
void 
nSpatialLight::SetModelMatrix(const matrix44 &matrix)
{
    //nSpatialElement::SetTransformMatrix(matrix);

    // set model's transformation matrix
    nSpatialModel *model = this->m_testModel;
    nSpatialSphereModel *sphereModel;

//    nSpatialFrustumModel *frustumModel;

    switch(model->GetType())
    {
        case nSpatialModel::SPATIAL_MODEL_SPHERE:
            sphereModel = (nSpatialSphereModel*) model;
            sphereModel->Set(matrix.pos_component());
            break;

        case nSpatialModel::SPATIAL_MODEL_FRUSTUM:
            //frustumModel = (nSpatialFrustumModel*) model;
            //frustumModel->SetTransformMatrix(matrix);
            break;

        default:
            break;
    }
}

