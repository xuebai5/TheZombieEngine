#include "precompiled/pchn3dsmaxexport.h"
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsfileserver.h"
#include "n3dsphysicexport/n3dsphysicexport.h"
#include "n3dsgraphicsexport/n3dsgraphicsexport.h"
#include "entity/nentityobjectserver.h"
#include "nasset/nentityassetbuilder.h"
#include "ndebug/nceditorclass.h"
#include "n3dsvisibilityexport/n3dsvisibilityexport.h"

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsExportServer::ExportBrush()
{
    bool valid = false;
    nString className( exportSettings.entityName ); 
    nArray<n3dsAssetInfo> assetPathArray; // foreach LOD a entityClass


    // init the modules. In the init the modules collect Max Scene Information
    // GetGraphics()->Init();
    GetPhysics()->Init();

    // create the brush class
    nString parentName;
    valid = GetPhysics()->GetBrushClass( parentName );
    n_assert( valid );
    if( ! valid )
    {
        return false;
    }

    nEntityClass * entityClass = GetExporterEntityClass( parentName , className.Get() , "nebrush", "nesimplebrush", "nemirage");
    if (! entityClass)
    {
        return false;
    }

    assetPathArray = CreateEntityClassesForLOD( entityClass );


    if ( exportSettings.gameLibrary )
    {
        GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(entityClass) );
        for ( int idx = 0; idx < assetPathArray.Size() ; ++idx )
        {
            GetQueueFileServer()->QueueDeleteDirectory( assetPathArray[idx].assetPath );
        }
    }

    // set debug info from exporter
    this->SetEditorInfo( entityClass );

    
    GetGraphics()->ExportBrushClass( entityClass, assetPathArray);
    GetPhysics()->ExportBrushClass( entityClass );

    // create the brush object the object is created with the base class
    // for not to init the instance with old data from previous exports.
    nEntityObject * entityObject = nEntityObjectServer::Instance()->NewEntityObject( parentName.Get() );
    n_assert( entityObject );
    if( entityObject )
    {
        // add information to the brush object
        GetPhysics()->ExportBrushObject( entityObject );
        // Clean
        nEntityObjectServer::Instance()->RemoveEntityObject( entityObject );
    }

    // save debug export data
    if( entityClass )
    {
        ncEditorClass * editorClass = entityClass->GetComponent<ncEditorClass>();
        if( editorClass )
        {
            editorClass->SaveAssetChunk();
        }
    }

    //Export ocluders
    this->GetVisibility()->ExportOccluders( entityClass );

    //SetLODTo( entityClass, assetPathArray);

    return true;
}