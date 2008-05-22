//-----------------------------------------------------------------------------
//  ngamematerialserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchgameplay.h"
#include "gameplay/ngamematerialserver.h"
#include "gameplay/ngamematerial.h"
#include "nphysics/nphymaterial.h"
#include "nphysics/nphysicsserver.h"
#include "kernel/nfileserver2.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nGameMaterialServer, "nroot");

//-----------------------------------------------------------------------------

nGameMaterialServer* nGameMaterialServer::Singleton(0);

nGameMaterialServer::tContainerGameMaterials nGameMaterialServer::listMaterials( NumInitialMaterialsSpace, NumMaterialsGrowthPace );

int nGameMaterialServer::MaterialsSize = 0;

//-----------------------------------------------------------------------------

namespace
{
    const char* MaterialsPath("/usr/gamematerials/");
    const char* FileMaterialsPath("wc:libs/materials/");
    const char* DefaultMaterialName( "defaultmaterial" );
}

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 21-Oct-2005   Zombie         created
*/
nGameMaterialServer::nGameMaterialServer()
{
    n_assert2( !Singleton , "Trying to instanciate a second instance of a singleton" );

    Singleton = this;
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 21-Oct-2005   Zombie         created
*/
nGameMaterialServer::~nGameMaterialServer()
{
    // destroying data
    this->Destroy();

    // Has to be the last line.
    Singleton = 0;
}

//-----------------------------------------------------------------------------
/**
    Get instance pointer

    @return pointer to the only instance of this object

    history:
        - 21-Oct-2005   Zombie         created
*/
nGameMaterialServer* nGameMaterialServer::Instance()
{
    n_assert2( Singleton , "Accessing to the physics server without instance." );

    return Singleton;
}

//-----------------------------------------------------------------------------
/**
    Destroys server's data

    history:
        - 21-Oct-2005   Zombie         created
*/
void nGameMaterialServer::Destroy()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Returns a material type trough the id.

    @return a material otherwise a null pointer

    history:
        - 01-Aug-2004   Zombie        created
        - 28-Oct-2005   Zombie        moved from nGameMaterial
*/
nGameMaterial* nGameMaterialServer::GetMaterial( const unsigned int id ) const
{
    nGameMaterial* material;

    if( listMaterials.Find( id, material ) )
        return material;

    return 0;
}

//-----------------------------------------------------------------------------
/**
    Creates a material.

    @param name new material's name

    @return a material otherwise a null pointer

    history:
        - 28-Oct-2005   Zombie          created
*/
nGameMaterial* nGameMaterialServer::CreateMaterial( const nString& name )
{
#ifndef NGAME
    nGameMaterial* check(0);
    // check the name it's not been used before
    bool found( this->listMaterials.Find( nTag( name ).KeyMap(), check ) );

    if( found == true )
    {
        n_assert2_always( "There's a material already with this name." );
        return 0;
    }
#endif

    // NOH Path
    nString nohPath(MaterialsPath);
    
    nohPath += name;

    nGameMaterial* material(static_cast<nGameMaterial*>
        (nKernelServer::Instance()->New( "ngamematerial", nohPath.Get())));

    n_assert2( material, "Failed to create the game material object." );

    // adds itself
    material->Add();

    return material;
}

//-----------------------------------------------------------------------------
/**
    Updates list of materials.

    history:
        - 01-Aug-2004   Zombie        created
        - 28-Oct-2005   Zombie        moved from nGameMaterial
*/
void nGameMaterialServer::UpdateList()
{
    // NOTE: for future use, where some information has to be updated
    nPhyMaterial::UpdateTableMaterialContacts();
}


//-----------------------------------------------------------------------------
/**
    Returns a material type trough the name.

    @param name material's name
    
    @return material if any

    history:
        - 28-Oct-2005   Zombie        created
*/
nGameMaterial* nGameMaterialServer::GetMaterialByName( const nString& name ) const
{
    return this->GetMaterial( nTag( name ).KeyMap() );
}


//-----------------------------------------------------------------------------
/**
    Adds an already existing material.

    @param material a material
    
    history:
        - 31-Oct-2005   Zombie        created
*/
void nGameMaterialServer::Add( nGameMaterial* material )
{
    n_assert2( material, "Error, no material present." );

#ifndef NGAME
    bool check( strncmp( material->GetFullName().Get(), MaterialsPath,strlen( MaterialsPath )) ? true:false );
    // checking it's in the right place
    n_assert2( !check, "The path it isn't in the right place" );

    if( check )
    {
        return;
    }
#endif
    // creates
    material->Create();

    // adding it to the list
    listMaterials.Add( material->GetMaterialId(), material );    

    // updates the list of materials
    this->UpdateList();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Returns if any of the materials is dirty (changed).

    @return true/false
    
    history:
        - 31-Oct-2005   Zombie        created
*/
const bool nGameMaterialServer::IsDirty() const
{
    for( int index(0); index < listMaterials.Size(); ++index )
    {
        if( listMaterials.GetElementAt( index )->IsDirty() )
        {
            return true;
        }
    }

    return false;
}

#endif

//-----------------------------------------------------------------------------
/**
    Inits object.

    @param ignored
    
    history:
        - 02-Nov-2005   Zombie        created
*/
void nGameMaterialServer::InitInstance(nObject::InitInstanceMsg)
{
    // Load materials
    this->LoadMaterials();
}

//-----------------------------------------------------------------------------
/**
    Loads materials.

    history:
        - 02-Nov-2005   Zombie        created
*/
void nGameMaterialServer::LoadMaterials()
{
    // load physics materials
    nPhysicsServer::Instance()->LoadMaterials();

    nFileServer2* fserver(nKernelServer::Instance()->GetFileServer());

    n_assert2( fserver, "File server required." );

    nArray<nString> files(fserver->ListFiles( FileMaterialsPath ));

    for( int index(0); index < files.Size(); ++index )
    {
        nString NOHPathName( MaterialsPath );

        NOHPathName.Append(files[ index ].ExtractFileName());
        NOHPathName.StripExtension();
        
        // create an object first
        nGameMaterial* gmaterial( static_cast<nGameMaterial*>(
            nKernelServer::Instance()->New( "ngamematerial", NOHPathName.Get() )));

        n_assert2( gmaterial, "Failed to createa a Game Material." );

        // load scripting info
        nKernelServer::Instance()->PushCwd( gmaterial );
        nKernelServer::Instance()->Load( files[ index ].Get(), false );
        nKernelServer::Instance()->PopCwd();

        gmaterial->Add();
    }
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Removes a material.

    @param material a game material

    history:
        - 02-Nov-2005   Zombie        created
*/
void nGameMaterialServer::RemoveMaterial(nGameMaterial* material)
{
    n_assert2( material, "Missing game material." );

    // removing a game material from the servers list
    listMaterials.Rem( nTag( material->GetName() ).KeyMap() );

    // removing the file in the repository
    nFileServer2* fserver(nKernelServer::Instance()->GetFileServer());

    n_assert2( fserver, "Missing file server." );

    nString filePathName( FileMaterialsPath );

    filePathName += material->GetName();

    filePathName += ".n2";

    if( fserver->FileExists( filePathName ) )
    {
        // deleting the material file
        fserver->DeleteFile( filePathName );
    }
        
    // destroying the object
    material->Release();
}

//-----------------------------------------------------------------------------
/**
    Removes a material from the server.

    @param material a game material

    history:
        - 04-Nov-2005   Zombie        created
*/
void nGameMaterialServer::Remove(nGameMaterial* material)
{
    n_assert2( material, "Missing game material." );

    if( !strcmp( material->GetName(),DefaultMaterialName ) )
    {
        n_assert2_always( "The default material can't be removed." );
        return;
    }

    if( nPhysicsServer::Instance()->ItsGameMaterialUsed( material->GetGameMaterialId() ) )
    {
        n_assert2_always( "The material can not be removed 'cos it's still in use." );
        return;
    }

    material->SetToBeRemoved( true );
}

#endif

//-----------------------------------------------------------------------------
/**
*/
nGameMaterial* 
nGameMaterialServer::GetMaterialByIndex( const int index ) const
{
    return this->listMaterials.GetElementAt(index);
}

//-----------------------------------------------------------------------------
/**
*/
int
nGameMaterialServer::GetNumMaterials() const
{
    return this->listMaterials.Size();
}

///-----------------------------------------------------------------------------
/**
    Debug function to update the last changes in materials.

    history:
        - 07-Feb-2006   Zombie        created
*/
#ifndef NGAME
void nGameMaterialServer::Update()
{
    nPhyMaterial::UpdateTableMaterialContacts();
}
#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
