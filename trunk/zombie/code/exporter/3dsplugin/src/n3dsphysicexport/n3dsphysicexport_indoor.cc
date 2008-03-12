//------------------------------------------------------------------------------
//  n3dsphysicexport_indoor.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsphysicexport/n3dsphysicexport.h"

#include "entity/nobjectinstancer.h"
#include "entity/nentityobjectserver.h"

#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dscustomattributes.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsvisibilityexport/n3dsvisibilityexport.h"

#include "nspatial/nindoorbuilder.h"
#include "nphysics/nphyspace.h"
#include "nphysics/ncphysimpleobj.h"
#include "nphysics/ncphycompositeobj.h"
#include "nasset/nentityassetbuilder.h"

//------------------------------------------------------------------------------
/**
    @param node max node with the object info
    @returns the created entity object
*/
nEntityObject *
n3dsPhysicExport::CreatePhySimpleObject( INode * const node )
{
    nEntityObject * entityObject;
    entityObject = nEntityObjectServer::Instance()->NewEntityObject( "nephysimple" );
    n_assert2( entityObject, "Can not create a Simple Physics Entity" );
    if( ! entityObject )
    {
        return 0;
    }

    // get physic component
    ncPhySimpleObj * phyObject = entityObject->GetComponent<ncPhySimpleObj>();
    n_assert2( phyObject, "Can not get the Simple Physic Component" );
    if( ! phyObject )
    {
        entityObject->Release();
        return 0;
    }

    // create the geometry
    nArray<nPhysicsGeom*> geoms;
    this->CreateGeometries( node, this->physicPath, geoms );
    if( geoms.Size() == 1 )
    {
        nPhysicsGeom* phyGeom = geoms[ 0 ];

        this->SetGeomAttributes( node, phyGeom );

        phyObject->SetGeometry( phyGeom );
    }
    else
    {
        N3DSERROR( physicExport , ( 0 , "ERROR: Can't create geometry %s for %s\n", node->GetName(), 
                this->entityClass->nClass::GetName() ) );
        entityObject->Release();
        return 0;
    }

    // set category of physic object
    this->SetCollisionAttributes( node, phyObject );

    // set the space
    TSTR spaceName;
    bool valid;
    valid = n3dsCustomAttributes::GetParamString( node, "CollParams", "space", spaceName);
    if( valid && ( 0 != strcmp( spaceName, "<none>" ) ) )
    {
        phyObject->SetSpaceByName( spaceName.data() );
    }

    return entityObject;
}

