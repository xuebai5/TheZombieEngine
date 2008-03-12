//------------------------------------------------------------------------------
//  n3dsphysicexport_brush.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsphysicexport/n3dsphysicexport.h"

#include "n3dsexporters/n3dscustomattributes.h"
#include "n3dsexporters/n3dslog.h"

#include "nphysics/ncphysimpleobj.h"
#include "nphysics/ncphycompositeobj.h"

#include "zombieentity/ncloaderclass.h"

//------------------------------------------------------------------------------
/**
    @retval BRUSH_COMPOSITE if brush has a composite
    @retval BRUSH_SIMPLE if brush has a simple geometry
*/
n3dsPhysicExport::BrushType
n3dsPhysicExport::GetBrushType( ) const
{
    if( collisionObjects.Size() == 1 )
    {
        if( T_COMPOSITE == this->GetCollisionType( collisionObjects[0] ) )
        {
            return BRUSH_COMPOSITE;
        }
        else if( T_REALCYLINDER == this->GetCollisionType( collisionObjects[0] ) )
        {
            return BRUSH_COMPOSITE;
        }

        //return BRUSH_SIMPLE;
        return BRUSH_COMPOSITE;
    }
    else if( collisionObjects.Size() > 1 )
    {
        // check that there isn't composites
        nArray<INode*>::iterator it;
        for( it = this->collisionObjects.Begin(); it != this->collisionObjects.End() ; ++it )
        {
            if( T_COMPOSITE == this->GetCollisionType( *it ) )
            {
                N3DSERROR( physicExport , ( 0 , "ERROR: Incorrect use of composite in a Brush , \"%s\" ", (*it)->GetName() ) );
                return BRUSH_NONE;
            }
        }

        return BRUSH_COMPOSITE;
    }

    return BRUSH_NONE;
}

