#include "precompiled/pchndirect3d9.h"
//--------------------------------------------------------------------------------------
//  nd3d9effectstatemanager.cc
//  (c) 2005 Conjurer Services, S.A.
//--------------------------------------------------------------------------------------
#include "gfx2/nd3d9effectstatemanager.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9types.h>

typedef unsigned long DWORD;

const int LOG_SHADER = NLOG2;

//--------------------------------------------------------------------------------------
// Base implementation of a custom ID3DXEffectStateManager interface
// This implementation does nothing more than forward all state change commands to the
// appropriate D3D handler.
// An interface that implements custom state change handling may be derived from this
// (such as statistical collection, or filtering of redundant state change commands for
// a subset of states)
//--------------------------------------------------------------------------------------
class nD3D9BaseStateManager : public nD3D9EffectStateManager
{
protected:
    LPDIRECT3DDEVICE9 m_pDevice;
    LONG              m_lRef;
    UINT              m_nTotalStateChanges;
    UINT              m_nTotalStateChangesPerFrame;
    WCHAR             m_wszFrameStats[256];
public:
    nD3D9BaseStateManager( LPDIRECT3DDEVICE9 pDevice )
        : m_lRef( 1UL ),
          m_pDevice( pDevice ),
          m_nTotalStateChanges( 0 ),
          m_nTotalStateChangesPerFrame( 0 )
    {
        // Increment the reference count on the device, because a pointer to it has
        // been copied for later use
        m_pDevice->AddRef();
        m_wszFrameStats[0] = 0;
    }
    
    virtual ~nD3D9BaseStateManager()
    {
        // Release the reference count held from the constructor
        m_pDevice->Release();
    }

    // Must be invoked by the application anytime it allows state to be
    // changed outside of the D3DX Effect system.
    // An entry-point for this should be provided if implementing custom filtering of redundant
    // state changes.
    virtual void DirtyCachedValues()
    {
    }

    virtual LPCWSTR EndFrameStats()
    {
        if( m_nTotalStateChangesPerFrame != m_nTotalStateChanges )
        {
            m_wszFrameStats[255] = 0;
            m_nTotalStateChangesPerFrame = m_nTotalStateChanges;
        }

        m_nTotalStateChanges = 0;

        return m_wszFrameStats;
    }

    // methods inherited from ID3DXEffectStateManager
    STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv)
    {
        if (iid == IID_IUnknown || iid == IID_ID3DXEffectStateManager)
        {
            *ppv = static_cast<ID3DXEffectStateManager*>(this);
        } 
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        
        reinterpret_cast<IUnknown*>(this)->AddRef();
        return S_OK;
    }
    STDMETHOD_(ULONG, AddRef)(THIS)
    {
        return (ULONG)InterlockedIncrement( &m_lRef );
    }
    STDMETHOD_(ULONG, Release)(THIS)
    {
        if( 0L == InterlockedDecrement( &m_lRef ) )
        {
            delete this;
            return 0L;
        }

        return m_lRef;
    }
    STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE d3dRenderState, DWORD dwValue )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetRenderState( d3dRenderState, dwValue );
    }
    STDMETHOD(SetSamplerState)(THIS_ DWORD dwStage, D3DSAMPLERSTATETYPE d3dSamplerState, DWORD dwValue )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetSamplerState( dwStage, d3dSamplerState, dwValue );
    }
    STDMETHOD(SetTextureStageState)(THIS_ DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTextureStageState, DWORD dwValue )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetTextureStageState( dwStage, d3dTextureStageState, dwValue );
    }
    STDMETHOD(SetTexture)(THIS_ DWORD dwStage, LPDIRECT3DBASETEXTURE9 pTexture )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetTexture( dwStage, pTexture );
    }
    STDMETHOD(SetVertexShader)(THIS_ LPDIRECT3DVERTEXSHADER9 pShader )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetVertexShader( pShader );
    }
    STDMETHOD(SetPixelShader)(THIS_ LPDIRECT3DPIXELSHADER9 pShader )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetPixelShader( pShader );
    }
    STDMETHOD(SetFVF)(THIS_ DWORD dwFVF )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetFVF( dwFVF );
    }
    STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetTransform( State, pMatrix );
    }
    STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9 *pMaterial )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetMaterial( pMaterial );
    }
    STDMETHOD(SetLight)(THIS_ DWORD Index, CONST D3DLIGHT9 *pLight )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetLight( Index, pLight );
    }
    STDMETHOD(LightEnable)(THIS_ DWORD Index, BOOL Enable )
    {
        m_nTotalStateChanges++;
        return m_pDevice->LightEnable( Index, Enable );
    }
    STDMETHOD(SetNPatchMode)(THIS_ FLOAT NumSegments )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetNPatchMode( NumSegments );
    }
    STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT RegisterIndex,
                                        CONST FLOAT *pConstantData,
                                        UINT RegisterCount )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetVertexShaderConstantF( RegisterIndex,
                                                    pConstantData,
                                                    RegisterCount );
    }
    STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT RegisterIndex,
                                        CONST INT *pConstantData,
                                        UINT RegisterCount )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetVertexShaderConstantI( RegisterIndex,
                                                    pConstantData,
                                                    RegisterCount );
    }
    STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT RegisterIndex,
                                        CONST BOOL *pConstantData,
                                        UINT RegisterCount )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetVertexShaderConstantB( RegisterIndex,
                                                    pConstantData,
                                                    RegisterCount );
    }
    STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT RegisterIndex,
                                       CONST FLOAT *pConstantData,
                                       UINT RegisterCount )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetPixelShaderConstantF( RegisterIndex,
                                                   pConstantData,
                                                   RegisterCount );
    }
    STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT RegisterIndex,
                                        CONST INT *pConstantData,
                                        UINT RegisterCount )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetPixelShaderConstantI( RegisterIndex,
                                                   pConstantData,
                                                   RegisterCount );
    }
    STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT RegisterIndex,
                                       CONST BOOL *pConstantData,
                                       UINT RegisterCount )
    {
        m_nTotalStateChanges++;
        return m_pDevice->SetPixelShaderConstantB( RegisterIndex,
                                                   pConstantData,
                                                   RegisterCount );
    }
};

