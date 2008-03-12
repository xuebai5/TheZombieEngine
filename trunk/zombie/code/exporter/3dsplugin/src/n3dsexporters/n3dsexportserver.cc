#include "precompiled/pchn3dsmaxexport.h"

//------------------------------------------------------------------------------
#include "n3dsexporters/n3dsexportserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nwin32loghandler.h"
#include "n3dsexporters/n3dsfileserver.h"
#include "n3dsexporters/n3dsbitmaps.h"
#include "n3dsexporters/n3dsSystemCoordinates.h"
#include "n3dsexporters/n3dslog.h"
#include "n3dsexporters/n3dsscenelist.h"
#include "n3dsmaterial/n3dsmateriallist.h"
#include "n3dsnebularuntime/n3dsnebularuntime.h"
#include "n3dsphysicexport/n3dsphysicexport.h"
#include "n3dsgraphicsexport/n3dsgraphicsexport.h"
#include "n3dsvisibilityexport/n3dsvisibilityexport.h"
#include "n3dsanimationexport/n3dsanimationexport.h"
#include "nasset/nentityassetbuilder.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/ncassetclass.h"
#include "nscene/ncscenelodclass.h"
#include "ndebug/nceditorclass.h"
#include "appinfo/appinfo.h"
#include "util/nstringlist.h"
#include "nspatial/ncspatialclass.h"
#include "zombieentity/ncdictionaryclass.h"
#include "nscene/ncscenelodclass.h"
#include "zombieentity/nctransformclass.h"
#include "nphysics/ncphysicsobjclass.h"

// @todo change the define for other form
#define PATH_WC                    "CONJURER_WC"
#define PATH_TEMPORAL_WC           "CONJURER_WCTMP"
#define PATH_TEMPORAL_WC2          "realwc:../wctmp2/"
#define PATH_ASSETS                "wc:export/assets/"
#define PATH_WCLEVEL               "wc:levels/exporter3ds/"
#define PATH_WCCLASSES             "wc:classes/"
#define PATH_WCTEXTURES            "wc:export/textures/"
#define PATH_ARTIST_REPO           "e:/renaissance/trunk/"


//------------------------------------------------------------------------------
n3dsExportServer* n3dsExportServer::server=0;

const char pathAnimators[]     =  "/usr/animators/anims";
const char pathCameras[] = "/usr/cameras";

//------------------------------------------------------------------------------
/**
*/
n3dsExportServer*
n3dsExportServer::Instance()
{
    if(server == 0)
    {
        server =n_new(n3dsExportServer);
    }
    n_assert(server);
    return server;
}

//------------------------------------------------------------------------------
/**
*/
n3dsExportServer::n3dsExportServer():
    iGameInterface(0),
    maxInterface(0),
    nebula(0),
    mtlList(0),
    coord(0),
    visibilityExport(0),
    physicExport(0),
    graphicsExport(0),
    animationExport(0),
    name(""),
    windowsPath(""),
    preOrder(0),
    postOrder(0),
    inIdOrder(0),
    queueFileServer(0)
    //showLog(false)
{
}

