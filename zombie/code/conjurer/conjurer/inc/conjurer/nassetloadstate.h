#ifndef N_ASSETLOADSTATE_H
#define N_ASSETLOADSTATE_H
//------------------------------------------------------------------------------
/**
    @class nAssetLoadState
    @ingroup NebulaConjurerEditor

    Editor state for loading a scene into conjurer.

    (C) 2004 Conjurer Services, S.A.
*/
#include "napplication/nappstate.h"
#include "kernel/nautoref.h"
#include "kernel/ncmdprotonativecpp.h"

class nSceneNode;
class nTransformNode;
class nEntityObject;
class nConjurerLayout;

//------------------------------------------------------------------------------
class nAssetLoadState : public nAppState
{
public:
    /// constructor
    nAssetLoadState();
    /// destructor
    virtual ~nAssetLoadState();
    /// called when state is created
    virtual void OnCreate(nApplication* app);
    /// called on state to perform state logic 
    virtual void OnFrame();

    /// set the next state to run
    void SetNextState(const nString& state);

    /// @name Script interface
    //@{

        /// set scene resource file (scene only)
        void SetSceneFile(const char *);
        /// get scene resource file (scene only)
        const char * GetSceneFile() const;
        /// set class name to instance
        void SetLoadClassName(const char *);
        /// get class name to instance
        const char * GetLoadClassName() const;
        /// set animation state index
        void SetAnimState(int);
        /// get animation state index
        int GetAnimState() const;

    //@}

private:
    nString className;
    nString resourceFile;
    nString loadNextState;
    int animStateIndex;
    nRef<nEntityObject> refEntity;
    nRef<nConjurerLayout> refLayout;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nAssetLoadState::SetSceneFile(const char* filename)
{
    this->resourceFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nAssetLoadState::GetSceneFile() const
{
    return this->resourceFile.IsEmpty() ? 0 : this->resourceFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAssetLoadState::SetLoadClassName(const char* className)
{
    this->className = className;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nAssetLoadState::GetLoadClassName() const
{
    return this->className.IsEmpty() ? 0 : this->className.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAssetLoadState::SetAnimState(int animStateIndex)
{
    this->animStateIndex = animStateIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAssetLoadState::GetAnimState() const
{
    return this->animStateIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nAssetLoadState::SetNextState( const nString & state )
{
    this->loadNextState = state; 
}

//------------------------------------------------------------------------------
#endif
