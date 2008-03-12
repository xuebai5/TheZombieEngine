//------------------------------------------------------------------------------
//  n3dsnebulafunctions.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "pluginutilfunctions/n3dsnebulafunctions.h"

#include "kernel/ntypes.h"

#include <cstring>

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nscriptserver.h"
#include "kernel/nwin32loghandler.h"

#include "entity/ncomponentclassserver.h"
#include "entity/ncomponentobjectserver.h"

#include "util/nstringlist.h"

#include "misc/nprefserver.h"

//------------------------------------------------------------------------------
nNebulaUsePackage(nnebula);
nNebulaUsePackage(nmaterial);
nNebulaUsePackage(nentity);
nNebulaUsePackage(nscene);
nNebulaUsePackage(noctree);
nNebulaUsePackage(nspatial);
nNebulaUsePackage(nlua);
nNebulaUsePackage(nphysics);
nNebulaUsePackage(framework);
nNebulaUsePackage(zombieentityexp);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(nnavmesh);
nNebulaUsePackage(ncshared);
nNebulaUsePackage(n3dsexporterdeps);

//------------------------------------------------------------------------------
#define PATH_WC                    "CONJURER_WC"
#define PATH_TEMPORAL_WC           "CONJURER_WCTMP"

//------------------------------------------------------------------------------
namespace n3dsNebulaFunctions
{
    nKernelServer * kernelServer = 0;
    nScriptServer * scriptServer = 0;
    nAtomTable * atomTable = 0;
    nComponentClassServer * compClassServer = 0;
    nComponentObjectServer * compObjectServer = 0;

    const char * MATERIAL_TEMPLATE = "wc:libs/materials/*.n2";
    const char * GFX_MATERIAL_TEMPLATE = "wc:libs/gfxmaterials/*.n2";

    //------------------------------------------------------------------------------
    /**
        Open Nebula and the neccesary servers
    */
    void OpenNebula( )
    {
        if( !atomTable )
        {
            atomTable = nAtomTable::Instance();
        }
        if( !kernelServer )
        {
            kernelServer = n_new(nKernelServer);
        }
        if( !compClassServer )
        {
            compClassServer = n_new(nComponentClassServer);
        }
        if( !compObjectServer )
        {
            compObjectServer = n_new(nComponentObjectServer);
        }
        kernelServer->SetLogHandler( n_new(nWin32LogHandler("n3dsfunctionlog") ) );
        kernelServer->New( "nprofilermanager", "/sys/servers/profilemanager" );     
    
        kernelServer->AddPackage(nnebula);
        kernelServer->AddPackage(ndirect3d9);
        kernelServer->AddPackage(nentity);
        kernelServer->AddPackage(nscene);
        kernelServer->AddPackage(nmaterial);
        kernelServer->AddPackage(noctree);
        kernelServer->AddPackage(nspatial);
        kernelServer->AddPackage(nlua);
        kernelServer->AddPackage(nphysics);
        kernelServer->AddPackage(framework);
        kernelServer->AddPackage(ncshared);
        kernelServer->AddPackage(nnavmesh);
        kernelServer->AddPackage(n3dsexporterdeps);
        kernelServer->AddPackage(zombieentityexp);

        char* s = getenv("NEBULA2_HOME");// temporal solution
        n_assert2( s , "The environment variable NEBULA2_HOME is not defined");

        kernelServer->GetFileServer()->SetAssign("home", s );

        scriptServer = static_cast<nScriptServer*>( kernelServer->New("nluaserver", "/sys/servers/script") );
        kernelServer->New( "nsceneserver", "/sys/servers/scene"); // Need for clone ncSceneClass component
        kernelServer->New( "nprefserver", "/sys/servers/prefserver" );
        kernelServer->New( "nvariableserver", "/sys/servers/variable" );
        kernelServer->New( "nentityobjectserver", "/sys/servers/entityobject" );
        kernelServer->New( "nentityclassserver", "/sys/servers/entityclass" );
        

        // run the common Nebula initialization
        nString result;
        scriptServer->RunScript("home:data/scripts/startup.lua", result);
        scriptServer->RunFunction("OnStartup", result);
    }

