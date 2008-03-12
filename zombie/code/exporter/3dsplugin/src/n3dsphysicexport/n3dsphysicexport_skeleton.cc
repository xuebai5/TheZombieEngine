//------------------------------------------------------------------------------
//  n3dsphysicexport_skeleton.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchn3dsmaxexport.h"

#include "n3dsphysicexport/n3dsphysicexport.h"
#include "n3dsexporters/n3dssystemcoordinates.h"

//------------------------------------------------------------------------------
#pragma warning( push, 3 )
#include "Max.h"
#include "simpobj.h"
#include "iparamb2.h"
#include "icustattribcontainer.h"
#include "custattrib.h"
#pragma warning( pop )

//------------------------------------------------------------------------------
#include "n3dsexporters/n3dscustomattributes.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsexporters/n3dsexportserver.h"

//------------------------------------------------------------------------------
#include "n3dsanimationexport/n3dsskeleton.h"
#include "nphysics/ncphycharacterobj.h"
#include "zombieentity/ncloaderclass.h"

//------------------------------------------------------------------------------
/**
    @param entityClass entity class instance to export
    @param pSkeleton the skeleton
*/
void
n3dsPhysicExport::ExportSkeletonClass( nEntityClass * entityClass, n3dsSkeleton* pSkeleton )
{
    n_assert( entityClass );
    n_assert( pSkeleton );
    this->skeleton = pSkeleton;

    // create physics entity
    this->entityClass = entityClass;
}

//------------------------------------------------------------------------------
/**
    @param entityObject entity object instance to export
*/
void
n3dsPhysicExport::ExportSkeletonObject( nEntityObject * entityObject)
{
    // check the object
    bool valid = ( entityObject != 0 && this->entityClass != 0 );
    n_assert( valid );
    if( ! valid )
    {
        N3DSERROR( animexport , ( 0 , "ERROR: no entityobject or no entityclass (n3dsPhysicExport::ExportSkeletonObject)"));
        return;
    }

    //collect geometries from scene
    this->Init();

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
    
    //change for composite skeleton object
    this->ExportCompositeSkeletonObject( entityObject );

    this->entityClass = 0;    
}

//------------------------------------------------------------------------------
/**
    @param entityObject entity object instance to export
*/
void 
n3dsPhysicExport::ExportCompositeSkeletonObject( nEntityObject * entityObject )
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
        //n_assert_always(); // FIXME restore it when exporting physics (asset will appear each time there's no collision objects)
        return;
    }

    valid = geometries != 0 && geometries->Size() >= 1;
    n_assert( valid );
    if( ! valid )
    {
        return;
    }

    ncPhyCharacterObj * object = entityObject->GetComponent<ncPhyCharacterObj>();
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
        
        //get bone index
        INode* boneNode((*geometries)[i]->GetParentNode());
        int boneIGameId = n3dsExportServer::Instance()->GetIGameScene()->GetIGameNode(boneNode)->GetNodeID();
        nString boneName = n3dsExportServer::Instance()->GetIGameScene()->GetIGameNode(boneNode)->GetName();//TEMP
        int boneIdx = this->skeleton->FindBoneIndexByNodeId( boneIGameId );
        vector3 offset = this->FindOffset( (*geometries)[i], boneIdx );

        for( int j = 0 ; j < geoms.Size() ; ++j )
        {
            phyGeom = geoms[ j ];
            this->SetGeomAttributes( (*geometries)[i], phyGeom );
            phyGeom->SetIdTag( boneIdx );

            object->Add( phyGeom );
            object->SetGeometryOffset(boneIdx, offset);
        }

        if( geoms.Size() == 0 )
        {
            N3DSERROR( physicExport , ( 0 , "ERROR: Can't create geometry %s for %s\n", (*geometries)[i]->GetName(), 
                    this->entityClass->nClass::GetName() ) );
        }

        geoms.Reset();
    }

    this->ExportPhysicChunk( this->entityClass, entityObject, object );
}

//------------------------------------------------------------------------------
/**
*/
vector3
n3dsPhysicExport::FindOffset( INode* iNode, int boneIdx )
{
    n3dsSystemCoordinates * sysCoord = n3dsExportServer::Instance()->GetSystemCoordinates();

    transform44 tr = sysCoord->MaxToNebulaTransform( iNode->GetNodeTM(0) );
    vector3 boxPose = tr.gettranslation();
    vector3 worldJointPose = this->skeleton->BonesArray[boneIdx].worldTr.pos_component();
    vector3 offset = boxPose - worldJointPose;

    if( n3dsPhysicExport::GetCollisionType( iNode ) == T_SPHERE)
    {
        offset *= -1;
    }

    return offset;
}

//------------------------------------------------------------------------------
