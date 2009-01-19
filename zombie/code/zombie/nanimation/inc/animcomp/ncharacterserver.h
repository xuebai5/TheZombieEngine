#ifndef N_CHARACTERSERVER_H
#define N_CHARACTERSERVER_H
//------------------------------------------------------------------------------
/**
    @class nCharacterServer
    @ingroup Anim2

    @brief The animation server offers central services of the animation
    subsystem (at the moment it is merely a factory for nAnimation objects).
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nautoref.h"

class nResourceServer;
class nAnimation;
class nCharacter2;
class ncCharacter;

//------------------------------------------------------------------------------
class nCharacterServer : public nRoot
{
public:
    typedef nArray<nEntityObject*> CharacterEntityPool;

    /// constructor
    nCharacterServer();
    /// destructor
    virtual ~nCharacterServer();
    /// return instance pointer
    static nCharacterServer* Instance();
    
    /// Update joints and skeletons, delete unused skeletons
    void Trigger(nTime time, float frames);
    /// register character entity
    bool Register(nEntityObject*);
    /// unregister character entity
    void Unregister(nEntityObject*);
    /// callback function when entity is removed
    void EntityDeleted(int id);

    /// test if skeletons must be updated from trigger
    bool DoTrigger(float frames);
    /// test if skeltons must be updated outside animationserver
    bool DoUpdate();
    /// get accumulated time since last update
    float GetAccumFrameTime() const;

    /// set fixed fps
    void SetFixedFPS(int fps);
    /// get fixed fps
    int GetFixedFPS() const;
    /// set fixed physics fps
    void SetFixedPhysicsFPS(int fps);
    /// get fixed physics fps
    int GetFixedPhysicsFPS() const;

    #ifndef NGAME
    /// enable/disable collision animations
    void SetPhysicsEnabled(bool enabled);
    /// set if collision animation is enabled
    bool GetPhysicsEnabled() const;
    #endif NGAME

private:
    static nCharacterServer* Singleton;

    nAutoRef<nResourceServer> refResourceServer;

    /// stores all character entities in the world
    CharacterEntityPool characterEntityPool;

    int fixedFPS;
    int fixedPhysicsFPS;
    float interFrameTime;
    float interPhysicsFrameTime;
    float accumTime;

    #ifndef NGAME
    bool physicsEnabled;
    #endif

    #ifdef __NEBULA_STATS__
    friend class ncCharacter;
    friend class nCharacter2;
    friend class nAnimState;
    nProfiler profAnim;
    nProfiler profAnimCollision;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharacterServer*
nCharacterServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterServer::SetFixedFPS(int fps)
{
    this->fixedFPS = fps;
    this->interFrameTime = 1.0f / this->fixedFPS;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacterServer::GetFixedFPS() const
{
    return this->fixedFPS;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterServer::SetFixedPhysicsFPS(int fps)
{
    this->fixedPhysicsFPS = fps;
    this->interPhysicsFrameTime = 1.0f / this->fixedPhysicsFPS;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacterServer::GetFixedPhysicsFPS() const
{
    return this->fixedPhysicsFPS;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacterServer::SetPhysicsEnabled(bool enabled)
{
    this->physicsEnabled = enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacterServer::GetPhysicsEnabled() const
{
    return this->physicsEnabled;
}
#endif

//------------------------------------------------------------------------------
#endif
