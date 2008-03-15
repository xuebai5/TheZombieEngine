#ifndef NRNSENTITYMANAGER_H
#define NRNSENTITYMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nRnsEntityManager

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "kernel/nprofiler.h"
#include "mathlib/polar.h"
#include "mathlib/polar.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nentitytypes.h"

//------------------------------------------------------------------------------
class nEntityObject;
class nstream;
class nDamageManager;
class nInventoryManager;
class nPathFinder;
class ncGameplayLiving;

//------------------------------------------------------------------------------
class nRnsEntityManager : public nRoot
{
public:
    enum RpcFunctions{
        SPAWN_PLAYER,
        SET_LOCAL_PLAYER,
        KILL_ENTITY,
        DELETE_ENTITY,
        UPDATE_INVENTORY,
        INVENTORY_ITEM,
        SET_OBJECTIVE_STATE,
        WEAPON_TRIGGER,
        WEAPON_BURST,
        SHOW_GAME_MESSAGE,
        SUICIDE_PLAYER,
        //Music
        STOP_MUSIC,
        PLAY_MUSIC_PART,
        PLAY_MUSIC_STINGER,
        SET_MUSIC_MOOD,
        SET_MUSIC_STYLE,
        // Sound
        PLAY_SOUND_EVENT,
        STOP_SOUND,
    };

    enum WeaponIds{
        WEAPON_M4 = 0,
        WEAPON_TABLE_SIZE,
    };

    enum PlayerDataFlags{
        PDF_POSITION = (1<<0),
        PDF_ANGLES = (1<<1),
        PDF_DEAD = (1<<2)
    };

    struct PlayerData 
    {
        int flags;
        char weapon;
        vector3 position;
        polar2 angles;

        void UpdateStream( nstream & data );
    };

    /// constructor
    nRnsEntityManager();

    /// destructor
    ~nRnsEntityManager();

    /// singleton instance
    static nRnsEntityManager * Instance();

    /// create a player for a connected client
    nEntityObject * CreatePlayer( const PlayerData & data );

    /// create a local entity of desiree class 
    nEntityObject * CreateLocalEntity( const char * classname );

    /// create a server entity of desiree class
    nEntityObject * CreateServerEntity( const char * classname );

    /// set the local player
    void SetLocalPlayer( int id );

    /// get the local player
    nEntityObject * GetLocalPlayer () const;

    /// warning about a entity was dead
    void EntityDead( ncGameplayLiving * entity );

    /// kill a game entity
    void KillEntity( int id );

    /// suicide a player
    void SuicidePlayer( nEntityObject * entity );

    /// delete a game entity
    void DeleteEntity( int id );

    /// an entity has been deleted from the server
    void EntityDeleted( int id );

    /// init level dependend systems and entities
    void Start();

    /// restore and shut down level depended systems and entities
    void Stop();

    // per-frame function
    void Trigger();

    /// @name Signals interface
    //@{
    //NSIGNAL_DECLARE('LEDD', void, EntityDead, 1, (nEntityObject *), 0, ());
    NSIGNAL_DECLARE('LEDE', void, EntityEliminated, 1, (nEntityObject *), 0, ());
    NSIGNAL_DECLARE('JMAC', void, MountingAccessory, 3, (int, const char *, float), 0, () );
    NSIGNAL_DECLARE('LCAC', void, CancelAccessory, 0, (), 0, () );
    NSIGNAL_DECLARE('LWCH', void, WeaponChanged, 2, (nEntityObject *, nEntityObject *), 0, () );
    //@}

    /// function called when a new entity is created with the network layer
    void NewNetworkEntity( int id );

    /// callback for a new client conected from network
    void NewNetworkClient( int client );

    /// callback to handle new entities created through the world interface
    void OnNewEntity( nEntityObject* entity );

    /// activate an object from another entity
    void ActivateObject( nEntityObject * user );

private:
    enum UsableObjectType
    {
        UOT_NOTHING = 0,
        UOT_DROPPING,
        UOT_USABLE,
        UOT_VEHICLE,

        UOT_LAST,
    };

    struct DeadEntity
    {
        // dead entity
        nEntityObject *entity;
        // time for the dead animation
        float time;
        // time for ragdoll state
        float timeRagdoll;
    };

    /// start up any system that requires the level to have been loaded first
    void StartUpLevelDependendSystems();
    /// shut down any system that depends on the current level
    void ShutDownLevelDependendSystems();
    /// register all normal (persisted) entities and set their initial behavior state
    void InitNormalEntities();
    /// remove all entities, destroying the dynamic ones (local or server entity types)
    void CleanEntities();
    /// respond to a signal that an entity has been removed
    void EntityDeleted( nEntityObjectId entityId, bool removeFromWorld );
    /// remove entity from game
    void RemoveEntity( nEntityObject * entity, bool removeFromWorld );
    /// init the player with the send data 
    void InitPlayer( nEntityObject * player, const PlayerData & data );
    /// init a entity object
    void InitEntityObject( nEntityObject * object );
    /// get the weapon name for a weapon id
    const char * GetWeaponName( const unsigned char weapon );
    /// get the nearest object for an entity that can use
    nObject* GetNearestObject( nEntityObject * player );
    /// get the nearest object for an entity that can use
    UsableObjectType GetUsableType( nObject * object );
    /// update the gameplay entities
    void UpdateGameplayEntities( const float deltaTime );
    /// leave the ragdoll a time in the ground before restore it
    void UpdateDeadEntities( float deltaTime );
    /// bury entities gradually into the ground and finally delete it
    void BuryDeadEntities( float deltaTime );
    /// remove entities from game
    void RemoveInvalidEntities();

    static nRnsEntityManager * singleton;

    static const char * weaponTable[WEAPON_TABLE_SIZE];
    static const char * playerName;

    nArray< nEntityObject * > gameplayEntities;
    nArray< nEntityObject * > invalidEntities;
    nArray< nEntityObjectId > removedEntities;
    nArray< DeadEntity * > deadGameplayEntities;
    nArray< DeadEntity * > buriedGameplayEntities;
    nEntityObject * localPlayer;
    nDamageManager * damageManager;
    nInventoryManager * inventoryManager;
    nPathFinder * pathFinder;

    nTime lastTime;

    bool hasStarted;

#ifdef __NEBULA_STATS__
    nProfiler profGPtrigger;
    nProfiler profGPupdate;
#endif
};

//------------------------------------------------------------------------------
/**
    @returns the local player
*/
inline
nEntityObject *
nRnsEntityManager::GetLocalPlayer() const
{
    return this->localPlayer;
}

//------------------------------------------------------------------------------
#endif//NRNSENTITYMANAGER_H
