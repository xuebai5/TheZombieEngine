#ifndef NFXMATERIAL_H
#define NFXMATERIAL_H
//------------------------------------------------------------------------------
/**
    @class nFxMaterial
    @ingroup SpecialFX
    @author 

    @brief ...

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "nspecialfx/ncspecialfxclass.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------
class nFxMaterial: public nRoot
{
public:
    class nFxNode{
    public:
        nString id; //name of group effect
        nArray<nString> fxlist;
        nArray<int> keys; //used only in gameplay
        ncSpecialFXClass::FxType GetType(int idx);
    private:
        friend class nFxMaterial;
        ncSpecialFXClass::FxType type;
    };

    struct nFxEffects{
        nString eventType;
        nArray<int> effectsIndexList; // List of effects, position in nFxMaterial::specialfx
        #ifndef __NEBULA_NO_ASSERT__
        nArray<nString> effectsNameList; // The name for debug
        #endif
    };

    //constructor
    nFxMaterial();
    //Destructor
    ~nFxMaterial();

    /// get fx node for an specific type
    nFxNode* GetFxNodeFor(nFxEffects* effect, int idx ) const;
    /// get number of fxnodes
    int GetNumFxNodes() const;
    /// get fx node by index
    nFxNode* GetFxNodeByIndex( int index ) const ;
    /// get fx effect for an specific event
    nFxEffects* GetFxEffectFor(const char* event ) const;

#ifndef NGAME
    /// get to be removed
    bool GetToBeRemoved() const;
    /// set to be removed
    void SetToBeRemoved(bool has);

    /// returns if the material is dirty
    bool IsDirty();

    //save cmds
    bool SaveMaterial(nPersistServer* server);
#endif

    /// begin list
    void BeginFx(const char *, const char *, int);
    /// add element to list
    bool AddFxClass(int, const char *);
    /// end list
    void EndFx();

    /// begin list
    void BeginEvent(const char *, int);
    /// add element to list
    bool AddEffect(int, const char *);
    /// end list
    void EndEvent();

private:
#ifndef NGAME
    bool hasToBeRemoved;
    bool isDirty;
#endif
    /// Find the first element has the same id, slowest
    int FindSpecialFxIndex(const char* id) const;

    // special fx array (for each fx there's some elements to randomize)
    nArray< nFxNode > specialfx;
    nMapTableTypes<nFxEffects>::NString matEffects;
    // pointer to last added fxeffects
    nFxEffects* lastFxEffects;
};

//------------------------------------------------------------------------------
/**
*/

#endif /*NFXMATERIAL_H*/
