#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  nfxmaterial_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/nfxmaterial.h"
#include "nspecialfx/ncspecialfxclass.h"
#include "nspecialfx/nfxserver.h"

//-----------------------------------------------------------------------------
nNebulaScriptClass(nFxMaterial, "nroot");

//-----------------------------------------------------------------------------
/**
*/
nFxMaterial::nFxMaterial() : 
    specialfx(0,4),
    lastFxEffects(0),
    matEffects(7) // The hash's size
#ifndef NGAME    
    , isDirty(false)
    , hasToBeRemoved(false)
#endif
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
nFxMaterial::~nFxMaterial()
{
    this->specialfx.Clear();
}

//-----------------------------------------------------------------------------
/**
*/
void
nFxMaterial::BeginFx( const char* id, const char* fxType, int numFx)
{
    nFxNode newNode;
    newNode.id = id;
    newNode.type = ncSpecialFXClass::StringToType(fxType);
    newNode.fxlist.SetFixedSize(numFx);
    newNode.keys.SetFixedSize(numFx);
    this->specialfx.Append( newNode );
}

//-----------------------------------------------------------------------------
/**
*/
bool
nFxMaterial::AddFxClass(int index, const char* fxClassName)
{
    nFxNode& fxNode = this->specialfx.Back();    
    if ( index < fxNode.fxlist.Size() )
    {
        fxNode.fxlist[index] = fxClassName;
    } else
    {
        NLOG( specialfx,  ( 0 | nFXServer::CREATION, "Incorrect fxMaterial  %s , when add  %s to %s" , this->GetName() , fxClassName , fxNode.id.Get() ));
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
*/
void
nFxMaterial::EndFx()
{
}

//-----------------------------------------------------------------------------
/**
*/
nFxMaterial::nFxNode*
nFxMaterial::GetFxNodeFor(nFxEffects* effect, int idx ) const
{
    n_assert( effect )
    nFxNode* node = GetFxNodeByIndex( effect->effectsIndexList[idx] ); 
    n_assert( (node == 0 ) || ( 0 == strcmp( node->id.Get() , effect->effectsNameList[idx].Get() ) ) );// The same string id
    return node;
}

//-----------------------------------------------------------------------------
/**
*/    
int
nFxMaterial::GetNumFxNodes() const
{
    return this->specialfx.Size();
}

//-----------------------------------------------------------------------------
/**
*/
nFxMaterial::nFxNode* 
nFxMaterial::GetFxNodeByIndex( int index ) const
{
    n_assert(index < this->specialfx.Size());
    return &this->specialfx[index];
}


//-----------------------------------------------------------------------------
/**
    Search the special fx by name
*/
int
nFxMaterial::FindSpecialFxIndex(const char* id) const
{
    for ( int idx = 0; idx < this->specialfx.Size() ; ++idx)
    {
        if ( 0 == strcmp(this->specialfx[idx].id.Get(), id ) )
        {
            return idx;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
*/
void
nFxMaterial::BeginEvent( const char* eventType, int numFx)
{
    nFxEffects newEffects;
    newEffects.eventType = eventType;
    newEffects.effectsIndexList.SetFixedSize(numFx);
#ifndef __NEBULA_NO_ASSERT__
    newEffects.effectsNameList.SetFixedSize(numFx); 
#endif
    this->matEffects.Add( eventType, &newEffects );
    this->lastFxEffects = this->matEffects[eventType];
}

//-----------------------------------------------------------------------------
/**
*/
bool
nFxMaterial::AddEffect(int index, const char* effectName)
{
    if ( this->lastFxEffects )
    {
        if ( index  < this->lastFxEffects->effectsIndexList.Size()  )
        {
#ifndef __NEBULA_NO_ASSERT__
            this->lastFxEffects->effectsNameList[index] = effectName; 
#endif
            this->lastFxEffects->effectsIndexList[index] = FindSpecialFxIndex( effectName );

            bool result = this->lastFxEffects->effectsIndexList[index] >= 0;

            NLOGCOND(specialfx,  !result, ( 0 | nFXServer::CREATION, "Incorrect fxMaterial  %s , when add  %s to %s, invalid name" , this->GetName() , effectName , this->lastFxEffects->eventType.Get() ));
            return result;
        } else
        {
            NLOG(specialfx, ( 0 | nFXServer::CREATION, "Incorrect fxMaterial  %s , when add  %s to %s , invalid index" , this->GetName() , effectName , this->lastFxEffects->eventType.Get() ));
        }
    } else
    {
        n_assert3_always( ("Incorrect fxMaterial  %s , when add  %s , possibly not put begin event command" , this->GetName() , effectName  ) );
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
*/
void
nFxMaterial::EndEvent()
{
    //
}

//-----------------------------------------------------------------------------
/**
*/
nFxMaterial::nFxEffects*
nFxMaterial::GetFxEffectFor( const char* eventType ) const
{
    return this->matEffects[ eventType];
}


#ifndef NGAME
//-----------------------------------------------------------------------------
/**
*/
bool 
nFxMaterial::GetToBeRemoved() const
{
    return this->hasToBeRemoved;
}

//-----------------------------------------------------------------------------
/**
*/
void 
nFxMaterial::SetToBeRemoved( bool has )
{
    if( has )
    {
        this->isDirty = true;
        this->HideInEditor();
    }
    else
    {
        this->UnHideInEditor();
    }
    this->hasToBeRemoved = has;
}

//-----------------------------------------------------------------------------
/**
*/
bool 
nFxMaterial::IsDirty()
{
    return this->isDirty;
}

#endif

//-----------------------------------------------------------------------------
/**
    @return the effect type of the neSpecialFX
*/
ncSpecialFXClass::FxType
nFxMaterial::nFxNode::GetType(int /*idx*/)
{
    //Now all neSpecialFx of this array has the same effect type.
    return this->type;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

