#ifndef NFXEVENTTRIGGER_H
#define NFXEVENTTRIGGER_H
//------------------------------------------------------------------------------
/**
    @class nFxEventTrigger

    @brief Module that chooses which special fx has to be used
*/
//------------------------------------------------------------------------------
#include "gameplay/nfxeventtriggertypes.h"

//------------------------------------------------------------------------------
class nFXObject;
class nGameMaterial;
class nFxMaterial;

//------------------------------------------------------------------------------
class nFxEventTrigger : public nObject
{
public:
    enum nFxSubType
    {
        MELEE_LEFT,
        MELEE_CENTER,
        MELEE_RIGHT,
        SHOT_FRONT,
        SHOT_BACK,
        SHOT_LEFT_FRONT,
        SHOT_RIGHT_FRONT,
        SHOT_LEFT_BACK,
        SHOT_RIGHT_BACK,
        SHOT_LEFT,
        SHOT_RIGHT,
        EXPLOSION,
        M4_1P_FIRE,
        M4_3P_FIRE,
        SCOPE,

        NUMSUBTYPES,
        INVALIDTYPE,
    };

    struct nFxClasses{
        nString fxclass;
        int key; //used only in gameplay
    };

    struct nFxSpecial{
        //nString specificName;
        nFxSubType fxsubtype;
        nArray<nFxClasses> fxlist;
    };

    /// constructor
    nFxEventTrigger();
    /// destructor
    ~nFxEventTrigger();

    /// return singleton instance
    static nFxEventTrigger * Instance();

    //save special fx that don't need material dependencies
    bool SaveSpecialFx(nPersistServer* ps);

    /// launch impact, explosions ...
    bool Launch(const nLaunchSpecialFxData& fx);
    /// launch explosion
    bool LaunchExplosion(const vector3&);
    ///  weapon shoot
    bool Shoot( nEntityObject* shooter, nEntityObject* weapon);

    /// Load all specialfx
    bool LoadSFx();

        /// begin list
        void BeginGroup( nString&, int);
        /// add element to list
        bool AddFxSpecialClass(nString&, nString&);
        /// add element to list
        bool AddFxClassToGroup(int, nString&);
        /// end list
        void EndGroup();
        /// splash
        void Splash(nString);

    /// convert type string to enum
    static nFxSubType StringToType(const char* str);
    /// convert type enum to string
    static const char* TypeToString(nFxSubType t);

    /// find shot orientation
    void MeleeSplash( const vector3& impactPos, nEntityObject* eObj);

    /// scope overlay
    nFXObject * Scope();

private:
    //load material non dependent fx
    bool LoadChunk();
    // load the specific material
    bool LoadSFx(nFxMaterial* fxMaterial);
    /// get special fx for a given type
    nFxSpecial* GetFxFor( nFxSubType type );
    /// return igf the entity can receive a decal
    static bool CanReceiveDecal(nEntityObject* object);

    static nFxEventTrigger * singleton;

    nArray<nFxSpecial> fxMatIndependent;

};

//------------------------------------------------------------------------------
#endif//NFXEVENTTRIGGER_H
