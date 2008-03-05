#ifndef N_D3D9STATEMANAGER_H
#define N_D3D9STATEMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nD3D9StateManager
    @ingroup Gfx2

    @brief A custom state manager for D3DX effects to handle redundant states
    set in effect techniques and samplers.
    The code is a port of the StateManager sample in the DirectX SDK.

    (c) 2006 Conjurer Services, S.A.
*/
#include "gfx2/nshader2.h"

#include <d3dx9.h>

//------------------------------------------------------------------------------
class nD3D9EffectStateManager : public ID3DXEffectStateManager
{
public:
    /// constructor
    nD3D9EffectStateManager();
    /// destructor
    ~nD3D9EffectStateManager();
    /// set all cached values to dirty
    virtual void DirtyCachedValues() = 0;
    
    virtual LPCWSTR EndFrameStats() = 0;

    static nD3D9EffectStateManager* 
        nD3D9EffectStateManager::Create( LPDIRECT3DDEVICE9 pDevice );

};

inline
nD3D9EffectStateManager::nD3D9EffectStateManager()
{
    //
}

inline
nD3D9EffectStateManager::~nD3D9EffectStateManager()
{
    //
}

#endif /*N_D3D9STATEMANAGER_H*/
