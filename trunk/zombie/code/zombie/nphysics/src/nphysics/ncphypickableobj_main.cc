//-----------------------------------------------------------------------------
//  nphypiceableobj_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#ifndef NGAME

#include "nphysics/ncphypickableobj.h"
#include "nphysics/nphygeombox.h"
#include "nphysics/nphysicsserver.h"

#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatialbatch.h"
#include "nscene/ncscene.h"

#include "zombieentity/nctransform.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyPickableObj,ncPhyCompositeObj);

//-----------------------------------------------------------------------------
namespace 
{
    static const phyreal epsilonExtens(phyreal(.1e-05));
}

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 07-Apr-2005   Zombie         created
*/
ncPhyPickableObj::ncPhyPickableObj() :
    lenghts(1,1,1)
{
    this->SetType( ncPhysicsObj::Pickeable );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 07-Apr-2005   Zombie         created
*/
ncPhyPickableObj::~ncPhyPickableObj()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Set the lengths of the box geometry for this object
        by default are 1 unit per axis.

    @param lenghts object lengths

    history:
        - 07-Apr-2005   Zombie         created
*/
void ncPhyPickableObj::SetLengths( const vector3& lengths )
{  
    vector3 lengthTmp(lengths);

    if( lengthTmp.x == 0 )
    {
        lengthTmp.x = epsilonExtens;
    }
    if( lengthTmp.y == 0 )
    {
        lengthTmp.y = epsilonExtens;
    }
    if( lengthTmp.z == 0 )
    {
        lengthTmp.z = epsilonExtens;
    }

    this->lenghts = lengthTmp;

    if( this->GetNumGeometries() )
    {
        static_cast<nPhyGeomBox*>(static_cast<nPhyGeomTrans*>(this->GetGeometry(0))->GetGeometry())->SetLengths( lengthTmp );
    }
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 17-May-2005   Zombie         created
*/
void ncPhyPickableObj::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhyCompositeObj::InitInstance(initType);

    if ( initType != nObject::ReloadedInstance )
    {
        nPhysicsServer::Instance()->Add( this );
    }

    if( initType != nObject::NewInstance )
    {
        ncTransform* transform( this->GetComponent<ncTransform>() );

        n_assert2( transform, "Component ncTransform requiered by this component." );

        matrix44 orientation(transform->GetQuat());

        orientation.set_translation(transform->GetPosition());

        this->Update(orientation);

        this->Scale(transform->GetScale().x);
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world it's the world where the object will be come to existance

    history:
        - 17-May-2005   Zombie         created
*/
void ncPhyPickableObj::Create( nPhysicsWorld* world )
{
    nPhyGeomBox* box( static_cast<nPhyGeomBox*>(nKernelServer::Instance()->New( "nphygeombox" )) );

    n_assert2( box, "Failed to create the pickeable object geometry." );

    if( this->containerGeometries.Size() > 0  )
    {
        // error
        char buffer[ 4096 ];
        sprintf( buffer, "The pickable object (%s) already contains collision geometries.", this->GetEntityClass()->GetName() );
        NLOG( physicsLog, (1,buffer) );
        return;
    }

    this->Add( box );

    this->SetLengths( this->lenghts );

    this->UpdateWithSpatialOrScene();

    ncPhyCompositeObj::Create( world );
}


//-----------------------------------------------------------------------------
/**
    @brief Updates lenghts and position of box with spatial or scene AABB

    If the entity hasn't got ncSpatial then 
*/
void
ncPhyPickableObj::UpdateWithSpatialOrScene()
{
    if( !this->GetNumGeometries() )
    {
        return;
    }

    nPhyGeomBox * box = static_cast<nPhyGeomBox*>(static_cast<nPhyGeomTrans*>(this->GetGeometry(0))->GetGeometry());

    ncSpatial* spatial( this->GetComponent<ncSpatial>() );
    if( spatial )
    {
        bbox3 bb(spatial->GetOriginalBBox());
        
        if( bb.diagonal_size() != phyreal(0) )
        {
            vector3 extens(bb.extents());

            extens *= phyreal(2);

            this->SetLengths(extens);
            box->SetPosition(bb.center());
        }
    }
    else
    {
        ncScene* scene( this->GetComponent<ncScene>() );

        if( scene )
        {
            bbox3 bb(scene->GetLocalBox());
            
            if( bb.diagonal_size() != phyreal(0) )
            {
                vector3 extens(bb.extents());

                extens *= phyreal(2);

                this->SetLengths(extens);
                box->SetPosition(bb.center());
            }
        }
    }
}

#else

class nClassComponentObject *
n_init_ncPhyPickableObj(char const *,class nComponentObjectServer *)
{
    return 0;
}

#endif // !NGAME

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
