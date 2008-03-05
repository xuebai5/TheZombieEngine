#ifndef N_PRELOADMANAGER_H
#define N_PRELOADMANAGER_H

//------------------------------------------------------------------------------
/**
    @class nPreloadManager
    @ingroup Framework

    Preloads some additional user-specified resources for a level

    For each level a user can specify through this manager which resources
    (entity classes and sounds) must be loaded just after having loaded the
    level. For each level a different resources list is specified.

    This class listens to LevelLoaded and LevelSaved signals thrown by the
    level manager. For the LevelLoaded it loads the resources the user desires
    to preload for that level, while for LevelSaved it saves the resources
    preload list.
*/

#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nPreloadManager : public nRoot
{
public:
    /// Default constructor
    nPreloadManager();
    /// Destructor
    ~nPreloadManager();

    /// Return singleton preload manager
    static nPreloadManager* Instance();

    /// Bind to level manager signals
    void Init();
    /// Clear resources lists
    void Clear();

    /// Enable/disable preload of resources
    void SetPreloadEnabled( bool enable );
    /// Tell if preload is enabled
    bool GetPreloadEnabled() const;

    /// Begin the list of classes to be preloaded (clears previous list)
    void BeginClassesToPreload();
    /// Add a class to be preloaded
    void AddClassToPreload( const char* className );
    /// End the list of classes to be preloaded
    void EndClassesToPreload();
    /// Get the number of classes to be preloaded
    int GetNumClassesToPreload() const;
    /// Get a class to be preloaded by index
    const char* GetClassToPreload( int index ) const;
    /// Set a class to be proloaded by index
    void SetClassToPreload( int index, const char* className );

    /// Begin the list of sounds to be preloaded (clears previous list)
    void BeginSoundsToPreload();
    /// Add a sound to be preloaded
    void AddSoundToPreload( const char* soundId );
    /// End the list of sounds to be preloaded
    void EndSoundsToPreload();
    /// Get the number of sounds to be preloaded
    int GetNumSoundsToPreload() const;
    /// Get a sound to be preloaded by index
    const char* GetSoundToPreload( int index ) const;
    /// Set a sound to be proloaded by index
    void SetSoundToPreload( int index, const char* soundId );

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Load the user specified resources for the current level
    void OnLevelLoaded();
    /// Save the user specified resources for the current level
    void OnLevelSaved();

    /// Load all those classes found in the class preload list
    void LoadClasses();
    /// Load all those sounds found in the sound preload list
    void LoadSounds();

    /// Names of the classes to preload
    nArray<nString> classesToPreload;
    /// Ids of the sounds to preload
    nArray<nString> soundsToPreload;

    /// Should or not load resources?
    bool preloadEnabled;

    /// Singleton instance
    static nPreloadManager* instance;
};

//------------------------------------------------------------------------------
/**
    Return singleton preload manager
*/
inline
nPreloadManager*
nPreloadManager::Instance()
{
    n_assert( nPreloadManager::instance );
    return nPreloadManager::instance;
}

//------------------------------------------------------------------------------
#endif // N_PRELOADMANAGER_H
