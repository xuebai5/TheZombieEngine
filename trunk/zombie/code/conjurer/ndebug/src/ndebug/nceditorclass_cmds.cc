//------------------------------------------------------------------------------
//  nceditorclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#ifndef NGAME
//------------------------------------------------------------------------------
#include "ndebug/nceditorclass.h"
#include "entity/nentityclassserver.h"
#include "zombieentity/ncloaderclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncEditorClass)
    NSCRIPT_ADDCMD_COMPCLASS('LSCH', void, SaveChunk , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSCK', void, SaveAssetChunk , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LICS', bool, IsSetClassKey, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LIAS', bool, IsSetAssetKey, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSCS', void, SetClassKeyString, 2, (const nString &, const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGCS', const char *, GetClassKeyString, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSCI', void, SetClassKeyInt, 2, (const nString &, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGCI', int, GetClassKeyInt, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSCF', void, SetClassKeyFloat, 2, (const nString &, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGCF', float, GetClassKeyFloat, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSAS', void, SetAssetKeyString, 2, (const nString &, const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGAS', const char *, GetAssetKeyString, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSAI', void, SetAssetKeyInt, 2, (const nString &, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGAI', int, GetAssetKeyInt, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSAF', void, SetAssetKeyFloat, 2, (const nString &, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGAF', float, GetAssetKeyFloat, 1, (const nString &), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @param data table with the data
*/
void
ncEditorClass::SaveVariableMap( nPersistServer * ps, nMapTableTypes<nVariable*>::NString & data )
{
    nFourCC command;
    nVariable ** var;
    nString key;

    data.Begin();
    data.Next( key, var );
    while( var )
    {
        switch( (*var)->GetType() )
        {
            case nVariable::String:
                command = ( &data == &this->classData)?('LSCS'):('LSAS');
                ps->Put( this->GetEntityClass(), command, key.Get(), (*var)->GetString() );
                break;

            case nVariable::Int:
                command = ( &data == &this->classData)?('LSCI'):('LSAI');
                ps->Put( this->GetEntityClass(), command, key.Get(), (*var)->GetInt() );
                break;

            case nVariable::Float:
                command = ( &data == &this->classData)?('LSCF'):('LSAF');
                ps->Put( this->GetEntityClass(), command, key.Get(), (*var)->GetFloat() );
                break;

            default:
                n_assert_always();
        }
        data.Next( key, var );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncEditorClass::SaveChunk( )
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);
    if( ps )
    {
        // create filename of debug chunk
        nString filename = nEntityClassServer::Instance()->GetEntityClassFilename( this->GetEntityClass()->nClass::GetName() );
        nString path = filename.ExtractDirName();
        path.Append( "debug/" );
        filename.StripExtension();
        path.Append( filename.ExtractFileName() );
        path.Append( ".n2" );

        // change command to 'THIS'
        nCmd * cmd = ps->GetCmd( this->GetEntityClass(), 'THIS' );
        n_assert( cmd );
        if( cmd )
        {
            if( nFileServer2::Instance()->MakePath( path.ExtractDirName().Get() ) )
            {
                if( ps->BeginObjectWithCmd( this->GetEntityClass(), cmd, path.Get() ) ) 
                {
                    // save commands
                    this->SaveVariableMap( ps, this->classData );

                    ps->EndObject( true );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncEditorClass::SaveAssetChunk( )
{
    ncLoaderClass * loaderClass = this->GetComponent<ncLoaderClass>();
    n_assert( loaderClass );
    if( loaderClass )
    {
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);
        if( ps )
        {
            // create filename of debug chunk
            nString path = loaderClass->GetResourceFile();
            if( path.GetExtension() )
            {
                path.StripExtension();
            }
            path.Append( "/debug/chunk.n2" );

            // change command to 'THIS'
            nCmd * cmd = ps->GetCmd( this->GetEntityClass(), 'THIS' );
            n_assert( cmd );
            if( cmd )
            {
                if( nFileServer2::Instance()->MakePath( path.ExtractDirName().Get() ) )
                {
                    if( ps->BeginObjectWithCmd( this->GetEntityClass(), cmd, path.Get() ) ) 
                    {
                        // save commands
                        this->SaveVariableMap( ps, this->assetData );

                        ps->EndObject( true );
                    }
                }
            }
        }
    }
}

#endif//!NGAME

//------------------------------------------------------------------------------
