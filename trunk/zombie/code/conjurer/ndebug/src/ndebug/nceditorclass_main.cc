//------------------------------------------------------------------------------
//  nceditorclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#ifndef NGAME

#include "ndebug/nceditorclass.h"
#include "entity/nentityclassserver.h"

nNebulaComponentClass(ncEditorClass,nComponentClass);

//------------------------------------------------------------------------------
/**
*/
ncEditorClass::ncEditorClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncEditorClass::~ncEditorClass()
{
    // clear class data
    nVariable ** var;
    nString key;
    
    this->classData.Begin();
    this->classData.Next( key, var );
    while( var )
    {
        n_delete( *var );
        this->classData.Next( key, var );
    }

    // clear asset data
    this->assetData.Begin();
    
    this->assetData.Next( key, var );
    while( var )
    {
        n_delete( *var );
        this->assetData.Next( key, var );
    }
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncEditorClass::InitInstance(nObject::InitInstanceMsg initType)
{
    if( initType != nObject::NewInstance  )
    {
        /*** Init Instance from Class data ***/
        // create filename of debug chunk
        nString filename = nEntityClassServer::Instance()->GetEntityClassFilename( this->GetEntityClass()->nClass::GetName() );
        nString path = filename.ExtractDirName();
        path.Append( "debug/" );
        filename.StripExtension();
        path.Append( filename.ExtractFileName() );
        path.Append( ".n2" );

        // load resource
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);
        if( ps )
        {
            if( nFileServer2::Instance()->FileExists( path.Get() ) )
            {
                nKernelServer::Instance()->PushCwd( this->GetEntityClass() );
                nKernelServer::Instance()->Load( path.Get(), false );
                nKernelServer::Instance()->PopCwd();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @retval true if key is found
*/
bool
ncEditorClass::IsSetClassKey( const nString & key ) const
{
    return (this->classData[ key ] != 0 );
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @retval true if key is found
*/
bool
ncEditorClass::IsSetAssetKey( const nString & key ) const
{
    return (this->assetData[ key ] != 0 );
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param data table with the data
    @returns the data created
*/
nVariable *
ncEditorClass::CreateKeyData( const nString & key, nMapTableTypes<nVariable*>::NString & data )
{   
    nVariable ** var = data[ key ];
    if( ! var )
    {
        nVariable * newVar = n_new( nVariable );
        n_assert( newVar );
        if( ! newVar )
        {
            return 0;
        }

        data.Add( key, &newVar );
        var = &newVar;
    }

    (*var)->Clear();

    return (*var);
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditorClass::SetClassKeyString( const nString & key, const nString & value )
{
    nVariable * var = this->CreateKeyData( key, this->classData );
    if( var )
    {
        var->SetType( nVariable::String );
        var->SetString( value.Get() );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
const char *
ncEditorClass::GetClassKeyString( const nString & key ) const
{
    nVariable ** var = this->classData[ key ];
    if( var )
    {
        if( nVariable::String == (*var)->GetType() )
        {
            return (*var)->GetString();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditorClass::SetClassKeyInt( const nString & key, int value )
{
    nVariable * var = this->CreateKeyData( key, this->classData );
    if( var )
    {
        var->SetType( nVariable::Int );
        var->SetInt( value );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
int
ncEditorClass::GetClassKeyInt( const nString & key ) const
{
    nVariable ** var = this->classData[ key ];
    if( var )
    {
        if( nVariable::Int == (*var)->GetType() )
        {
            return (*var)->GetInt();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditorClass::SetClassKeyFloat( const nString & key, float value )
{
    nVariable * var = this->CreateKeyData( key, this->classData );
    if( var )
    {
        var->SetType( nVariable::Float );
        var->SetFloat( value );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
float
ncEditorClass::GetClassKeyFloat( const nString & key ) const
{
    nVariable ** var = this->classData[ key ];
    if( var )
    {
        if( nVariable::Float == (*var)->GetType() )
        {
            return (*var)->GetFloat();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditorClass::SetAssetKeyString( const nString & key, const nString & value )
{
    nVariable * var = this->CreateKeyData( key, this->assetData );
    if( var )
    {
        var->SetType( nVariable::String );
        var->SetString( value.Get() );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
const char *
ncEditorClass::GetAssetKeyString( const nString & key ) const
{
    nVariable ** var = this->assetData[ key ];
    if( var )
    {
        if( nVariable::String == (*var)->GetType() )
        {
            return (*var)->GetString();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditorClass::SetAssetKeyInt( const nString & key, int value )
{
    nVariable * var = this->CreateKeyData( key, this->assetData );
    if( var )
    {
        var->SetType( nVariable::Int );
        var->SetInt( value );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
int
ncEditorClass::GetAssetKeyInt( const nString & key ) const
{
    nVariable ** var = this->assetData[ key ];
    if( var )
    {
        if( nVariable::Int == (*var)->GetType() )
        {
            return (*var)->GetInt();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @param value data to save in the key
*/
void
ncEditorClass::SetAssetKeyFloat( const nString & key, float value )
{
    nVariable * var = this->CreateKeyData( key, this->assetData );
    if( var )
    {
        var->SetType( nVariable::Float );
        var->SetFloat( value );
    }
}

//------------------------------------------------------------------------------
/**
    @param key name of the key
    @returns the value of the key
*/
float
ncEditorClass::GetAssetKeyFloat( const nString & key ) const
{
    nVariable ** var = this->assetData[ key ];
    if( var )
    {
        if( nVariable::Float == (*var)->GetType() )
        {
            return (*var)->GetFloat();
        }
    }

    return 0;
}

#endif//!NGAME

//------------------------------------------------------------------------------