//--------------------------------------------------------------------------------------
// Templated class for caching and duplicate state change filtering of paired types
// (such as D3DRENDERSTATETYPE/DWORD value types)
//--------------------------------------------------------------------------------------
template < typename _Ty >
class multicache
{
protected:
    nKeyArray< _Ty > cache;         // A map provides a fast look-up of contained states
                                    // and furthermore ensures that duplicate key values
                                    // are not present.
                                    // Additionally, dirtying the cache can be done by
                                    // clear()ing the container.
public:
    // constructor
    multicache< typename _Ty >() :
        cache(16, 16)
    {
    }

    // Command to dirty all cached values
    inline void dirtyall( )
    {
        //cache.clear();
        cache.Clear();
    }

    // Command to dirty one key value
    inline void dirty( int key )
    {
        if (cache.Find(key))
        {
            cache.Rem(key);
        }
    }

    // Called to update the cache
    // The return value indicates whether or not the update was a redundant change.
    // A value of 'true' indicates the new state was unique, and must be submitted
    // to the D3D Runtime.
    inline bool set_val( /*_Kty*/ int key, _Ty value )
    {
        if (!cache.HasKey(key))
        {
            cache.Add(key, value);
            return true;
        }

        if (cache.GetElement(key) == value)
        {
            return false;
        }

        cache.GetElement(key) = value;

        return true;
    }
};

//--------------------------------------------------------------------------------------
// Implementation of a state manager that filters redundant state change commands.
// This implementation is useful on PURE devices.
// PURE HWVP devices do not implement redundant state change filtering.
// States that may be useful to filter on PURE device are:
//      Render States
//      Texture Stage States
//      Sampler States
// See the Direct3D SDK Documentation for further details on pure device state change
// behavior.
//--------------------------------------------------------------------------------------
#define CACHED_STAGES 4        // The number of stages to cache
                               // Remaining stages are simply passed through with no
                               // redundancy filtering.
                               // For this sample, the first two stages are cached, while
                               // the remainder are passed through

class nD3D9PureDeviceStateManager : public nD3D9BaseStateManager
{
protected:
    typedef multicache< DWORD > samplerStageCache;
    typedef multicache< DWORD > textureStateStageCache;

    multicache< DWORD> cacheRenderStates;    // cached Render-States
    nArray<samplerStageCache>      vecCacheSamplerStates;
    nArray<textureStateStageCache> vecCacheTextureStates;

    UINT    m_nFilteredStateChanges;                            // Statistics -- # of redundant
                                                                // states actually filtered
    UINT    m_nFilteredStateChangesPerFrame;