//------------------------------------------------------------------------------
/**
    @param node max node with the object info
    @returns the created entity object
*/
nEntityObject *
n3dsPhysicExport::CreatePhyCompositeObject( INode * const node )
{
    nEntityObject * entityObject = 0;
    entityObject = nEntityObjectServer::Instance()->NewEntityObject( "nephycomposite" );
    n_assert2( entityObject, "Can not create a Composite Physics Entity" );
    if( ! entityObject )
    {
        return 0;
    }

    // get physic component
    ncPhyCompositeObj * phyObject = entityObject->GetComponent<ncPhyCompositeObj>();
    n_assert2( phyObject, "Can no get the Composite Physic Component" );
    if( ! phyObject )
    {
        entityObject->Release();
        return 0;
    }


    // get the geometries
    nArray<INode*> geometries;
    bool valid = n3dsCustomAttributes::GetParamNodes( node, 
        "CollCompositeParams", "geometries", geometries );
    n_assert2( valid, "Composite object without Composite parameters" );
    if( ! valid )
    {
        entityObject->Release();
        return 0;
    }

    // create the geometry objects
    nArray<nPhysicsGeom*> geoms;
    nPhysicsGeom* phyGeom = 0;
    for( int i=0; i < geometries.Size() ; ++i )
    {
        this->CreateGeometries( geometries[i], this->physicPath, geoms );

        for( int j = 0 ; j < geoms.Size() ; ++j )
        {
            phyGeom = geoms[ j ];

            this->SetGeomAttributes( geometries[i], phyGeom );

            phyObject->Add( phyGeom );
        }

        if( geoms.Size() == 0 )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Can't create geometry %s for %s\n", geometries[i]->GetName(), 
                    this->entityClass->nClass::GetName() ) );
        }

        geoms.Reset();
    }

    if( phyObject->GetNumGeometries() <= 0 )
    {
        N3DSERROR( physicExport , ( 0 , "ERROR: Composite Object without geometries \"%s\"", node->GetName() ) );
        entityObject->Release();
        return 0;
    }

    // get the space. check that all geometries are in the same space
    TSTR spaceName;
    valid = n3dsCustomAttributes::GetParamString( geometries[ 0 ], "CollParams", "space", spaceName);
    for( int i=0; valid && ( i < geometries.Size() ) ; ++i )
    {
        TSTR geomSpace;
        bool validGeom;
        validGeom = n3dsCustomAttributes::GetParamString( geometries[ i ], "CollParams", "space", geomSpace);
        if( validGeom && ( 0 != strcmp( spaceName, geomSpace ) ) )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Geometries in %s with differents space names", node->GetName() ) );
            valid = false;
        }
    }

    if( valid && ( 0 != strcmp( spaceName, "<none>" ) ) )
    {
        phyObject->SetSpaceByName( spaceName.data() );
    }


    // set category of physic object
    this->CheckAllPhysicCategories( &geometries );
    this->SetCollisionAttributes( geometries[ 0 ], phyObject );

    return entityObject;
}

//------------------------------------------------------------------------------
/**
    @param node max node with the object info
    @returns the created entity object
*/
nEntityObject *
n3dsPhysicExport::CreatePhyCylinderObject( INode * const node )
{
    nEntityObject * entityObject = 0;
    entityObject = nEntityObjectServer::Instance()->NewEntityObject( "nephycomposite" );
    n_assert( entityObject );
    if( ! entityObject )
    {
        return 0;
    }

    // get physic component
    ncPhyCompositeObj * phyObject = entityObject->GetComponent<ncPhyCompositeObj>();
    n_assert( phyObject );
    if( ! phyObject )
    {
        entityObject->Release();
        return 0;
    }

    // create the geometry objects
    nArray<nPhysicsGeom*> geoms;
    nPhysicsGeom* phyGeom = 0;

    this->CreateGeometries( node, this->physicPath, geoms );

    for( int i = 0 ; i < geoms.Size() ; ++i )
    {
        phyGeom = geoms[ i ];

        this->SetGeomAttributes( node, phyGeom );

        phyObject->Add( phyGeom );
    }

    if( geoms.Size() == 0 )
    {
        N3DSERROR( physicExport , ( 0 , "ERROR: Can't create geometries %s for %s", node->GetName(), 
                this->entityClass->nClass::GetName() ) );
        entityObject->Release();
        return 0;
    }

    // set the space
    TSTR spaceName;
    bool valid;
    valid = n3dsCustomAttributes::GetParamString( node, "CollParams", "space", spaceName);
    if( valid && ( 0 != strcmp( spaceName, "<none>" ) ) )
    {
        phyObject->SetSpaceByName( spaceName.data() );
    }

    return entityObject;
}