//------------------------------------------------------------------------------
/**
    Create interface with igame a 3dsmax scene
    Initialize system coordinates ( IGame to nebula)
    Open Interface with nebula runtime
    @param localName the name of file to export

*/
bool
n3dsExportServer::init( const char* localName, bool exportSelected)
{
    windowsPath=localName;
    windowsPath.StripExtension();
    name=windowsPath.ExtractFileName();


    if (maxInterface || iGameInterface)
    {
        n_error("Export server only init one time");
    }

    this->GetNebula()->Open(windowsPath);
    fileLog.Clear();
    nWin32LogHandler* windowhandler = (nWin32LogHandler*) nKernelServer::Instance()->GetLogHandler();
    fileLog = windowhandler->GetAbsolutePath();
    fileLog += "\"";
    fileLog = "\"" + fileLog;
    n3dsLog::SetFileLog(fileLog.Get());

    queueFileServer = n_new( n3dsFileServer );


    // Get from scene export settings and validate it
    if ( ! GetAndCheckExportSetting( exportSelected ) )
    {
        n_message( "incorrect export settings" );
        return false;
    }

    // Create all directories ... by export settings
    if ( ! ConfigureByExportSettings() )
    {
        return false;
    }

    // Comprobe its posible create class
    if ( ! exportSettings.CheckValidExport() )
    {
       return false;
    }


    this->maxInterface   = GetCOREInterface();
    n_assert(maxInterface);

    this->iGameInterface = GetIGameInterface();
    n_assert(iGameInterface);

    this->coord = n_new(n3dsSystemCoordinates);
    this->GetSystemCoordinates()->SetCoordinates();

    if ( 0 == maxInterface->GetRootNode()->NumberOfChildren() )
    {
        /// when  the scene not has a node then the function  iGameInterface->InitialiseIGame fails
        n_message(" The scene is emty");
        return false;
    } 
   
    this->iGameInterface->InitialiseIGame(exportSelected);
	this->iGameInterface->SetStaticFrame(0);

    // Create the preOder and postOrder List from scene
    int numElements = GetIGameScene()->GetTotalNodeCount();
    this->preOrder  = n_new( n3dsSceneList( numElements, numElements ) ); //inital size for nArray
    this->postOrder = n_new( n3dsSceneList( numElements, numElements ) );
    this->inIdOrder = n_new( n3dsSceneList( numElements, numElements ) );

    
    for(int loop = 0; loop < GetIGameScene()->GetTopLevelNodeCount(); loop++)
    {
        IGameNode* node = GetIGameScene()->GetTopLevelNode(loop);
        AppendToSceneList( node );
	}
    *inIdOrder = *postOrder;
    inIdOrder->SortMaxId();

    // Create  a physics exporter
    this->physicExport = n_new( n3dsPhysicExport ); 

    // create visibility exporter if necessary
    this->visibilityExport = n_new ( n3dsVisibilityExport );

    // Create  a graphics exporter
    this->graphicsExport = n_new( n3dsGraphicsExport );

    // Create  an animation exporter
    this->animationExport = n_new( n3dsAnimationExport ); 

    return true;
}


//------------------------------------------------------------------------------
/**
*/
void 
n3dsExportServer::AppendToSceneList( IGameNode* node)
{
    this->preOrder->Append( n3dsObject( node ) );

    for( int i=0; i < node->GetChildCount(); i++ )
    {
        IGameNode* child = node->GetNodeChild(i);
        AppendToSceneList( child );
    }

    this->postOrder->Append( n3dsObject( node ) );
}




