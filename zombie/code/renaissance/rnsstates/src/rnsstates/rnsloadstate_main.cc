//------------------------------------------------------------------------------
//  rnsloadstate_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsstates.h"

#include "rnsstates/rnsloadstate.h"

#include "entity/nentityclassserver.h"

#include "zombieentity/nloaderserver.h"

#include "nspecialfx/nfxserver.h"
#include "nspecialfx/nfxmaterial.h"

#include "gameplay/ngamematerialserver.h"
#include "gameplay/ngamematerial.h"

#include "nmusictable/nmusictable.h"

//------------------------------------------------------------------------------
nNebulaScriptClass( RnsLoadState, "ncommonstate" );

//------------------------------------------------------------------------------
const char * RnsLoadState::PreloadFile = "wc:libs/preload.n2";

//------------------------------------------------------------------------------
/**
*/
RnsLoadState::RnsLoadState():
    maxPreLoads( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
RnsLoadState::~RnsLoadState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
RnsLoadState::OnCreate(nApplication* application)
{
    nCommonState::OnCreate(application);

    // The level must be loaded before entering into game state or otherwise
    // initial gameplay entities aren't registered in nRnsEntityManager
    this->loadEnabled = true;
}

//------------------------------------------------------------------------------
/**
    @param prevState previous state
*/
void 
RnsLoadState::OnStateEnter( const nString & prevState )
{
    if( this->nextStateName.IsEmpty() )
    {
        this->nextStateName = prevState;
    }

    this->firstFrame = true;

    // get the resources to preload from config file
    if (kernelServer->GetFileServer()->FileExists( RnsLoadState::PreloadFile ) )
    {
        kernelServer->PushCwd( this );
        kernelServer->Load( RnsLoadState::PreloadFile , false);
        kernelServer->PopCwd();
    }

    // get the resources to preload from game materials
    nFXServer::Instance()->LoadFxMaterials();

    nGameMaterialServer * gmServer = nGameMaterialServer::Instance();
    n_assert( gmServer );
    if( gmServer )
    {
        nGameMaterial * gameMaterial = 0;
        for( int i = 0 ; i < gmServer->GetNumMaterials() ; ++i )
        {
            gameMaterial = gmServer->GetMaterialByIndex( i );
            n_assert( gameMaterial );
            this->PreloadGameMaterial( gameMaterial );
        }
    }

    // get the resources from sound
    nAutoRef<nMusicTable> refMusicTable(musicTableNOHPath);
    if (refMusicTable.isvalid())
    {
        refMusicTable->LoadAllSamples();
    }

    this->maxPreLoads = this->preLoadClasses.Size();

    nCommonState::OnStateEnter( prevState );
}

//------------------------------------------------------------------------------
/**
*/
void
RnsLoadState::OnFrame()
{
    // check if there is something to load
    if( this->preLoadClasses.Empty() )
    {
        if( this->nextStateName.IsEmpty() )
        {
            this->app->SetQuitRequested(true);
        }
        else
        {
            this->app->SetState( this->nextStateName );
        }
    }
    // drop the first frame to show the load message
    else if( this->firstFrame )
    {
        this->firstFrame = false;
    }
    // load one resource per frame
    else
    {
        nString className( this->preLoadClasses.Back() );

        this->preLoadClasses.Erase( this->preLoadClasses.End() - 1 );

        nEntityClass * entityClass = nEntityClassServer::Instance()->GetEntityClass( className.Get() );

        if( entityClass )
        {
            nLoaderServer::Instance()->LoadClassResources( entityClass );
            nLoaderServer::Instance()->RetainClassResources( entityClass );
        }
    }

    nCommonState::OnFrame();
}

//------------------------------------------------------------------------------
/**
    @param stateName the next state before load
*/
void
RnsLoadState::SetNextState( const char * stateName )
{
    this->nextStateName = stateName;
}

//------------------------------------------------------------------------------
/**
    @param name name of the class to preload
*/
bool
RnsLoadState::InsertPreloadClass( const char * name )
{
    if( name )
    {
        if( this->preLoadClasses.Find( nString( name ) ) )
        {
            return true;
        }

        if( nEntityClassServer::Instance()->ExistEntityClass( name ) )
        {
            this->preLoadClasses.Append( nString( name ) );
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @param gameMaterial game materia lto preload
*/
void
RnsLoadState::PreloadGameMaterial( nGameMaterial * const gameMaterial )
{
    if( gameMaterial )
    {
        nFxMaterial * fxMaterial = 0;
        nFxMaterial::nFxNode * fxNode = 0;
        nString fxName;

        fxMaterial = gameMaterial->GetFxMaterialObject();
        if( fxMaterial )
        {
            for( int i = 0 ; i < fxMaterial->GetNumFxNodes() ; ++i )
            {
                fxNode = fxMaterial->GetFxNodeByIndex( i );

                for( int j = 0 ; j < fxNode->fxlist.Size() ; ++j )
                {
                    this->InsertPreloadClass( fxNode->fxlist[ j ].Get() );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @returns the percent
*/
float
RnsLoadState::GetPercent()const
{
    if ( this->maxPreLoads > 0 )
    {
        return ( 100.0f * ( 1.0f - this->preLoadClasses.Size() / float( this->maxPreLoads ) ) );
    }

    return 100;
}

//------------------------------------------------------------------------------
