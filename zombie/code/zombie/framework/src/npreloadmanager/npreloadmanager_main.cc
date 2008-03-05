//-----------------------------------------------------------------------------
//  npreloadmanager_main.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchncommonapp.h"
#include "npreloadmanager/npreloadmanager.h"
#include "nlevel/nlevelmanager.h"
#include "entity/nentityclassserver.h"
#include "zombieentity/nloaderserver.h"
#include "nsoundlibrary/nsoundlibrary.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "kernel/nfileserver2.h"

//-----------------------------------------------------------------------------
nNebulaScriptClass(nPreloadManager, "nroot");

//------------------------------------------------------------------------------
nPreloadManager* nPreloadManager::instance = 0;

//-----------------------------------------------------------------------------
namespace
{
    const char* PreloadListFilePath( "level:preload.n2" );
}

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nPreloadManager::nPreloadManager() :
    preloadEnabled( true )
{
    if ( !nPreloadManager::instance )
    {
        // Initialize instance pointer
        nPreloadManager::instance = this;
    }
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nPreloadManager::~nPreloadManager()
{
    if ( nPreloadManager::instance == this )
    {
        nPreloadManager::instance = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Bind to level manager signals

    Call this method after the level manager has been instantiated
*/
void
nPreloadManager::Init()
{
    // Binds to the load and saved level signals to load and save the preload list
    nLevelManager::Instance()->BindSignal( nLevelManager::SignalLevelLoaded,
        this, &nPreloadManager::OnLevelLoaded, 0 );
    nLevelManager::Instance()->BindSignal( nLevelManager::SignalLevelSaved,
        this, &nPreloadManager::OnLevelSaved, 0 );
}

//-----------------------------------------------------------------------------
/**
    Clear resources lists
*/
void
nPreloadManager::Clear()
{
    this->classesToPreload.Clear();
    this->soundsToPreload.Clear();
}

//-----------------------------------------------------------------------------
/**
    Enable/disable preload of resources
*/
void
nPreloadManager::SetPreloadEnabled( bool enable )
{
    this->preloadEnabled = enable;
}

//-----------------------------------------------------------------------------
/**
    Tell if preload is enabled
*/
bool
nPreloadManager::GetPreloadEnabled() const
{
    return this->preloadEnabled;
}

//-----------------------------------------------------------------------------
/**
    Begin the list of classes to be preloaded (clears previous list)
*/
void
nPreloadManager::BeginClassesToPreload()
{
    this->classesToPreload.Clear();
}

//-----------------------------------------------------------------------------
/**
    Add a class to be preloaded
*/
void
nPreloadManager::AddClassToPreload( const char* className )
{
    this->classesToPreload.Append( className );
}

//-----------------------------------------------------------------------------
/**
    End the list of classes to be preloaded
*/
void
nPreloadManager::EndClassesToPreload()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Get the number of classes to be preloaded
*/
int
nPreloadManager::GetNumClassesToPreload() const
{
    return this->classesToPreload.Size();
}

//-----------------------------------------------------------------------------
/**
    Get a class to be preloaded by index
*/
const char*
nPreloadManager::GetClassToPreload( int index ) const
{
    if ( index >= 0 && index < this->classesToPreload.Size() )
    {
        return this->classesToPreload[index].Get();
    }
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Set a class to be preloaded by index
*/
void
nPreloadManager::SetClassToPreload( int index, const char* className )
{
    if ( index >= 0 && index < this->classesToPreload.Size() )
    {
        this->classesToPreload[index] = className;
    }
}

//-----------------------------------------------------------------------------
/**
    Begin the list of sounds to be preloaded (clears previous list)
*/
void
nPreloadManager::BeginSoundsToPreload()
{
    this->soundsToPreload.Clear();
}

//-----------------------------------------------------------------------------
/**
    Add a sound to be preloaded
*/
void
nPreloadManager::AddSoundToPreload( const char* soundId )
{
    this->soundsToPreload.Append( soundId );
}

//-----------------------------------------------------------------------------
/**
    End the list of sounds to be preloaded
*/
void
nPreloadManager::EndSoundsToPreload()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Get the number of sounds to be preloaded
*/
int
nPreloadManager::GetNumSoundsToPreload() const
{
    return this->soundsToPreload.Size();
}

//-----------------------------------------------------------------------------
/**
    Get a sound to be preloaded by index
*/
const char*
nPreloadManager::GetSoundToPreload( int index ) const
{
    if ( index >= 0 && index < this->soundsToPreload.Size() )
    {
        return this->soundsToPreload[index].Get();
    }
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Set a sound to be preloaded by index
*/
void
nPreloadManager::SetSoundToPreload( int index, const char* soundId )
{
    if ( index >= 0 && index < this->soundsToPreload.Size() )
    {
        this->soundsToPreload[index] = soundId;
    }
}

//-----------------------------------------------------------------------------
/**
    Load the user specified resources for the current level
*/
void
nPreloadManager::OnLevelLoaded()
{
    // Must clear resources lists since the preload file may not contain
    // new resource types (the file won't contain a Begin/End for new types).
    // Also old levels doesn't contain a preload file at all.
    // If clear is not done in these cases, resources of the previous level
    // will be loaded for this level as well.
    this->Clear();

    // Load user specified resources for current level
    if ( nFileServer2::Instance()->FileExists( PreloadListFilePath ) )
    {
        this->LoadState( PreloadListFilePath );

        if ( this->preloadEnabled )
        {
            this->LoadClasses();
            this->LoadSounds();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Save the user specified resources for the current level
*/
void
nPreloadManager::OnLevelSaved()
{
    nString dirPath( nString(PreloadListFilePath).ExtractDirName() );
    nFileServer2::Instance()->MakePath( dirPath.Get() );
    this->SaveStateAs( PreloadListFilePath );
}

//-----------------------------------------------------------------------------
/**
    Load all those classes found in the class preload list
*/
void
nPreloadManager::LoadClasses()
{
    for ( int i(0); i < this->classesToPreload.Size(); ++i )
    {
        const char* className = this->classesToPreload[i].Get();
        nEntityClass* entityClass = nEntityClassServer::Instance()->GetEntityClass( className );
        nLoaderServer::Instance()->LoadClassResources( entityClass );
        nLoaderServer::Instance()->RetainClassResources( entityClass );
    }
}

//-----------------------------------------------------------------------------
/**
    Load all those sounds found in the sound preload list
*/
void
nPreloadManager::LoadSounds()
{
    for ( int i(0); i < this->soundsToPreload.Size(); ++i )
    {
        const char* soundId = this->soundsToPreload[i].Get();
        int soundIndex = nSoundLibrary::Instance()->GetSoundIndex( soundId );
        if ( soundIndex != -1 && !nSoundScheduler::Instance()->IsLoadedSound( soundIndex ) )
        {
            nSoundScheduler::Instance()->LoadSound( soundIndex );
        }
    }
}