    //------------------------------------------------------------------------------
    /**
        Close Nebula
    */
    void CloseNebula( )
    {
        nEntityClassServer::Instance()->Release();
        nEntityObjectServer::Instance()->Release();
        nPrefServer::Instance()->Release();
        scriptServer->Release();
        nLogHandler* log = kernelServer->GetLogHandler();

        if (compObjectServer)
        {
            n_delete(compObjectServer);
            compObjectServer = 0;
        }

        if (compClassServer)
        {
            n_delete(compClassServer);
            compClassServer = 0;
        }

        if (kernelServer)
        {
            n_delete( kernelServer );
            kernelServer = 0;
        }

        n_delete( log );
    }

    //------------------------------------------------------------------------------
    /**
        Free static variables, it only when detach process
    */
    void WhenDetachProcess()
    {
        if (atomTable)
        {
            n_delete(atomTable);
            atomTable = 0;
        }
    }

    //------------------------------------------------------------------------------
    /**
        Gets the entity classes names and the folders from a nStringList and its 
        childrens
        @param node the nStringList from the names we get
        @param folders folders in the node
        @param classes classes names in the node
        @param path prefix of the folders
    */
    void GetEntityClasses( nStringList * node, 
        nArray<nString*> & folders,
        nArray<nString*> & classes,
        const nString & path )
    {
        nString * name = 0;
        for( int i = 0 ; i < node->Size() ; ++i )
        {
            name = n_new( nString( path ) );
            name->Append( "/" );
            name->Append( node->GetString( i )->Get() );
            classes.Append( name );
        }

        nStringList * child = static_cast<nStringList*>( node->GetHead() );
        while( child )
        {
            name = n_new( nString( path ) );
            name->Append( "/" );
            name->Append( child->GetName() );
            folders.Append( name );
            GetEntityClasses( child, folders, classes, *name );
            child = static_cast<nStringList*>( child->GetSucc() );
        }
    }

    //------------------------------------------------------------------------------
    /**
        Get the entity classes and folders from grimoire.
        @param gamelib flag to use the game directories or the temporal
        @param numFolders number of folders returned
        @param folders folders returned
        @param numClasses number of classes returned
        @param classes classes returned
    */
    void GetEntityClasses( bool gamelib, int & numFolders, char *** folders, int & numClasses, char *** classes )
    {
        OpenNebula();

        nArray<nString> paths;
        nString grimoire = "wc:libs/grimoire/general.n2";

        if( ! gamelib )
        {
            char * s = getenv(PATH_TEMPORAL_WC);
            n_assert2( s , "enviroment variable if not defined\n"  );
            paths.Append( s );
        }

        char * s = getenv(PATH_WC);
        n_assert2( s , "enviroment variable if not defined\n" );
        if( s )
        {
            paths.Append( s );
        }

        kernelServer->GetFileServer()->SetAssignGroup("wc", paths );
        
        nStringList * node = static_cast<nStringList*>( 
            kernelServer->LoadAs( grimoire.Get(), "/grimoire/general" ) );

        nArray<nString*> foldersArray;
        nArray<nString*> classesArray;

        foldersArray.Append( n_new( nString( "general" ) ) );

        GetEntityClasses( node, foldersArray, classesArray, "general" );

        numFolders = foldersArray.Size();
        (*folders) = new( char * [numFolders] );

        for( int i = 0 ; i < numFolders ; ++i )
        {
            (*folders)[ i ] = strdup( foldersArray[ i ]->Get() );
            n_delete( foldersArray[ i ] );
        }

        numClasses = classesArray.Size();
        (*classes) = new( char * [numClasses] );

        for( int i = 0 ; i < numClasses ; ++i )
        {
            (*classes)[ i ]= strdup( classesArray[ i ]->Get() );
            n_delete( classesArray[ i ] );
        }

        CloseNebula();
    }

