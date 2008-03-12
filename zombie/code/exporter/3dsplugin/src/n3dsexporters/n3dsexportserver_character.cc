#include "precompiled/pchn3dsmaxexport.h"
//------------------------------------------------------------------------------
#include "n3dsexporters/n3dsexportserver.h"
#include "n3dsexporters/n3dsfileserver.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsphysicexport/n3dsphysicexport.h"
#include "n3dsanimationexport/n3dsanimationexport.h"
#include "n3dsanimationexport/n3dsskeleton.h"
#include "n3dsgraphicsexport/n3dsgraphicsexport.h"
#include "entity/nentityclassserver.h"

//------------------------------------------------------------------------------
#include "animcomp/nccharacterclass.h"

//------------------------------------------------------------------------------
#include "nasset/nentityassetbuilder.h"
#include "ndebug/nceditorclass.h"
#include "entity/nentityclass.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/ncdictionaryclass.h"
#include "zombieentity/ncassetclass.h"
#include "animcomp/ncskeletonclass.h"

//------------------------------------------------------------------------------
/**
    different character exportations depending on what has to be exported there are
*/
void
n3dsExportServer::ExportCharacterResources()
{
    switch( this->GetSettings().animMode )
    {
    case n3dsExportSettings::Mesh:
        {
            this->ExportCharacter();
            break;
        }
    case n3dsExportSettings::Skeleton:
        {
            this->ExportSkeleton();
            break;
        }
    case n3dsExportSettings::SkinAnimation:
        {
            this->GetAnimation()->ExportSkinAnimation();
            break;
        }
    case n3dsExportSettings::MorpherAnimation:
        {
            // @todo check if it's a morph animation resource
            //export resource
            //add animation to the correspondant morph entity
            break;
        }
    default:
        {
            //TODO
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsExportServer::ExportCharacter()
{
    nString className( this->exportSettings.entityName );
    nString parentName( "necharacter" );    
    nArray<n3dsAssetInfo> assetPathArray; // foreach LOD an entityClass
    nArray<n3dsAssetInfo> ragAssetPathArray; // foreach ragdoll an entityClass

    nEntityClass * entityClass = this->GetExporterEntityClass( parentName , className );
    if( ! entityClass )
    {
        N3DSERROR( animexport , ( 0 , "ERROR: There's another entity with that name. Change the name. (n3dsExportServer::ExportCharacter)"));
        return false;
    }

    assetPathArray = this->CreateEntityClassesForLOD( entityClass );
    ragAssetPathArray = this->CreateEntityClassesForRagdoll( entityClass );
    n_assert( ragAssetPathArray.Size() == assetPathArray.Size() );

    if ( exportSettings.gameLibrary )
    {
        GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(entityClass) );
        for ( int idx = 0; idx < assetPathArray.Size() ; ++idx )
        {
            GetQueueFileServer()->QueueDeleteDirectory( assetPathArray[idx].assetPath );
        }
        for ( int idx = 0; idx < ragAssetPathArray.Size() ; ++idx )
        {
            GetQueueFileServer()->QueueDeleteDirectory( ragAssetPathArray[idx].assetPath );
        }
    }

    // set debug info from exporter
    this->SetEditorInfo( entityClass );

    // set variables
    ncDictionaryClass *dictionaryClass = entityClass->GetComponent<ncDictionaryClass>();
    dictionaryClass->SetFloatVariable("one", 1);
    //dictionaryClass->SetIntVariable("charState", 1);

    //set skeleton data
    ncCharacterClass *characterClass = entityClass->GetComponent<ncCharacterClass>();
    
    this->GetGraphics()->ExportCharacterClass( entityClass, assetPathArray, ragAssetPathArray);

    //for each skeleton_lod, setskeletonclass
    for( int lodLevel=0; lodLevel< n3dsExportServer::Instance()->GetSettings().lod.GetCountLOD(); lodLevel++)
    {
        nString lodsk = this->exportSettings.skeletonName;
        lodsk += "_";
        lodsk += lodLevel;

        characterClass->SetSkeletonLevelClass( lodLevel, lodsk.Get() );

        if(lodLevel==( n3dsExportServer::Instance()->GetSettings().lod.GetCountLOD()-1))
        {
            characterClass->SetPhysicsSkeletonClass( lodsk.Get() );
        }
    }

    nString ragSkeletonName = this->exportSettings.skeletonName + "_ragdoll";

    if( nEntityClassServer::Instance()->GetEntityClass(ragSkeletonName.Get() ))
    {
        characterClass->SetRagdollSkeletonClass( ragSkeletonName.Get() );
    }

    //no lod
    if( n3dsExportServer::Instance()->GetSettings().lod.GetCountLOD() == 0 )
    {
        characterClass->SetSkeletonLevelClass(0, this->exportSettings.skeletonName.Get());//SetSkeletonClass( this->exportSettings.skeletonName.Get() );
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

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
n3dsExportServer::ExportSkeleton()
{
    n3dsExportServer *exportServer = n3dsExportServer::Instance();

    // get skeletons
    nArray<n3dsSkeleton*> skeletons(exportServer->GetSettings().lod.GetCountLOD(),1);
    nArray<int> rootBones;

    for(int lodLevel = 0; lodLevel< exportServer->GetSettings().lod.GetCountLOD() ; lodLevel++)
    {
        skeletons.Append( n_new(n3dsSkeleton(lodLevel)) );
        if( skeletons[0]->BonesArray.Size() == 0 )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: There's no skeleton for %i lod. (n3dsExportServer::ExportSkeleton)", lodLevel));
            return false;
        }
        rootBones.Append( skeletons[lodLevel]->GetNumberOfRootBones());
    }

    if( exportServer->GetSettings().lod.GetCountLOD() == 0 )
    {
        // get skeleton
        skeletons.Append( n_new(n3dsSkeleton()) );
        if( skeletons[0]->BonesArray.Size() == 0 )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: There's no skeleton in that scene (n3dsExportServer::ExportSkeleton)"));
            return false;
        }
    }

    // get entity name
    nString className( exportServer->GetSettings().entityName );
    //get class name
    nString parentName( "neskeleton" );

    for(int lodLevel = 0; lodLevel< skeletons.Size() ; lodLevel++)
    {
        //skeleton name, if lod, add _x
        nString lodClassName = className;
        if(skeletons.Size() != 1)
        {
            lodClassName += "_";
            lodClassName += lodLevel;
        }

        nEntityClass * entityClass = this->GetExporterEntityClass( parentName , lodClassName );
        if( ! entityClass )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: Cannot create entityclass for %i skeleton (n3dsExportServer::ExportSkeleton)", lodLevel));
            return false;
        }

        //add file or directory
        ncSkeletonClass* skelClass = entityClass->GetComponentSafe<ncSkeletonClass>();

        if ( this->exportSettings.gameLibrary )
        {
            this->GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(entityClass) );
            ///do not remove chunk data from lodlevel 0 because that file is filled by artists
            if( lodLevel == 0)
            {
                nString path = skelClass->GetChunkPath();
                this->GetQueueFileServer()->QueueCopyFile(path, path,true);
            }
        }
        // set debug info from exporter
        this->SetEditorInfo( entityClass );

        n_assert(skeletons[lodLevel]);
        this->GetAnimation()->ExportSkeleton( entityClass, lodLevel, skeletons );

        if( lodLevel == 0)
        {
            nArray<n3dsAssetInfo> assetPathArray; // foreach LOD an entityClass
            nArray<n3dsAssetInfo> ragAssetPathArray; // foreach ragdoll an entityClass
            assetPathArray = this->CreateEntityClassesForLOD( entityClass );
            ragAssetPathArray = this->CreateEntityClassesForRagdoll( entityClass );
            n_assert( ragAssetPathArray.Size() == assetPathArray.Size() );
            this->GetGraphics()->ExportCharacterClass( entityClass, assetPathArray, ragAssetPathArray );
        }

        //physics are related to lowest lod skeleton
        if( lodLevel == (skeletons.Size()-1))
        {
            this->GetPhysics()->ExportSkeletonClass( entityClass, skeletons[lodLevel] );

            //create the skeleton object, the object is created with the base class
            //for not to init the instance with old data from previous exports.
            nEntityObject * entityObject = nEntityObjectServer::Instance()->NewEntityObject( parentName.Get() );
            n_assert( entityObject );
            if( entityObject )
            {
                // add information to skeleton object
                this->GetPhysics()->ExportSkeletonObject( entityObject );
                // Clean
                nEntityObjectServer::Instance()->RemoveEntityObject( entityObject );
            }
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
    }

    //export ragdoll skeleton data if there's any
    if( skeletons[0]->RagBonesArray.Size() != 0 )
    {
        // get entity name
        nString ragdollClassName( exportServer->GetSettings().entityName );
        ragdollClassName += "_ragdoll";

        // what kind of ragdoll is it
        n3dsExportSettings::CritterNameType exportCritter = n3dsExportServer::Instance()->GetSettings().critterName;
        switch ( exportCritter )
        {
        case n3dsExportSettings::Human:
            parentName = "nehumragdoll";
            break;

        case n3dsExportSettings::Scavenger:
            parentName = "nefourleggedragdoll";
            break;

        case n3dsExportSettings::Strider:
            parentName = "nehumragdoll";
            break;
                    
        default:
            n_assert_always();
        }

        // create/get entityclass
        nEntityClass * entityClass = this->GetExporterEntityClass( parentName , ragdollClassName );
        if( ! entityClass )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: Cannot create ragdoll entityclass (n3dsExportServer::ExportSkeleton)"));
            return false;
        }

        if ( this->exportSettings.gameLibrary )
        {
            this->GetQueueFileServer()->QueueDeleteDirectory( nEntityAssetBuilder::GetAssetPath(entityClass) );
        }

        this->GetAnimation()->ExportRagdollSkeleton( entityClass, skeletons );

        // set debug info from exporter
        this->SetEditorInfo( entityClass );

        // save debug export data
        if( entityClass )
        {
            ncEditorClass * editorClass = entityClass->GetComponent<ncEditorClass>();
            if( editorClass )
            {
                editorClass->SaveAssetChunk();
            }
        }
    }

    // a root bone has been removed
    for(int lodLevel = 0; lodLevel< exportServer->GetSettings().lod.GetCountLOD() ; lodLevel++)
    {
        if (rootBones[lodLevel] != rootBones[0] )
        {
            N3DSERROR( animexport , ( 0 , "ERROR: Attention: You've removed a root bone at lod %i! Animations won't be correctly exported.", lodLevel));
            n_message( "Attention: You've removed a root bone at lod %i! Animations won't be correctly exported.", lodLevel);
        }
    }

    return true;
}