//------------------------------------------------------------------------------
/**
    @param indoorClass indoor class that is exported
    @param builder indoor builder used to export the objects
*/
void
n3dsPhysicExport::ExportIndoorObjects(nEntityClass* indoorClass, nIndoorBuilder * builder )
{
    if( ! indoorClass || ! builder )
    {
        n_assert2_always( "Incorrect call to ExportIndoorObjects" );
        return;
    }

    this->entityClass = indoorClass;

    this->physicPath = nEntityAssetBuilder::GetAssetPath( indoorClass );
    this->physicPath.Append( "/physics" );
    
    nFileServer2::Instance()->MakePath( this->physicPath.Get() );

    nObjectInstancer * objectArray = 0;

    // create space instancer
    objectArray = static_cast<nObjectInstancer*>( nKernelServer::Instance()->New( "nobjectinstancer" ) );
    n_assert( objectArray );
    if( !objectArray )
    {
        return;
    }

    nArray<nString> spaces;
    nString spaceName;
    // get the spaces
    for( int i=0; i < this->collisionObjects.Size() ; ++i )
    {
        TSTR name;
        bool valid;
        valid = n3dsCustomAttributes::GetParamString( this->collisionObjects[ i ], "CollParams", "space", name);
        if( valid && ( 0 != strcmp( name, "<none>" ) ) )
        {
            spaceName.Set( name.data() );
            if( spaces.FindIndex( spaceName ) == -1 )
            {
                spaces.Append( spaceName );
            }
        }
    }


    objectArray->Init( spaces.Size(), 0 );

    // create the spaces
    for( int i = 0 ; i < spaces.Size() ; ++i )
    {
        nPhySpace * space = static_cast<nPhySpace*>( nKernelServer::Instance()->New( "nphygeomhashspace" ) );
        n_assert( space );
        if( space )
        {
            space->SetName( spaces[ i ] );
            objectArray->Append( space );
        }
    }

    // save spaces
    nString filename( this->physicPath );
    filename.Append( "/spaces.n2" );
    objectArray->SaveAs( filename.Get() );

    // clean object array
    for( int i = 0 ; i < spaces.Size() ; ++i )
    {
        objectArray->At( i )->Release();
    }
    objectArray->Release();

    // create object instancer
    objectArray = static_cast<nObjectInstancer*>( nKernelServer::Instance()->New( "nobjectinstancer" ) );
    n_assert( objectArray );
    if( !objectArray )
    {
        return;
    }

    objectArray->Init( this->collisionObjects.Size(), 0 );

    // create the geometry objects
    for( int i=0; i < this->collisionObjects.Size() ; ++i )
    {
        nEntityObject * object = 0;

        if( T_COMPOSITE == GetCollisionType( this->collisionObjects[ i ] ) )
        {
            object = this->CreatePhyCompositeObject( this->collisionObjects[ i ] );
        }
        else if( T_REALCYLINDER == GetCollisionType( this->collisionObjects[ i ] ) )
        {
            object = this->CreatePhyCylinderObject( this->collisionObjects[ i ] );
        }
        else
        {
            object = this->CreatePhySimpleObject( this->collisionObjects[ i ] );
        }

        if( object )
        {
            ncPhysicsObj * phyObject = object->GetComponent<ncPhysicsObj>();
            n_assert( phyObject );
            if( phyObject )
            {
                phyObject->isInIndoor = true;

                // calculate the bounding box
                nPhysicsAABB pBox;
                phyObject->GetAABB( pBox );
            
                vector3 center;
                center.x = (pBox.maxx + pBox.minx)/2.0f;
                center.y = (pBox.maxy + pBox.miny)/2.0f;
                center.z = (pBox.maxz + pBox.minz)/2.0f;

                vector3 extends;
                extends.x = abs( center.x - pBox.maxx );
                extends.y = abs( center.y - pBox.maxy );
                extends.z = abs( center.z - pBox.maxz );
                
                bbox3 box( center, extends );

                // put in a spatial cell if possible
                if( ! builder->InsertPhysicsObject( object, box ) )
                {
                    // or put in global instancer
                    objectArray->Append( object );
                }
            }
        }
        else
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Can not create physics object for %s", 
                this->collisionObjects[ i ]->GetName() ) );
        }
    }

    // save objects
    filename = this->physicPath;
    filename.Append( "/objects.n2" );
    objectArray->SaveAs( filename.Get() );

    // clean object array
    for( int i = 0 ; i < objectArray->Size() ; ++i )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( static_cast<nEntityObject*>(objectArray->At( i )) );
    }
    objectArray->Release();
}

//------------------------------------------------------------------------------