    //------------------------------------------------------------------------------
    /**
        Get the entity classes tha had a common parent
        @param gamelib flag to use the game directories or the temporal
        @param parent common parent of returned classes
        @param numClasses number of classes returned
        @param classes classes returned
    */
    void GetEntityClassesOf( bool gamelib, const char * parent, int & numClasses, char *** classes )
    {
        OpenNebula();

        nArray<nString> paths;
        if( ! gamelib )
        {
            char * s = getenv(PATH_TEMPORAL_WC);
            n_assert2( s , "enviroment variable if not defined\n"  );
            if( s )
            {
                paths.Append( s );
            }
        }

        char * s = getenv(PATH_WC);
        n_assert2( s , "enviroment variable if not defined\n" );
        if( s )
        {
            paths.Append( s );
        }

        kernelServer->GetFileServer()->SetAssignGroup("wc", paths );

        nArray<nString> subClasses;

        nEntityClassServer * entityServer = nEntityClassServer::Instance();
        n_assert( entityServer );
        if( entityServer )
        {
            entityServer->SetEntityClassDir("wc:classes/");
            entityServer->GetEntitySubClassNames( parent, subClasses );
        }

        numClasses = subClasses.Size();
        (*classes) = new( char * [numClasses] );

        for( int i = 0 ; i < numClasses ; ++i )
        {
            (*classes)[ i ]= strdup( subClasses[ i ].Get() );
        }

        CloseNebula();
    }

    //------------------------------------------------------------------------------
    /**
        Get the game material list
        @param gamelib flag to use the game directories or the temporal
        @param numMaterials number of materials returned
        @param materials materials returned
    */
    void GetMaterialList( bool gamelib, int & numMaterials, char *** materials )
    {
        GetFileList( gamelib, MATERIAL_TEMPLATE ,numMaterials, materials );
        
        if( numMaterials == 0 )
        {
            // return default material
            numMaterials = 1;
            (*materials) = new( char * [numMaterials] );
            (*materials)[ 0 ] = strdup( "default" );
        } 
    }

    //------------------------------------------------------------------------------
    /**
        Get the graphics game material list
        @param gamelib flag to use the game directories or the temporal
        @param numMaterials number of materials returned
        @param materials materials returned
    */
    void GetGfxMaterialList( bool gamelib, int & numMaterials, char *** materials )
    {
        GetFileList( gamelib, GFX_MATERIAL_TEMPLATE ,numMaterials, materials );
    }

    //------------------------------------------------------------------------------
    /**
        Get the game material list
        @param gamelib flag to use the game directories or the temporal
        @param numMaterials number of materials returned
        @param materials materials returned
    */
    void GetFileList( bool gamelib, const char* path, int & numMaterials, char *** materials )
    {
        OpenNebula();

        nArray<nString> paths;
        nString grimoire = "wc:libs/grimoire/general.n2";

        if( ! gamelib )
        {
            char * s = getenv(PATH_TEMPORAL_WC);
            n_assert2( s , "enviroment variable if not defined\n"  );
            paths.Append( s );
        }

        char * s = getenv(PATH_WC);
        n_assert2( s , "enviroment variable if not defined\n" );
        if( s )
        {
            paths.Append( s );
        }

        nFileServer2 * fileServer = kernelServer->GetFileServer();

        fileServer->SetAssignGroup("wc", paths );

        nArray<nString> files;

        fileServer->GetAllFilesFrom( path , files );

        if( files.Size() == 0 )
        {
            // return default material
            numMaterials = 0;
        }
        else
        {
            // remove '.' and '..' directories
            nString * index = 0;
            index = files.Find( "." );
            if( index )
            {
                files.Erase( index );
            }

            index = files.Find( ".." );
            if( index )
            {
                files.Erase( index );
            }

            // return file names
            numMaterials = files.Size();
            (*materials) = new( char * [numMaterials] );

            for( int i = 0; i < numMaterials ; ++i )
            {
                files[ i ].StripExtension();
                (*materials)[ i ] = strdup( files[ i ].Get() );
            }
        }

        CloseNebula();
    }

    
    //------------------------------------------------------------------------------
    /**
        Get the path of gfx materials, This allocate memory for new char
        @param materials materials returned
    */
    char* GetGfxMaterialPath()
    {
        nString path( getenv( PATH_WC ) );
        path+="/libs/gfxmaterials";
        char* copy = strdup( path.Get());
        return copy;
    }

    //------------------------------------------------------------------------------
    /**
        Get the path of working copy, This allocate memory for new char
        @param materials materials returned
    */
    char* GetWCPath()
    {
        const char* path = getenv( PATH_WC );
        char* copy = strdup( path);
        return copy;
    }

}// NAMESPACE n3dsNebulaFunctions

//------------------------------------------------------------------------------
