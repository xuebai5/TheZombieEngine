//------------------------------------------------------------------------------
//  ncscenedotlaser_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsscene.h"
#include "ndotlaser/ncscenedotlaser.h"
#include "ndotlaser/ncscenedotlaserclass.h"

#include "nphysics/ncphysicsobj.h"
#include "nphysics/nphysicsgeom.h"
#include "nscene/nscenegraph.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/nspatialmodels.h"

const float MAX_RAY_LENGTH = 50.f;


//------------------------------------------------------------------------------
/**
*/
ncSceneDotLaser::ncSceneDotLaser() :
    entityReceiver(0),
    allAreCandidates(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneDotLaser::~ncSceneDotLaser()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneDotLaser::InitInstance(nObject::InitInstanceMsg initType)
{
    n_assert2(this->GetEntityObject()->GetComponent<ncScene>() , "The ncSceneDotLaser always need ncScene component" )
    ncSceneLight::InitInstance(initType);
    switch (initType)
    {
        case nObject::NewInstance: 
        case nObject::LoadedInstance:
        case nObject::ReloadedInstance:
            this->UpdateLaserProjection();
            break;
        default:
            break;
    }

#ifndef NGAME
    this->GetEntityClass()->BindSignal( nObject::SignalObjectModified, this, &ncSceneDotLaser::TriggerClassModified,10 );
#endif NGAME
}

//------------------------------------------------------------------------------
/**
    Calculate the laser colision point and search the receiver entity
*/
void 
ncSceneDotLaser::Render(nSceneGraph* sceneGraph)
{
    //Dete
    //const matrix44& model = sceneGraph->GetModelTransform();
    const matrix44& model = this->GetEntityObject()->GetComponent<ncTransform>()->GetTransform();
    float minDist(0.f);
    const vector3& rayBegin = model.pos_component();
    this->allAreCandidates = false;

    // note , (contactPosition -pos).length = minDist
    ncSceneDotLaser::GetContactPosition( model, this->contactPosition, minDist, this->entityReceiver); 

    if  ( this->entityReceiver && !(this->entityReceiver->GetComponent<ncScene>()) )
    {
        this->entityReceiver = 0;
        this->allAreCandidates = true;
        // example this is a nePhySimple in a indoor.

        //@TODO: use the spatial server for search a good candidate
        // the idea is make a sphere, its centre is the collision points
        // if intersets the sphere with bbox of object is a good candidates;
    }

    if ( this->entityReceiver == entityObject )
    {
        this->entityReceiver = 0;
    }

    //Put variable for the beam
    //This nscenodes are not affect to nDotLaserNode.
    ncSceneDotLaserClass*  laserClass= this->GetClassComponentSafe<ncSceneDotLaserClass>();
    n_assert_return( laserClass , );
    nShaderParams& shaderParams = this->GetEntityObject()->GetComponentSafe<ncScene>()->GetShaderOverrides();
    shaderParams.SetArg( nShaderState::LightPos , vector4( rayBegin.x, rayBegin.y , rayBegin.z , 1.f) );
    vector3 lightDir = this->contactPosition - rayBegin;
    shaderParams.SetArg( nShaderState::LightDir, vector4( lightDir.x, lightDir.y , lightDir.z , laserClass->GetLaserEpsilon() ) );


    return ncSceneLight::Render( sceneGraph );
}

//------------------------------------------------------------------------------
/**
    Return the light projection
    tranform from world coordinate is light projection coordinates.
*/
const matrix44&
ncSceneDotLaser::GetLightProjection() const
{
    // The projection has the spatial component is the same frustrum,
    // I prefer not use a duplicate info.

    ncSpatialLight* sptLight= this->GetComponentSafe<ncSpatialLight>();
    if (sptLight)
    {
        nSpatialModel*  testModel = sptLight->GetTestModel();
        if ( testModel && testModel->GetType() == nSpatialModel::SPATIAL_MODEL_FRUSTUM  )
        {
            nSpatialFrustumModel* frustrum = static_cast<nSpatialFrustumModel*>(testModel);
            return frustrum->GetViewProjection();
        }
    }
    // this is a invalid case
    return matrix44_identity;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncSceneDotLaser::AffectsEntity(nEntityObject* entityObject)
{
    return (allAreCandidates) ||
           (entityObject == this->entityObject) || 
           ( entityObject == this->entityReceiver );
}

//------------------------------------------------------------------------------
/*
    Calculate the laser colision point.

*/
void
ncSceneDotLaser::GetContactPosition( const matrix44& model, vector3& contactPosition , float& minDist , nEntityObject *& receiver) const
{
    bool restoreEmmiterPhysicsIsEnable(false);
    static const int MAX_CONTACTS = 6;  
    static const float maxLenghtLaser(MAX_RAY_LENGTH);
    static const float stepLenght(5.f);
    const vector3& pos =  model.pos_component();
    ncPhysicsObj* emitterPhysicsObj(0);


    vector3  dir = -model.z_component();
    dir.norm();
 
    minDist = maxLenghtLaser;

    if ( this->GetEntityObject() )
    {
        emitterPhysicsObj = this->GetEntityObject()->GetComponent<ncPhysicsObj>();
    }
    if (emitterPhysicsObj)
    {
        restoreEmmiterPhysicsIsEnable = emitterPhysicsObj->IsEnabled();
        emitterPhysicsObj->Disable();
    }

    // Divide the ray in small rays. For search the first collision point.
    // The big ray is slow phisic collision
    // The phisic return the collision point but they are not the first n collision points.

    nPhyCollide::nContact contacts[MAX_CONTACTS];
    int maxLoop( (int)n_ceil( maxLenghtLaser / stepLenght ) ) ;
    int numContacts(0);
    vector3 beginPos(pos);
    vector3 deltaPos(dir*stepLenght);
    for( int idx = 0; (idx < maxLoop) && (numContacts == 0); ++idx )
    {
        numContacts = nPhysicsServer::Instance()->Collide( beginPos , dir , stepLenght, MAX_CONTACTS, contacts, nPhysicsGeom::All  );
        beginPos += deltaPos;
    }

    if (emitterPhysicsObj && restoreEmmiterPhysicsIsEnable)
    {
        emitterPhysicsObj->Enable();
    }

    // If contacts not found
    if ( ! numContacts)
    {
        contactPosition = pos + ( dir*minDist);
        receiver = 0;
    }
    else
    {
        // search the nearest contact
        // get first contact
        minDist*=minDist; // compare squared values 
        float dist = 0;
        int ind = -1;
        for( int i = 0 ; i < numContacts ; ++i )
        {
            contacts[i].GetContactPosition( contactPosition );
            dist = ( contactPosition - pos ).lensquared();
            const ncPhysicsObj* obj1 = contacts[i].GetPhysicsObjA();
            const ncPhysicsObj* obj2 = contacts[i].GetPhysicsObjB();
            if( (dist < minDist)
                && ( obj1 || obj2 ) // the ray not has a ncphysicobj
                )
            {
                minDist = dist;
                ind = i;

                if (obj1)
                {
                    receiver = obj1->GetEntityObject();
                } else
                {
                    receiver = obj2->GetEntityObject();
                }
            }
        }

        minDist = n_sqrt( minDist);
        if (ind >= 0 )
        {
            contacts[ind].GetContactPosition( contactPosition );
        } 
        else
        {
            contactPosition = pos + ( dir*minDist);
        }
    } 
}

//------------------------------------------------------------------------------
/*
    Update the all components.
    example spatial frustrum
*/
void
ncSceneDotLaser::UpdateLaserProjection()
{
    float radius1, radius2, dist1, dist2;
   

    this->GetClassComponentSafe<ncSceneDotLaserClass>()->GetLaserProjection( dist1, radius1, dist2, radius2);
    ncSpatialLight* sptLight= this->GetComponentSafe<ncSpatialLight>();
    ncTransform* transform= this->GetComponentSafe<ncTransform>();

    if ( radius1 == radius2 )
    {
        sptLight->SetZOffset( 0.f);
        sptLight->SetTestFrustumOrthoProperties( radius1, radius2, 0.00001f, MAX_RAY_LENGTH);

    } else if ( radius1 < radius2 )
    {
        // Equation
        // tan(a) = ( radius1 / (dist + dist1))  = ( radius2 / ( dist + dist2) )
        float dist  = ( dist2*radius1 - dist1*radius2 ) / ( radius2 - radius1 ) ;
        float fov_2 = n_atan2(radius1, ( dist + dist1) );
        float tan_a = n_tan(fov_2); //use this because (r1 = 0) and (d1 = 0) =>  (d =0) and ( 0 / 0 Ind)

        //Update spatial frustrum
        sptLight->SetZOffset( dist);
        sptLight->SetTestFrustumProperties( n_rad2deg(2.0f*fov_2), 1.f, dist, MAX_RAY_LENGTH + dist );
        if ( transform )
        {
            sptLight->Update(transform->GetTransform());
        }

        //Update the equation for laser beam
        // dist * equ.x + equ.y = radius
        vector4 equ( tan_a, tan_a*dist , 0.f, 0.f);
        nShaderParams& shaderParams = this->GetEntityObject()->GetComponentSafe<ncScene>()->GetShaderOverrides();  
        //use the lightDifuse as equation 
        shaderParams.SetArg( nShaderState::LightDiffuse1 , equ );


    } else
    {
        n_assert2_always(  "invalid  radius in SetLaserProjection" );
    }

}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncSceneDotLaser::TriggerClassModified(nObject* N_IFDEF_ASSERTS(emitter) )
{
    n_assert2( this->GetEntityClass() == emitter , "Update ncSceneDotLaser from other object" );
    this->UpdateLaserProjection();
}
#endif //!NGAME



//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
