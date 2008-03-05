#ifndef N_SCRIPTSERVER_H
#define N_SCRIPTSERVER_H
//------------------------------------------------------------------------------
/**
    @class nScriptServer
    @ingroup NebulaScriptServices

    The script server object is the communication point between
    a nRoot object's Dispatch() method and a specific scripting
    language. More specifically, the script server's main task is to
    translate nCmd objects into script statements and back, and to
    communicate with receiver nRoot objects by sending nCmd objects
    to them (which is done by invoking Dispatch() on the receiver
    with the nCmd object as the argument).

    Script servers are also used as filter for the object serialization.
    An object serializes itself by emitting nCmd objects to the
    file server. The file server will talk to a script server to
    translate those nCmd objects to actual script statements,
    which are then written to the output stream. This stream
    must then simply be fed into a script server to reconstruct
    the serialized object, no special loader code is needed.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "util/nstring.h"
#include "util/nringbuffer.h"

//------------------------------------------------------------------------------
class nFile;
class nScriptServer : public nRoot 
{
public:
    // the select methods (see below)
    enum SelectMethod 
    {
        SELCOMMAND,         // write a select statement after object constructor
        NOSELCOMMAND,       // don't write a select statement
    };

    /// constructor
    nScriptServer();
    /// destructor
    virtual ~nScriptServer();
    /// evaluate a script command
    virtual bool Run(const char * cmdStr, nString& result); 
    /// invoke a script function by name, with no parameters
    virtual bool RunFunction(const char* functionName, nString& result);
    /// evaluate a script file
    virtual bool RunScript(nFile * file, nString& result);
    /// evaluate a script file (default implementation based on previous)
    virtual bool RunScript(const char * filename, nString& result);
    /// set an unnamed object that will receive cmds instead of the cwd
    virtual void SetCurrentTargetObject(nObject *obj);
    /// get the currently set unnamed object
    virtual nObject* GetCurrentTargetObject();
    /// write header of a persistent object file
    virtual bool BeginWrite(nFile * file, nObject* obj, nCmd * cmd);
    /// Write the comment
    virtual bool WriteComment(nFile *, const char *);
    /// write object header with default constructor
    virtual bool WriteBeginNewObject(nFile* fp, nObject *o, nObject *owner);
    /// write object header with constructor cmd 
    virtual bool WriteBeginNewObjectCmd(nFile* fp, nObject *o, nObject *owner, nCmd *cmd);  
    /// write object header without constructor (select only)
    //virtual bool WriteBeginSelObject(nFile* fp, nRoot *o, nRoot *owner); 
    /// translate and write a cmd message
    virtual bool WriteCmd(nFile*, nCmd *);
    /// write object footer
    virtual bool WriteEndObject(nFile* fp, nObject *o, nObject *owner);
    /// finish a persistent object file
    virtual bool EndWrite(nFile*);
    /// abort program on error?
    void SetFailOnError(bool b);
    /// get abort program status
    bool GetFailOnError() const;
    /// set the select method when writing persistent objects
    void SetSelectMethod(SelectMethod sm);
    /// get current select method
    SelectMethod GetSelectMethod() const;
    /// set the quit requested flag
    void SetQuitRequested(bool b);
    /// get the quit requested flag
    bool GetQuitRequested() const;
    /// trigger server (should be called frequently)
    virtual bool Trigger(void);
    /// adds a last output
    void AddOutput( const nString& output );
    /// adds a last output without jumping a line
    void AppendOutput( const nString& output );

    /// returns the last output line and flushes it
    bool FlushLastOutput( nString& output );

    /// Set short overhead in header and end
    void SetUseShortHeader(bool  useShortHeader);
    /// Get if use short header
    bool GetUseShortHeader() const;

#ifndef __XBxX__
    /// generate a prompt string for interactive mode
    virtual nString Prompt(); 
#endif

protected:
    /// when set the script server will dispatch cmds to this object instead of the cwd
    nRef<nObject> currentTargetObject;

private:
    bool quitRequested;
    bool failOnError;
    bool useShortHeader;
    SelectMethod selMethod;

    static const int maxLastOutputLines = 50;

    nRingBuffer<nString> listLastOutput;

    nString lineLastOutput;

    int outputcounter;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nScriptServer::SetQuitRequested(bool b)
{
    this->quitRequested = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nScriptServer::GetQuitRequested() const
{
    return this->quitRequested;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nScriptServer::SetFailOnError(bool b)
{
    this->failOnError = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nScriptServer::GetFailOnError() const
{
    return this->failOnError;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nScriptServer::SetSelectMethod(nScriptServer::SelectMethod sm)
{
    this->selMethod = sm;
}

//------------------------------------------------------------------------------
/**
*/
inline
nScriptServer::SelectMethod
nScriptServer::GetSelectMethod() const
{
    return this->selMethod;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nScriptServer::SetCurrentTargetObject(nObject *obj)
{
    this->currentTargetObject = obj;
}

//------------------------------------------------------------------------------
/**
*/
inline
nObject* 
nScriptServer::GetCurrentTargetObject()
{
    return this->currentTargetObject.get_unsafe();
}

//------------------------------------------------------------------------------
#endif