//------------------------------------------------------------------------------
/**
    Launch the file export with default application
*/
void
n3dsExportServer::RunScene()
{
    if( (( ! (this->GetSettings().exportMode == n3dsExportSettings::ExportModeType::Character )) ||
         ( this->GetSettings().animMode != n3dsExportSettings::CharacterModeType::Skeleton)    ) &&
         this->GetSettings().openConjurer )
    {
        nString document;
        if ( GetSettings().gameLibrary )
        {
            document = "realwc:classes/";
        } else
        {
            document = "tmpwc:classes/";
        }

        document += this->GetSettings().entityName;

        // when there's lod and we're visualizing an animation, the maximum lod skeleton has to be opened
        if( this->GetSettings().lod.GetCountLOD() != 0  && this->GetSettings().exportMode == n3dsExportSettings::ExportModeType::Character && this->GetSettings().animMode == n3dsExportSettings::CharacterModeType::SkinAnimation)
        {
            document += "_0";
        }

        document += ".n2";

        document = nFileServer2::Instance()->ManglePath( document.Get() , true );

        ShellExecute(NULL,                   // hWnd
            "",
            document.Get(),    // lpFile
            "",  // lpParameters
            windowsPath.Get(), // lpDirectory
            SW_SHOW);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsExportServer::Close()
{
    if ( postOrder != 0)
    {
        const n3dsSceneList& list = this->GetPostOrderScene();
        for ( n3dsSceneList::iterator i( list.Begin() ); i != list.End() ; ++i ) 
        {
            n3dsObject& object = (*i);
            object.GetNode()->ReleaseIGameObject();
        }
    }
   

    if ( iGameInterface)
    {
        iGameInterface->ReleaseIGame();
        iGameInterface = 0;
    }


    if ( mtlList      != 0) 
    {
        n_delete( mtlList );
        mtlList = 0;
    }

    if ( physicExport != 0) 
    {
        n_delete( physicExport );
        physicExport = 0;
    }

    if ( graphicsExport != 0)
    {
        n_delete( graphicsExport);
        graphicsExport = 0; 
    }

    if ( visibilityExport != 0)
    {
        n_delete( visibilityExport);
        visibilityExport = 0;
    }

    if ( animationExport != 0)
    {
        n_delete( animationExport );
        animationExport = 0;
    }

    if ( coord        != 0)
    {
        n_delete( coord );
        coord = 0;
    }

    if ( postOrder    != 0) 
    {
        n_delete( postOrder );
        postOrder = 0;
    }

    if ( preOrder     != 0)
    {
        n_delete( preOrder );
        preOrder = 0;
    }

    if ( inIdOrder    != 0) 
    {
        n_delete( inIdOrder );
        inIdOrder = 0;
    }

    if (queueFileServer !=0 )
    {
       n_delete( queueFileServer );
       queueFileServer = 0;
    }
    //if (this->assetBuilder !=0 ) n_delete( this->assetBuilder );


    if ( nebula  != 0)
    {
        n_delete( nebula ); // ddelete last the nebula, this has a kernel server.
        nebula = 0;
    }
}


//------------------------------------------------------------------------------
/**
*/
bool 
n3dsExportServer::Save()
{
    bool result = true;

    if ( !nEntityClassServer::Instance()->SaveEntityClasses() )
    {
        return false;
    }

    if ( !nEntityObjectServer::Instance()->SaveConfig() )
    {
        return false;
    }

    PutInGrimoire();

    if ( GetSettings().gameLibrary )
    {
        if ( n3dsLog::GetHasError() )
        {
            result= false;
            n_message (" Not export in working copy\n The scene has problem");
        } else
        {
            result = GetQueueFileServer()->QueueCopyWC();
            result |= GetQueueFileServer()->Flush();
            if (!result)
            {
                n_message ("don't copy to original working copy,\n apply the revert command");
            }
        }
    } else
    {
        result = GetQueueFileServer()->Flush();
        if (!result)
        {
            n_message ("don't copy any files to temporal working copy,\n");
        }
        result = true; //Always open the conjurer in gamelibrary mode
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsExportServer::PutInGrimoire()
{
        // if skeleton or animation, do not create thumbnail
    if ( ! ((this->GetSettings().exportMode == n3dsExportSettings::ExportModeType::Character )&&
            ( this->GetSettings().animMode == n3dsExportSettings::CharacterModeType::SkinAnimation ||
              this->GetSettings().animMode == n3dsExportSettings::CharacterModeType::MorpherAnimation || 
              this->GetSettings().animMode == n3dsExportSettings::CharacterModeType::Skeleton)))
    {
        nString grimoirePath( "wc:libs/grimoire/general.n2" );
        nStringList * grimoire = static_cast<nStringList*>( 
            nKernelServer::ks->LoadAs( grimoirePath.Get(), "/grimoire/general" ) );
        
        nString libraryFolder( GetSettings().libraryFolder );
        // delete general root
        int index = libraryFolder.FindChar( '/', 0 );
        libraryFolder = libraryFolder.ExtractRange( index,libraryFolder.Length() - index );
        libraryFolder = libraryFolder.TrimLeft( "/" );
        libraryFolder.Append( "/" );

        nString component( libraryFolder );
        component.Append( GetSettings().entityName );
        grimoire->AddComponent( component );


        //// Put in grimoire dthe lod class
        //nString entityName = GetSettings().entityName;
        //libraryFolder += entityName + "_subc/";

        //if (GetSettings().exportMode == n3dsExportSettings::Indoor )
        //{
        //    entityName = nEntityAssetBuilder::GetIndoorFacadeClassName(entityName);
        //    nString component2(libraryFolder);
        //    component2+= entityName;
        //    grimoire->AddComponent( component2 );
        //}

        //for ( int idx = 0; idx < GetSettings().lod.GetCountLOD() ; ++idx)
        //{
        //    nString component2(libraryFolder);
        //    component2+= entityName + "_lod";
        //    component2.Append( idx );
        //    grimoire->AddComponent( component2 );
        //}

        nFileServer2::Instance()->MakePath( grimoirePath.ExtractDirName().Get() );
        grimoire->SaveAs( grimoirePath.Get() );
        grimoire->Release();

        nString thumbnailPath( nEntityAssetBuilder::GetDefaultAssetPath( GetSettings().entityName ) );
        thumbnailPath.Append( "/debug/thumb.jpg" );
        if ( this->GetSettings().exportThumbNail )
        {
            nFileServer2::Instance()->MakePath( thumbnailPath.ExtractDirName().Get() );
            n3dsBitmaps::CreateThumbNail( thumbnailPath );
        } else if ( nFileServer2::Instance()->FileExists(thumbnailPath) )
        {
            //trick for no delete thumbnails in asset
            this->GetQueueFileServer()->QueueCopyFile( thumbnailPath , thumbnailPath, GetSettings().gameLibrary );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsExportServer::SetEditorInfo( nEntityClass * entityClass )
{
    ncEditorClass * editorClass = entityClass->GetComponent<ncEditorClass>();
    n_assert( editorClass );
    if( editorClass )
    {
        editorClass->SetAssetKeyInt( "releaseNumber", N_RELEASE_NUMBER );
        nString maxFileName = static_cast<char*>(GetMaxInterface()->GetCurFilePath() );
        maxFileName.ConvertBackslashes();
        maxFileName = maxFileName.ReplaceBegin( PATH_ARTIST_REPO , "/" );
        editorClass->SetAssetKeyString( "3dsMaxFile" , maxFileName );
        // subversion keys
        editorClass->SetAssetKeyString( "svnRevision", "$Revision$" );
        editorClass->SetAssetKeyString( "svnHeadURL", "$HeadURL$" );
        editorClass->SetAssetKeyString( "svnDate", "$Date$" );
        editorClass->SetAssetKeyString( "svnAuthor", "$Author$" );
    }
}

//------------------------------------------------------------------------------
/**
*/
bool  
n3dsExportServer::Export()
{    
    bool result = true;
    switch( GetSettings().exportMode  )
    {
    case n3dsExportSettings::Brush:
        result = this->ExportBrush(); 
        break;

    case n3dsExportSettings::Character:
        this->ExportCharacterResources();
        break;
    case n3dsExportSettings::Indoor:
        result = this->ExportIndoor();
        break;

    default:
        break;
    }

    return result;
}

//-----------------------------------------------------------------------------
/**
    Get from scene the export settings
    and check its is valids
    @return the result of check , false if it is invalid
*/
bool
n3dsExportServer::GetAndCheckExportSetting(bool exportSelected)
{
    if (!exportSelected)
    {
        exportSettings = n3dsExportSettings::GetFromScene();
        return exportSettings.CheckValidSetting();
    } else
    {
        exportSettings.entityName = "N3dselectedbrush";
        exportSettings.binaryResource = true;
        exportSettings.exportMode = n3dsExportSettings::Brush;
        exportSettings.gameLibrary = false;
        exportSettings.valid = true;
        return true;
    }
}


//-----------------------------------------------------------------------------
/**
    Configure by exort settings
    Configure the assigns, assigns group and file resource
*/

bool  
n3dsExportServer::ConfigureByExportSettings()
{
    nArray<nString> paths;
    nFileServer2* const fileServer = this->GetNebula()->GetFileServer();

    // Set The "wc:" assigns group

    // if export to temporal directory set first

    if ( this->exportSettings.gameLibrary )
    {
        paths.Append( PATH_TEMPORAL_WC2  );
        fileServer->SetAssign("tmpwc", PATH_TEMPORAL_WC2  );
    } else
    {
        char* var = getenv( PATH_TEMPORAL_WC );
        n_assert2( var , "enviroment variable if not defined\n" );
        paths.Append( var  ); 
        fileServer->SetAssign("tmpwc", var  );
    }

    char* var = getenv( PATH_WC );
    n_assert2( var , "enviroment variable if not defined\n" );
    
    paths.Append( var );
    fileServer->SetAssignGroup( "wc" , paths );
    fileServer->SetAssign("realwc", var );


    if ( this->exportSettings.gameLibrary )
    {
        bool res = GetQueueFileServer()->DeleteDirectory( PATH_TEMPORAL_WC2 ); // empty the temporal working export
        if ( !res)
        {
            n_message( "Dont delete the temporal directoty\n %s" , PATH_TEMPORAL_WC2 ); 
            return false;
        }
    }


    // Set the "wcclases"
    fileServer->SetAssign( "wcclasses", PATH_WCCLASSES );

    // temporal level for export
    fileServer->SetAssign( "wclevel" ,  PATH_WCLEVEL );

    fileServer->SetAssign( "wctextures", PATH_WCTEXTURES);


    //Create directories ( only create it if not exists)
    fileServer->MakePath("wcclasses:");
    fileServer->MakePath("wctextures:");
    fileServer->MakePath("wclevel:config");
    fileServer->MakePath("wclevel:objects");

    // Config the EntityServer
    nEntityClassServer::Instance()->SetEntityClassDir( "wcclasses:" );
    nEntityObjectServer::Instance()->SetConfigDir( "wclevel:config" );
    nEntityObjectServer::Instance()->SetEntityObjectDir( "wclevel:objects" );
    
    return true;

}

//------------------------------------------------------------------------------
/**
    Copy components from class, compatibility with neBrushClass , 
    neMirageClass and neSimpleBrushClass
*/
void
n3dsExportServer::CopyClassComponents( nEntityClass* srcClass,  nEntityClass* dstClass) const
{
    if ( dstClass->GetComponent<ncDictionaryClass>() )
    {
        dstClass->CopyComponentFrom( srcClass->GetComponent<ncDictionaryClass>() );
    }

    
    if ( dstClass->GetComponent<ncTransformClass>() )
    {
        dstClass->CopyComponentFrom( srcClass->GetComponent<ncTransformClass>() );
    }
    

    if ( dstClass->GetComponent<ncAssetClass>() )
    {
        dstClass->CopyComponentFrom( srcClass->GetComponent<ncAssetClass>() );
    }

    if ( dstClass->GetComponent<ncSceneLodClass>() )
    {
        dstClass->CopyComponentFrom( srcClass->GetComponent<ncSceneLodClass>() );
    }

    if ( dstClass->GetComponent<ncSpatialClass>() )
    {
        dstClass->CopyComponentFrom( srcClass->GetComponent<ncSpatialClass>() );
    }

    if ( dstClass->GetComponent<ncPhysicsObjClass>() )
    {
        dstClass->CopyComponentFrom( srcClass->GetComponent<ncPhysicsObjClass>() );
    }

    if ( dstClass->GetComponent<ncEditorClass>() )
    {
        dstClass->CopyComponentFrom( srcClass->GetComponent<ncEditorClass>() );
    }
}

//------------------------------------------------------------------------------
/**
*/
nEntityClass* 
n3dsExportServer::MutateClass( nEntityClass* oldClass,  nEntityClass* newBaseClass) const
{
    n_assert( oldClass );
    n_assert( newBaseClass );
    nString className( oldClass->GetName() );
    const nString swapClassName("N3ds_temporal_object_for_mutate" );
    nEntityClass* newClass;
   
    nEntityClass* swapClass = nEntityClassServer::Instance()->NewEntityClass( newBaseClass, swapClassName.Get() );
    if ( ! swapClass )
    {
        n_message( "The wc has a error, the class \"N3ds_temporal_object_for_mutate\"  is in repository " );
        return 0;
    }


    this->CopyClassComponents( oldClass , swapClass );

    /// trick for no delete from real working copy
    nEntityClassServer::Instance()->SaveEntityClass( oldClass );
    nEntityClassServer::Instance()->RemoveEntityClass( className.Get() );
    oldClass = 0;

    newClass = nEntityClassServer::Instance()->NewEntityClass( newBaseClass, className.Get() );
    if ( ! newClass )
    {
        n_message(  "Problem for mutate a class \"%s\"",  className.Get() );
        return 0;
    }

    this->CopyClassComponents( swapClass, newClass );
    nEntityClassServer::Instance()->RemoveEntityClass( swapClassName.Get() );

    return newClass;

}


//-----------------------------------------------------------------------------
/**
*/
nEntityClass*
n3dsExportServer::GetExporterEntityClass(const nString& base, const nString& className, const nString& otherBase1, const nString& otherBase2, const nString& otherBase3) const
{
    nEntityClass * baseClass = nEntityClassServer::Instance()->GetEntityClass( base.Get() );
    n_assert( baseClass );
    if( ! baseClass )
    {
        n_message( "The base class \"%s\" not exist", base.Get() );
        return 0;
    }

    nEntityClass * entityClass = nEntityClassServer::Instance()->GetEntityClass( className.Get() ) ;


    if ( 0 != entityClass && entityClass->GetSuperClass() != baseClass )
    {
        bool valid = false;

        valid |=  ! otherBase1.IsEmpty() && entityClass->GetSuperClass() == nEntityClassServer::Instance()->GetEntityClass( otherBase1.Get() );
        valid |=  ! otherBase2.IsEmpty() && entityClass->GetSuperClass() == nEntityClassServer::Instance()->GetEntityClass( otherBase2.Get() );
        valid |=  ! otherBase3.IsEmpty() && entityClass->GetSuperClass() == nEntityClassServer::Instance()->GetEntityClass( otherBase3.Get() );
        
        if (valid)
        {
            //keeps existing class data
            entityClass = this->MutateClass( entityClass , baseClass );
        } else
        {
            n_message( "The \"%s\" exist, is a \"%s\" \n select another name",  className.Get() , entityClass->GetSuperClass()->GetName() );
            return 0;
        }
    }

   
    // create new class
    if ( 0 == entityClass )
    {
        // Create it
        entityClass = nEntityClassServer::Instance()->NewEntityClass( baseClass, className.Get() );
        // Configure the loader asset by default
        ncLoaderClass* loader = entityClass->GetComponent<ncLoaderClass>();
        if (loader)
        {
            loader->SetDefaultResourceFile();
            nFileServer2::Instance()->MakePath( loader->GetResourceFile() );
        }
        n_assert( entityClass && entityClass->nClass::IsA( base.Get() ) );
    } else
    {
        // nothing
        // Export keeps existing class data
    }
	
	nEntityClassServer::Instance()->SetEntityClassDirty( entityClass , true );

    return entityClass;
}


DWORD WINAPI n3dsExportServerdummybarfn(LPVOID /*arg*/)
{
    return(0);
}

//-----------------------------------------------------------------------------
/**
*/
void
n3dsExportServer::ProgressBarStart(const nString& title)
{
    this->barTitle  = title;
    GetCOREInterface()->ProgressStart( _T((char*)barTitle.Get() ) , TRUE, n3dsExportServerdummybarfn, 0 );
}
//-----------------------------------------------------------------------------
/**
*/
void
n3dsExportServer::ProgressUpdate(float val, nString name )
{
    GetCOREInterface()->ProgressUpdate( (int)( 100.0f* val ) ,  FALSE ,  (TCHAR*)name.Get() ); 
}

//-----------------------------------------------------------------------------
/**
*/
void
n3dsExportServer::ProgressBarEnd()
{
    GetCOREInterface()->ProgressEnd();
}

//------------------------------------------------------------------------------
/**
    if the nebula interface not existed then create it
    @return a nebula interface
*/
n3dsNebulaRuntime*
n3dsExportServer::GetNebula()
{
    if (nebula == 0)
    {
        nebula = n_new(n3dsNebulaRuntime);
        n_assert(nebula);
    }
    return nebula;
}

//------------------------------------------------------------------------------
/**
    @return The IGame interface
*/
IGameScene*
n3dsExportServer::GetIGameScene()
{
    n_assert(iGameInterface)
    return iGameInterface;
}

//------------------------------------------------------------------------------
/**
    @return The 3ds Max Interface
*/
Interface*
n3dsExportServer::GetMaxInterface()
{
    n_assert(maxInterface)
    return maxInterface;
}
//------------------------------------------------------------------------------
/**
    if the system coordinates not existed then create it
    @return a system coordinates
*/
n3dsSystemCoordinates*
n3dsExportServer::GetSystemCoordinates()
{
    if (coord == 0)
    {
        coord = n_new(n3dsSystemCoordinates());
        n_assert(coord);
    }
    return coord;
}


//------------------------------------------------------------------------------
/**
*/
n3dsExportServer::~n3dsExportServer()
{
    Close();
    server = 0;
}

//------------------------------------------------------------------------------
/**
*/
n3dsVisibilityExport*
n3dsExportServer::GetVisibility()
{
    return this->visibilityExport;
}

//------------------------------------------------------------------------------
/**
*/
n3dsPhysicExport*           
n3dsExportServer::GetPhysics()
{
    return this->physicExport;
}

//------------------------------------------------------------------------------
/**
*/
n3dsGraphicsExport*           
n3dsExportServer::GetGraphics()
{
    return this->graphicsExport;
}

//------------------------------------------------------------------------------
/**
*/
n3dsAnimationExport*           
n3dsExportServer::GetAnimation()
{
    return this->animationExport;
}

//-----------------------------------------------------------------------------
/**
    Configure by exort settings
    Configure the assigns, assigns group and file resource
*/
n3dsFileServer*
n3dsExportServer::GetQueueFileServer()
{
    n_assert(queueFileServer);
    return queueFileServer;
}

//------------------------------------------------------------------------------
/**
    if the material list  not existed then create it
    @return material list
    
*/
n3dsMaterialList*
n3dsExportServer::GetMtlList()
{

    if (mtlList == 0)
    {
        this->mtlList = n_new(n3dsMaterialList);
        n_assert(mtlList)
    }
    return mtlList;
}

//------------------------------------------------------------------------------
/**
    @return a scene's name
*/
const char*
n3dsExportServer::GetName()
{
    return this->name.Get();
}


//------------------------------------------------------------------------------
/**
    @return a windows path of file
*/
const char*
n3dsExportServer::GetPath()
{
    return this->windowsPath.Get();
}

//------------------------------------------------------------------------------
/**
*/
nArray<n3dsAssetInfo> 
n3dsExportServer::CreateEntityClassesForLOD(nEntityClass* entityClass)
{
    int countLOD = exportSettings.lod.GetCountLOD() ;
    nArray<n3dsAssetInfo> assetPathArray;
    n3dsAssetInfo lodAsset;

    ncSpatialClass* spatialComponent = entityClass->GetComponent<ncSpatialClass>();
    n_assert(spatialComponent);
    ncSceneLodClass* lodComponent = entityClass->GetComponent<ncSceneLodClass>();
    n_assert(lodComponent);
    ncAssetClass* assetComponent = entityClass->GetComponent<ncAssetClass>();
    n_assert(assetComponent);

    if (countLOD > 0)
    {
        float previousDistance = 0.0f;
        for (int level = 0;  level < exportSettings.lod.GetCountLOD() ; ++level)
        {
            float currentDistance = exportSettings.lod.GetDistance( level ) ;
            lodAsset.assetPath = nEntityAssetBuilder::GetDefaultAssetPathForLOD( entityClass , level );
            assetPathArray.Append(lodAsset);
            assetComponent->SetLevelResource(level, lodAsset.assetPath.Get());
            spatialComponent->SetLevelDistance(level, currentDistance);
            // keep ranges for scene component for backwards compatibility
            lodComponent->SetLevelRange(level, previousDistance, currentDistance);
            previousDistance = currentDistance;
        }
    }
    else
    {
        lodAsset.assetPath = nEntityAssetBuilder::GetAssetPath(entityClass);
        //lodAsset.assetPath = nEntityAssetBuilder::GetDefaultAssetPathForLOD( entityClass , 0 );
        //assetComponent->SetLevelResource(0, lodAsset.assetPath.Get());
        //spatialComponent->SetLevelDistance(0, 1000000);
        assetPathArray.Append(lodAsset);
        //lodComponent->SetLevelRange(0, 0, 1000000);
    }

    return assetPathArray;
}

//------------------------------------------------------------------------------
/**
*/
nArray<n3dsAssetInfo> 
n3dsExportServer::CreateEntityClassesForRagdoll(nEntityClass* entityClass)
{
    int countLOD = exportSettings.lod.GetCountLOD() ;
    nArray<n3dsAssetInfo> assetPathArray;
    n3dsAssetInfo lodAsset;

    ncAssetClass* assetComponent = entityClass->GetComponent<ncAssetClass>();
    n_assert(assetComponent);

    if (countLOD > 0)
    {
        for (int level = 0;  level < exportSettings.lod.GetCountLOD() ; ++level)
        {
            lodAsset.assetPath = nEntityAssetBuilder::GetDefaultAssetPathForRagdoll( entityClass , level );
            assetPathArray.Append(lodAsset);
            assetComponent->SetRagdollResource(level, lodAsset.assetPath.Get());
        }
    }
    else
    {
        int level = 0;
        lodAsset.assetPath = nEntityAssetBuilder::GetDefaultAssetPathForRagdoll(entityClass, level);
        assetPathArray.Append(lodAsset);
        assetComponent->SetRagdollResource(level, lodAsset.assetPath.Get());
    }

    return assetPathArray;
}

//------------------------------------------------------------------------------
/**
*/
IGameNode*
n3dsExportServer::GetCamera()
{
    n3dsSceneList list = n3dsExportServer::Instance()->GetMaxIdOrderScene();
    n3dsSceneList::iterator index( list.Begin( n3dsObject::InvalidType ) );

    for ( ; index != list.End() ; ++index)
    {
        IGameNode* node = (*index).GetNode();
        if( node->GetIGameObject()->GetIGameType() == IGameObject::IGAME_CAMERA)
        {
            return node;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsExportServer::SetOptimizeMeshData( bool optimize)
{
    this->exportSettings.optimizeMesh = optimize;
}

//------------------------------------------------------------------------------
/**
*/
/*
void 
n3dsExportServer::SetLODTo(nEntityClass* entityClass, nArray<nEntityClass*> entityClassArray)
{
    //Copy the spatial Element
    ncSpatialClass* spatCompLOD0 = entityClassArray[0]->GetComponent<ncSpatialClass>();
    n_assert(spatCompLOD0);
    ncSpatialClass* spatComp = entityClass->GetComponent<ncSpatialClass>();
    n_assert(spatComp);

    bbox3 spatialBBox = *spatCompLOD0->GetOriginalBBox( );
    spatComp->SetOriginalBBox( spatialBBox);
   
}
*/
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------

