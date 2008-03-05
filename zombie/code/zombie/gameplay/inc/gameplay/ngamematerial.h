#ifndef N_GAMEMATERIAL_H
#define N_GAMEMATERIAL_H

//-----------------------------------------------------------------------------
/**
    @class nGameMaterial
    @brief Game Material information container.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Game Material
    
    @cppclass nGameMaterial
    
    @superclass nRoot

    @classinfo A game material abstract representation.
*/    

#include "kernel/nroot.h"
#include "util/ntag.h"
#include "util/nmaptabletypes.h"

//-----------------------------------------------------------------------------
class nPhyMaterial;
class nFxMaterial;
//-----------------------------------------------------------------------------

class nGameMaterial : public nRoot
{
public:
    /// constructor
    nGameMaterial();

    /// destructor
    ~nGameMaterial();

    /// object persistency
    bool SaveCmds(nPersistServer *ps);

    /// returns the material id
    int GetMaterialId() const;

    /// creates the material
    void Create();

    /// inits object
    void InitInstance(nObject::InitInstanceMsg initType);

    /// gets the fx material
    nFxMaterial* GetFxMaterialObject();
    /// sets the fx material
    void SetFxMaterialObject( nFxMaterial* fxmaterial);

    /// returns the game material name
    const char* GetGameMaterialName() const;

    /// updates the information (should be called after the material properties had been changed)
    void Update();

    /// returns the game material id
    const unsigned int GetGameMaterialId() const;

    /// sets physics material
    void SetPhysicsMaterial(nPhyMaterial*);

    /// gets the physics material
    nPhyMaterial* GetPhysicsMaterial() const;

    /// sets fx material
    void SetFxMaterial(const char*);

    /// gets fx material
    const char* GetFxMaterial() const;

    /// add a sound event
    void  AddSoundEvent(const char *, const char *);

    /// get a sound event sound id
    const char*  GetSoundEvent(const char *);

    /// temporal fixed methods for the property editor: hit and slide sound events
    /// add hit sound event
    void  SetHitSoundEvent(const char *);

    /// add slide sound event
    void  SetSlideSoundEvent(const char *);

    /// get hit sound event id
    const char*  GetHitSoundEvent ();

    /// get slide sound event id
    const char*  GetSlideSoundEvent ();

    /// adds himself to the server
    void Add();

    /// set if the material is a hard material
    void SetHard(bool);
    /// get if the material is a hard material
    bool GetHard() const;

#ifndef NGAME
    /// returns if any of the materials components is dirty (changed)
    const bool IsDirty() const;

    /// return if the object it's meant to be removed
    bool GetToBeRemoved() const;

    /// set to be removed
    void SetToBeRemoved( bool has );

#endif

private:
#ifndef NGAME
    /// stores if the material has to removed
    bool hasToBeRemoved;
#endif

    /// materials id
    nTag id;

    /// stores the physics materials
    nPhyMaterial* physicsMaterial;

    nDynAutoRef<nFxMaterial> fxMaterial;

    // Map table for sound events
    nMapTableTypes<nString>::NString soundEventsMap;

    bool isHard;
};

#endif