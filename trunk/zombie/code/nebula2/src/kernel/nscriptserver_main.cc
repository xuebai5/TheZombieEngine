//------------------------------------------------------------------------------
//  nscriptserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nscriptserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"

nNebulaScriptClass(nScriptServer, "nroot");

//------------------------------------------------------------------------------
/**
*/
nScriptServer::nScriptServer() :
    quitRequested(false),
    failOnError(true),
    selMethod(SELCOMMAND),
    listLastOutput( maxLastOutputLines ),
    lineLastOutput(""),
    useShortHeader(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nScriptServer::~nScriptServer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Run a script statement.

    @param  cmdStr      the statement to execute
    @param  result      [out] will be filled with the result
    @return             false if an error occured during execution
*/
bool 
nScriptServer::Run(const char* /* cmdStr */, nString& /* result */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    Run a script function with the specified name without any args

    @param functionName     the function to invoke
    @param result           [out] will be filled with the result
    @return                 false if an error occured during execution
*/
bool
nScriptServer::RunFunction(const char* /*functionName*/, nString& /*result*/)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    Run a script file provided by nFile.

    @param  file        the nFile to use for reading
    @param  result      [out] will be filled with the result
    @return             false if an error occured during execution
*/
bool
nScriptServer::RunScript(nFile * /*file*/, nString& /* result */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    Run a script file provided by filename.

    @param  filename    the script filename (ABSOLUTE PATH!)
    @param  result      [out] will be filled with the result
    @return             false if an error occured during execution
*/
bool
nScriptServer::RunScript(const char* filename, nString& result)
{
    bool success = false;

    nFile * file = nFileServer2::Instance()->NewFileObject();
    n_assert(file);
    if (!file)
    {
        return false;
    }

    if (file->Open(filename, "rb"))
    {
        success = this->RunScript(file, result);
        file->Close();
    }
    file->Release();

    return success;
}

//------------------------------------------------------------------------------
/**
*/
#ifndef __XBxX__
nString
nScriptServer::Prompt()
{
    nString emptyString;
    return emptyString;
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool
nScriptServer::BeginWrite(nFile * /* file */, nObject* /* obj */, nCmd * /* cmd */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteBeginNewObject(nFile *file, nObject *obj, nObject * /*owner*/)
{
    n_assert(file);
    n_assert(obj);

    const char* objClass = obj->GetClass()->GetName();

    // write 'beginnewobject' statement
    nCmdProto *cp = obj->GetClass()->FindCmdById('BENO');
    n_assert(cp);
    nCmd * cmd = cp->NewCmd();
    n_assert(cmd);
    cmd->In()->SetS( objClass );
    if (obj->IsA("nroot"))
    {
        cmd->In()->SetS(((nRoot *) obj)->GetName());
    }
    else
    {
        cmd->In()->SetS( "" );
    }
    bool success = this->WriteCmd(file, cmd);
    cp->RelCmd(cmd);

    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteBeginNewObjectCmd(nFile *file, nObject * /*o*/, nObject * /*owner*/, nCmd *cmd)
{
    n_assert(file);
    //n_assert(o);
    n_assert(cmd);

    // write constructor statement
    return this->WriteCmd(file, cmd);
}

//------------------------------------------------------------------------------
/**
*/
#if 0
bool 
nScriptServer::WriteBeginSelObject(nFile*, nRoot *, nRoot *)
{
   // overwrite in subclass
   return false;
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteCmd(nFile*, nCmd *)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nScriptServer::WriteEndObject(nFile* file, nObject * o, nObject * /*owner*/)
{
    n_assert(file);
    n_assert(o);

    // write end object
    nCmdProto *cp = o->GetClass()->FindCmdById('ENDO');
    n_assert(cp);
    nCmd * cmd = cp->NewCmd();
    n_assert(cmd);
    bool success = this->WriteCmd(file, cmd);
    cp->RelCmd(cmd);

    return success;
}

//------------------------------------------------------------------------------
/**
*/
bool
nScriptServer::EndWrite(nFile* /* file */)
{
    // overwrite in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
    This method should be called frequently (normally once per frame) when
    the script server is not run in interactive mode.

     - 21-Dec-98   floh    created
     - 31-Aug-99   floh    'quit_requested' wird zurueckgesetzt, wenn
                           es einmal true war
*/
bool 
nScriptServer::Trigger()
{
    bool retval = !(this->quitRequested);
    this->quitRequested = false;
    return retval;
}

//------------------------------------------------------------------------------
/**
    Adds the last ouput.

     - 27-May-2005   David Reyes    created
*/
void 
nScriptServer::AddOutput( const nString& output )
{
    if( this->listLastOutput.IsFull() )
    {
        this->listLastOutput.DeleteTail();
    }

    nString* newline(this->listLastOutput.Add());

    n_assert2( newline, "Failed to add a new output entry." );

    *newline = output;
}

//------------------------------------------------------------------------------
/**
    Adds a last output without jumping a line.

     - 27-May-2005   David Reyes    created
*/
void nScriptServer::AppendOutput( const nString& output )
{
    if( output.FindChar( '\n',0 ) != -1 )
    {
        this->lineLastOutput += output;
        this->AddOutput( this->lineLastOutput );
        this->lineLastOutput = "";
    }
    else
    {
        this->lineLastOutput += output;
    }
}

//------------------------------------------------------------------------------
/**
    Returns the last output line and flushes it.

     - 27-May-2005   David Reyes    created
*/
bool nScriptServer::FlushLastOutput( nString& output )
{
    if( this->listLastOutput.IsEmpty() )
    {
        return false;
    }

    output = *this->listLastOutput.GetTail();

    this->listLastOutput.DeleteTail();

    return true;
}

bool 
nScriptServer::WriteComment(nFile *, const char *)
{
    /// Empty
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void nScriptServer::SetUseShortHeader(bool  useShortHeader)
{
    this->useShortHeader = useShortHeader;
}

//------------------------------------------------------------------------------
/**
*/
bool nScriptServer::GetUseShortHeader() const
{
    return this->useShortHeader;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------


