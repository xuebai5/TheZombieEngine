//------------------------------------------------------------------------------
//  nceditor_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#ifndef NGAME

#include "ndebug/nceditor.h"

#include "entity/nentityobjectserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncEditor)
    NSCRIPT_ADDCMD_COMPOBJECT('LSCH', void, SaveChunk , 0, (), 0, ());
    /*NSCRIPT_ADDCMD_COMPOBJECT('CSCH', void, SaveChunkFile, 1, (nFile*), 0, ());*/
    NSCRIPT_ADDCMD_COMPOBJECT('LISK', bool, IsSetEditorKey, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSES', void, SetEditorKeyString, 2, (const nString &, const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGES', const char *, GetEditorKeyString, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSEI', void, SetEditorKeyInt, 2, (const nString &, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGEI', int, GetEditorKeyInt, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSEF', void, SetEditorKeyFloat, 2, (const nString &, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGEF', float, GetEditorKeyFloat, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSLI', void, SetLayerId, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGLI', int, GetLayerId , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JISD', bool, IsDeleted , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSED', void, SetDeleted, 1, (bool), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
void
ncEditor::SaveChunkFile(nFile* file )
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);
    if( ps )
    {
        // change command to 'THIS'
        nCmd * cmd = ps->GetCmd( this->GetEntityObject(), 'THIS' );
        n_assert( cmd );
        if( cmd )
        {
            if( ps->BeginObjectWithCmd( this->GetEntityObject(), cmd, file ) ) 
            {
                // begin save commands
                nVariable ** var;
                nString key;

                this->editorData.Begin();
                this->editorData.Next( key, var );
                while( var )
                {
                    switch( (*var)->GetType() )
                    {
                        case nVariable::String:
                            ps->Put( this->GetEntityObject(), 'LSES', key.Get(), (*var)->GetString() );
                            break;

                        case nVariable::Int:
                            ps->Put( this->GetEntityObject(), 'LSEI', key.Get(), (*var)->GetInt() );
                            break;

                        case nVariable::Float:
                            ps->Put( this->GetEntityObject(), 'LSEF', key.Get(), (*var)->GetFloat() );
                            break;

                        default:
                            n_assert_always();
                    }
                    this->editorData.Next( key, var );
                }
                // end save commands

                ps->EndObject( true );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncEditor::SaveChunk( )
{
    // create filename of debug chunk
    nString filename = nEntityObjectServer::Instance()->GetEntityObjectFilename( this->GetEntityObject()->GetId() );
    nString path = filename.ExtractDirName();
    path.Append( "debug/" );
    filename.StripExtension();
    path.Append( filename.ExtractFileName() );
    path.Append( ".n2" );

    nFileServer2::Instance()->MakePath( path.ExtractDirName().Get() );

    nFile* file = nKernelServer::Instance()->GetFileServer()->NewFileObject();
    if (file->Open(path.Get(), "w"))
    {
        this->SaveChunkFile( file );
        file->Close();
    } else
    {
        n_assert2_always( (nString("Can't save chunk for entity , ") + path  ).Get() );
    }
    file->Release();
}

#endif//!NGAME

//------------------------------------------------------------------------------