    #ifdef __NEBULA_STATS__
    nWatched watchTotalStateChanges;
    nWatched watchTotalStateChangesPerFrame;
    nWatched watchFilteredStateChanges;
    nWatched watchFilteredStateChangesPerFrame;
    #endif

public:
    nD3D9PureDeviceStateManager( LPDIRECT3DDEVICE9 pDevice )
        : nD3D9BaseStateManager( pDevice ),
          cacheRenderStates(), 
          m_nFilteredStateChanges( 0 ),
          m_nFilteredStateChangesPerFrame( 0 )
          #ifdef __NEBULA_STATS__
         ,watchTotalStateChanges("gfxTotalStateChanges", nArg::Int),
          watchTotalStateChangesPerFrame("gfxTotalStateChangesPerFrame", nArg::Int),
          watchFilteredStateChanges("gfxNumFilteredStateChanges", nArg::Int),
          watchFilteredStateChangesPerFrame("gfxNumFilteredStateChangesPerFrame", nArg::Int)
          #endif
    {
        vecCacheSamplerStates.SetFixedSize( CACHED_STAGES );
        vecCacheTextureStates.SetFixedSize( CACHED_STAGES );
    }
    virtual LPCWSTR EndFrameStats()
    {
        // If either the 'total' state changes or the 'filtered' state changes
        // has changed, re-compute the frame statistics string
        if( 0 != ( (m_nTotalStateChangesPerFrame  - m_nTotalStateChanges) | 
                   (m_nFilteredStateChangesPerFrame - m_nFilteredStateChanges) ) )
        {
            m_wszFrameStats[255] = 0;

            m_nTotalStateChangesPerFrame = m_nTotalStateChanges;
            m_nFilteredStateChangesPerFrame = m_nFilteredStateChanges;
        }

        #ifdef __NEBULA_STATS__
        watchTotalStateChanges->SetI(m_nTotalStateChanges);
        watchFilteredStateChanges->SetI(m_nFilteredStateChanges);
        watchTotalStateChangesPerFrame->SetI(m_nTotalStateChangesPerFrame);
        watchFilteredStateChangesPerFrame->SetI(m_nFilteredStateChangesPerFrame);
        #endif

        m_nTotalStateChanges = 0;
        m_nFilteredStateChanges = 0;

        return m_wszFrameStats;
    }
    // More targeted 'Dirty' commands may be useful.
    void DirtyCachedValues()
    {
        cacheRenderStates.dirtyall();

        nArray<samplerStageCache>::iterator it_samplerStages;
        for( it_samplerStages =  vecCacheSamplerStates.Begin();
             it_samplerStages != vecCacheSamplerStates.End();
             it_samplerStages++ )
        {
            (*it_samplerStages).dirtyall();
        }

        nArray<textureStateStageCache>::iterator it_textureStages;
        for( it_textureStages =  vecCacheTextureStates.Begin();
             it_textureStages != vecCacheTextureStates.End();
             it_textureStages++ )
        {
            (*it_textureStages).dirtyall();
        }
    }

    // methods inherited from ID3DXEffectStateManager
    STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE d3dRenderState, DWORD dwValue )
    {
        m_nTotalStateChanges++;

        // Update the render state cache
        // If the return value is 'true', the command must be forwarded to
        // the D3D Runtime.
        if( cacheRenderStates.set_val( d3dRenderState, dwValue ) )
            return m_pDevice->SetRenderState( d3dRenderState, dwValue );

        m_nFilteredStateChanges++;

        return S_OK;
    }

    STDMETHOD(SetSamplerState)(THIS_ DWORD dwStage, D3DSAMPLERSTATETYPE d3dSamplerState, DWORD dwValue )
    {
        m_nTotalStateChanges++;

        // If this dwStage is not cached, pass the value through and exit.
        // Otherwise, update the sampler state cache and if the return value is 'true', the 
        // command must be forwarded to the D3D Runtime.
        if( dwStage >= CACHED_STAGES || vecCacheSamplerStates[dwStage].set_val( d3dSamplerState, dwValue ) )
            return m_pDevice->SetSamplerState( dwStage, d3dSamplerState, dwValue );

        m_nFilteredStateChanges++;

        return S_OK;
    }

    STDMETHOD(SetTextureStageState)(THIS_ DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTextureStageState, DWORD dwValue )
    {
        m_nTotalStateChanges++;

        // If this dwStage is not cached, pass the value through and exit.
        // Otherwise, update the texture stage state cache and if the return value is 'true', the 
        // command must be forwarded to the D3D Runtime.
        if( dwStage >= CACHED_STAGES || vecCacheTextureStates[dwStage].set_val( d3dTextureStageState, dwValue ) )
            return m_pDevice->SetTextureStageState( dwStage, d3dTextureStageState, dwValue );

        m_nFilteredStateChanges++;

        return S_OK;
    }
};

//--------------------------------------------------------------------------------------
// Create an extended ID3DXEffectStateManager instance
//--------------------------------------------------------------------------------------
nD3D9EffectStateManager*
nD3D9EffectStateManager::Create( LPDIRECT3DDEVICE9 pDevice )
{
    nD3D9EffectStateManager* pStateManager = NULL;

    D3DDEVICE_CREATION_PARAMETERS cp;
    memset( &cp, 0, sizeof cp );

    if( SUCCEEDED( pDevice->GetCreationParameters( &cp ) ) )
    {
        // A PURE device does not attempt to filter duplicate state changes (with some
        // exceptions) from the driver.  Such duplicate state changes can be expensive
        // on the CPU.  To create the proper state manager, the application determines
        // whether or not it is executing on a PURE device.
        bool bPureDevice = (cp.BehaviorFlags & D3DCREATE_PUREDEVICE) != 0;
        
        //if (bPureDevice)
        //    pStateManager = new nD3D9PureDeviceStateManager( pDevice );
        //else
            pStateManager = new nD3D9BaseStateManager( pDevice );
    }

    NLOGCOND(resource, NULL == pStateManager, (0 | LOG_SHADER, "Failed to Create State Manager"))

    return pStateManager;
}
