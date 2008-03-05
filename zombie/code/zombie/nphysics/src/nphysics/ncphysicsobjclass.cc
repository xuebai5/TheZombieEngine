//------------------------------------------------------------------------------
//  ncphysicsobjclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/ncphysicsobjclass.h"
#include "file/nmemfile.h"
#include "zombieentity/ncloaderclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncPhysicsObjClass,nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhysicsObjClass)
    NSCRIPT_ADDCMD_COMPCLASS('DBMA', void, SetMass, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DBDE', void, SetDensity, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DBMG', const phyreal, GetMass, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DBDG', const phyreal, GetDensity, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DBSM', void, SetMobile, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DBGM', const bool, GetMobile, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 15-Jun-2005   David Reyes    created
*/
ncPhysicsObjClass::ncPhysicsObjClass() : 
    objectMass(0),
    objectDensity(1) ,
    mobile( false ),
    chunkFile(0)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 23-Feb-2006   Cristobal Castillo    created
*/
ncPhysicsObjClass::~ncPhysicsObjClass()
{
    this->UnLoadChunkFile();
}


//-----------------------------------------------------------------------------
/**
    Object persistency.

    history:
        - 15-Jun-2005   David Reyes    created
*/
bool ncPhysicsObjClass::SaveCmds(nPersistServer *ps)
{
    nCmd* cmd(0);

    // persist the object mass
    cmd = ps->GetCmd( this->GetEntityClass(), 'DBMA' );

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMass() );

    ps->PutCmd(cmd);        

    // persist the object density
    cmd = ps->GetCmd( this->GetEntityClass(), 'DBDE' );

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetDensity() );

    ps->PutCmd(cmd);        

    // persist the object mobility
    cmd = ps->GetCmd( this->GetEntityClass(), 'DBSM' );

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetB( this->GetMobile() );

    ps->PutCmd(cmd);        

    return true;
}

//-----------------------------------------------------------------------------
/**
    Sets if the object is mobile.

    @param is true/false

    history:
        - 20-Feb-2006   David Reyes    created
*/
void ncPhysicsObjClass::SetMobile(const bool is)
{
    this->mobile = is;
}

//-----------------------------------------------------------------------------
/**
    Gets if the object is mobile.

    @return true/false

    history:
        - 20-Feb-2006   David Reyes    created
*/
const bool ncPhysicsObjClass::GetMobile() const
{
    return this->mobile;
}

//-----------------------------------------------------------------------------
/**
    Free the memFile

    history:
        - 23-Feb-2006   Cristobal Castillo    created
*/
void
ncPhysicsObjClass::UnLoadChunkFile()
{
    //this->chunkFile = n_new( nMemFile );
    if  ( this->chunkFile )
    {
        this->chunkFile->Release(); // The release close the file if is necesry
        this->chunkFile = 0;
    }
  
}
//-----------------------------------------------------------------------------
/**
    Free the memFile

    history:
        - 23-Feb-2006   Cristobal Castillo    created
*/
nFile* 
ncPhysicsObjClass::GetChunkFile()
{
    if ( this->chunkFile )
    {
        if ( this->chunkFile->IsOpen() )
        {
            this->chunkFile->Seek( 0, nFile::START );
        }
    } else
    {
        this->chunkFile = n_new( nMemFile  );
        n_assert( this->chunkFile );
        ncLoaderClass *loaderClass(this->GetComponent<ncLoaderClass>());
        nString emptyFilename;

        if ( loaderClass && chunkFile->Open(emptyFilename, "r") )
        {
            nString fileName;
            
            
            if( loaderClass->GetResourceFile() )
            {
                fileName.Append( loaderClass->GetResourceFile() );
            }
            fileName.Append( "/physics/chunk.n2" );


            nFile* sourceFile = nFileServer2::Instance()->NewFileObject();
            n_assert( sourceFile );
            if ( sourceFile->Open( fileName , "rb" ) )
            {
                this->chunkFile->AppendFile( sourceFile );
                this->chunkFile->Seek( 0, nFile::START );
            }
            else
            {
                this->chunkFile->Close(); // Mark has not a file
            }
            sourceFile->Release();
        } else
        {
            // Empty
            // If not has File then chunkFile is closed.
            // This form is for no test if file exist in the next GetChunkFile
        }
    }
    return this->chunkFile;
}