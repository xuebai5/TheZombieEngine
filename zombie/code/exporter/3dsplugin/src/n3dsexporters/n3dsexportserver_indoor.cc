#include "precompiled/pchn3dsmaxexport.h"
#include "n3dsexporters/n3dsexportserver.h"

//------------------------------------------------------------------------------
#include "n3dsexporters/n3dsfileserver.h"
#include "n3dsgraphicsexport/n3dsgraphicsexport.h"
#include "n3dsvisibilityexport/n3dsvisibilityexport.h"
#include "n3dsphysicexport/n3dsphysicexport.h"

//------------------------------------------------------------------------------
#include "nspatial/nindoorbuilder.h"
#include "entity/nentityclassserver.h"
#include "nasset/nentityassetbuilder.h"
#include "ndebug/nceditorclass.h"
#include "nspatial/ncspatialclass.h"

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsExportServer::ExportIndoor()
{

    nEntityClass * indoorClass;
    nEntityClass * indoorBrushClass;
    nEntityClass * portalClass;
    nEntityClass * indoorFacadeClass;
    nArray<n3dsAssetInfo> assetPathArray; // foreach LOD a antityclas of indoorFacadeClass

    nString indoorClassName( exportSettings.entityName ); 
    nString indoorBrushClassName; //( indoorClassName);
    nString indoorPortalClassName; //( indoorClassName);
    nString indoorFacadeClassName;

    indoorBrushClassName = indoorClassName + "_indoor_brush";
    indoorClass  = GetExporterEntityClass("neindoor", indoorClassName);
    if ( ! indoorClass)
    {
        return false;
    }

    indoorBrushClassName = nEntityAssetBuilder::GetIndoorBrushClassName( indoorClass);
    indoorPortalClassName = nEntityAssetBuilder::GetPortalClassName( indoorClass);
    indoorFacadeClassName = nEntityAssetBuilder::GetIndoorFacadeClassName(indoorClass);

    // set debug info from exporter
    this->SetEditorInfo( indoorClass );


    indoorBrushClass = GetExporterEntityClass("neindoorbrush", indoorBrushClassName);
    if ( ! indoorBrushClass)
    {
        return false;
    }

    portalClass = GetExporterEntityClass("neportal", indoorPortalClassName.Get() );
    if ( ! portalClass)
    {
        return false;
    }

    indoorFacadeClass = GetExporterEntityClass("nemirage", indoorFacadeClassName.Get() );
    if ( ! indoorFacadeClass)
    {
        return false;
    }
    assetPathArray = CreateEntityClassesForLOD( indoorFacadeClass );

    if ( this->exportSettings.gameLibrary )
    {
        GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(indoorClass) );
        GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(indoorBrushClass) );
        GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(portalClass) );
        GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(indoorFacadeClass) );
        for ( int idx = 0; idx < assetPathArray.Size() ; ++idx )
        {
            GetQueueFileServer()->QueueDeleteDirectory( assetPathArray[idx].assetPath );
        }
    }

    // set debug info from exporter
    this->SetEditorInfo( indoorBrushClass );

    // init the modules. In the init the modules collect Max Scene Information
    GetGraphics()->InitExportIndoor(indoorClass , indoorBrushClass, portalClass, indoorFacadeClass );
    GetVisibility()->Init(indoorClass, indoorBrushClass, portalClass );
    GetPhysics()->Init();

    GetGraphics()->ExportIndoor(indoorClass , indoorBrushClass, portalClass, indoorFacadeClass, assetPathArray );
    GetPhysics()->ExportIndoorObjects(indoorClass, GetVisibility()->GetIndoorBuilder() );
    GetVisibility()->Save();

    // save export data
    ncEditorClass * editorClass = 0;
    editorClass = indoorClass->GetComponent<ncEditorClass>();
    if( editorClass )
    {
        editorClass->SaveAssetChunk();
    }

    editorClass = indoorBrushClass->GetComponent<ncEditorClass>();
    if( editorClass )
    {
        editorClass->SaveAssetChunk();
    }

    //SetLODTo( indoorFacadeClass, entityClassArray );

    // if not has a geometry
    if ( ! indoorFacadeClass->GetComponentSafe<ncSpatialClass>()->GetOriginalBBox().IsValid() )
    {
        // if not need a class remove it
        // for dont delete in original working copy save first in temporal and then delete it.
        // For save need a not invalid Box.
        bbox3 box( vector3(0.0f,0.0f,0.0f) , vector3(0.0f,0.0f,0.0f) );
        indoorFacadeClass->GetComponentSafe<ncSpatialClass>()->SetOriginalBBox(box);
        nEntityClassServer::Instance()->SaveEntityClass( indoorFacadeClass ); 
        nEntityClassServer::Instance()->RemoveEntityClass( indoorFacadeClassName.Get() );
        GetVisibility()->GetIndoorBuilder()->SetIndoorShell("");
    }
        

    // Export ocluders
    this->GetVisibility()->ExportOccluders( indoorClass );
    
    return true;
}