//------------------------------------------------------------------------------
/**
    @param parentName [OUT] return the name of parent class
    @retval true if return a parent class name
*/
bool 
n3dsPhysicExport::GetBrushClass( nString & parentName )
{
    switch( this->GetBrushType() )
    {
    case BRUSH_COMPOSITE:
        parentName = "nebrush";
        return true;
        break;

    case BRUSH_SIMPLE:
        parentName = "nesimplebrush";
        return true;
        break;

    case BRUSH_NONE:
        parentName ="nemirage";
        N3DSWARN( physicExport , ( 0  , "WARNING: Create Brush without collision objects" ) );
        return true;
        break;

    default:
        n_assert_always();
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @param entityClass entity class instance to export
*/
void
n3dsPhysicExport::ExportBrushClass( nEntityClass * entityClass )
{
    n_assert( entityClass );
    this->entityClass = entityClass;
}

//------------------------------------------------------------------------------
/**
    @param entityObject entity object instance to export
*/
void
n3dsPhysicExport::ExportBrushObject( nEntityObject * entityObject )
{
    // check the object
    bool valid = ( entityObject != 0 && this->entityClass != 0 );
    n_assert( valid );
    if( ! valid )
    {
        return;
    }

    ncLoaderClass * loaderClass = this->entityClass->GetComponent<ncLoaderClass>();
    n_assert( loaderClass );
    if( loaderClass )
    {
        this->physicPath = loaderClass->GetResourceFile();
        this->physicPath += "/physics";
    }
    else
    {
        this->physicPath = "physics";
    }
    
    switch( this->GetBrushType() )
    {
    case BRUSH_COMPOSITE:
        this->ExportCompositeBrushObject( entityObject );
        break;

    case BRUSH_SIMPLE:
        this->ExportSimpleBrushObject( entityObject );
        break;

    case BRUSH_NONE:
        // no physics to export
        break;

    default:
        n_assert_always();
    }

    this->entityClass = 0;
}

//------------------------------------------------------------------------------
/**
    @param entityObject entity object instance to export
*/
void
n3dsPhysicExport::ExportSimpleBrushObject( nEntityObject * entityObject )
{
    bool valid = false;

    // check that only there is a geometry
    valid = ( collisionObjects.Size() == 1 );
    n_assert( valid );
    if( ! valid )
    {
        return;
    }

    // check that there isn't a composite or realcylinder ( that create a composite )
    valid = ( T_COMPOSITE != this->GetCollisionType( collisionObjects[0] ) );
    valid = valid && ( T_REALCYLINDER != this->GetCollisionType( collisionObjects[0] ) );
    n_assert( valid );
    if( ! valid )
    {
        return;
    }

    // get physic component
    ncPhySimpleObj * object = entityObject->GetComponent<ncPhySimpleObj>();
    n_assert( object );
    if( ! object )
    {
        return;
    }

    // create the geometry
    nArray<nPhysicsGeom*> geometries;
    this->CreateGeometries( collisionObjects[0], this->physicPath, geometries );
    if( geometries.Size() == 1)
    {
        nPhysicsGeom * phyGeom = geometries[ 0 ];

        this->SetGeomAttributes( collisionObjects[0], phyGeom );

        object->SetGeometry( phyGeom );
    }
    else
    {
        N3DSERROR( physicExport , ( 0 , "ERROR: Can't create geometry %s for %s\n", collisionObjects[0]->GetName(), 
                this->entityClass->nClass::GetName() ) );
    }

    // set category of physic object
    this->SetCollisionAttributes( collisionObjects[ 0 ], object );

    this->ExportPhysicChunk( this->entityClass, entityObject, object );
}

//------------------------------------------------------------------------------
/**
    @param entityObject entity object instance to export
*/
void
n3dsPhysicExport::ExportCompositeBrushObject( nEntityObject * entityObject )
{
    bool valid = false;

    // get the geometries
    nArray<INode*> compGeometries;
    nArray<INode*> * geometries = 0;
    if( collisionObjects.Size() == 1 )
    {
        if( T_COMPOSITE == this->GetCollisionType( collisionObjects[0] ) )
        {
            valid = n3dsCustomAttributes::GetParamNodes( collisionObjects[0], 
                "CollCompositeParams", "geometries", compGeometries );
            n_assert( valid );
            if( ! valid )
            {
                return;
            }
        }
        else
        {
            compGeometries.Append( collisionObjects[0] );
        }
        geometries = &compGeometries;
    } 
    else if( collisionObjects.Size() > 1 )
    {
        geometries = &collisionObjects;
    }
    else
    {
        n_assert_always();
        return;
    }

    valid = geometries != 0 && geometries->Size() >= 1;
    n_assert( valid );
    if( ! valid )
    {
        return;
    }

    ncPhyCompositeObj * object = entityObject->GetComponent<ncPhyCompositeObj>();
    n_assert( object );
    if( ! object )
    {
        return;
    }

    //If the class already existed to erase the previous objects
    while ( object->GetNumGeometries() > 0 )
    {
        object->Remove( object->GetGeometry( object->GetNumGeometries() -1 ) ); 
    }

    // create the geometry objects
    nPhysicsGeom * phyGeom = 0;
    nArray<nPhysicsGeom*> geoms;
    for( int i=0; i < geometries->Size() ; ++i )
    {
        this->CreateGeometries( (*geometries)[i], this->physicPath, geoms );

        for( int j = 0 ; j < geoms.Size() ; ++j )
        {
            phyGeom = geoms[ j ];

            this->SetGeomAttributes( (*geometries)[i], phyGeom );

            object->Add( phyGeom );
        }

        if( geoms.Size() == 0 )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Can't create geometry %s for %s\n", (*geometries)[i]->GetName(), 
                    this->entityClass->nClass::GetName() ) );
        }

        geoms.Reset();
    }

    if( object->GetNumGeometries() > 0 )
    {

       // set category of physic object
        this->CheckAllPhysicCategories( geometries );
        this->SetCollisionAttributes( (*geometries)[ 0 ], object );

        this->ExportPhysicChunk( this->entityClass, entityObject, object );
    }
    else
    {
        N3DSERROR( physicExport , ( 0 , "ERROR: Composite Object without geometries in \"%s\"", 
                this->entityClass->nClass::GetName() ) );
    }
}

//------------------------------------------------------------------------------
