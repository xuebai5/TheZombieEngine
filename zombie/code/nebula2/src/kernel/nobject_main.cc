//------------------------------------------------------------------------------
//  nobject_main.cc
//  (c) 2004 Vadim Macagon
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nkernelserver.h"
#include "kernel/nobject.h"
#include "kernel/npersistserver.h"
#include "kernel/ndependencyserver.h"
#include "kernel/nkernelserverproxy.h"
#include "entity/nentityobject.h"
#include "entity/nentityobjectserver.h"
#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"

nNebulaRootClass(nObject);
#ifndef NGAME
NSIGNAL_DEFINE(nObject,ObjectModified);
NSIGNAL_DEFINE(nObject,ObjectDirty);
NSIGNAL_DEFINE(nObject,ObjectChanges);
#endif

//------------------------------------------------------------------------------
/**
*/
nObject::nObject() :
    instanceClass(0)
#ifndef NGAME
    ,
    hidden(false),
    objectDirty(false)
#endif
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nObject::~nObject()
{
    n_assert(this->instanceClass->numInstances > 0);
    this->SetClass(0);

#ifndef NGAME
    nKernelServerProxy * ksp = nKernelServerProxy::Instance();
    if( ksp )
    {
        // this code is to avoid that this signal enters in infinite loop
        // an object is deleted, a signal is generated which can produce the
        // deletion of an aditional object, then a new signal is generated and
        // so forever and ever
        static bool signalingObjectDeleted = false;
        if( !signalingObjectDeleted )
        {
            signalingObjectDeleted = true;
            ksp->SignalObjectDeleted( ksp );
            signalingObjectDeleted = false;
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool 
nObject::Release()
{
    n_assert(this->refCount > 0);
    bool retval = false;
    this->refCount--;
    if (this->refCount == 0) 
    {
        // do not delete as long as mutex is set
        this->LockMutex();
        n_delete(this);
        retval = true;
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Lock the object's main mutex.
*/
void 
nObject::LockMutex()
{
    this->mutex.Lock();
}

//------------------------------------------------------------------------------
/**
    Unlock the object's main mutex.
*/
void
nObject::UnlockMutex()
{
    this->mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    Create a new object executed in an object container.
    @return the object created
*/
nObject * 
nObject::BeginNewObject(const char * objClass, const char * objName)
{
    nObject * retObj = 0;
    n_assert(objClass);

    // check class
    nClass * cl = nKernelServer::Instance()->FindClass(objClass);
    if ( cl && ! cl->IsA("nentityobject") )
    {
        // create object and select it
        if (objName && (0 != objName[0]))
        {
            retObj = kernelServer->New(objClass, objName, false);
        }
        else
        {
            retObj = kernelServer->New(objClass, false);
        }
    }
    else
    {
        // get entity object type for current object
        nEntityObject * eo = static_cast<nEntityObject *> (this);
        nEntityObjectServer::nEntityObjectType otype = nEntityObjectServer::Instance()->GetEntityObjectType(eo->GetId());
        // case when the it is an entity object (a new local id will be assigned)
        retObj = nEntityObjectServer::Instance()->NewEntityObjectFromType(objClass, otype, 0, false);
    }

    // report this to persist server
    if (retObj)
    {
        nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(retObj, nObject::LoadedInstance);
    }
    else
    {
        n_error("nObject::BeginNewObject(%s,%s):failed!\n", objClass, objName);
    }

    return retObj;
}

//------------------------------------------------------------------------------
/**
    end object
*/
void 
nObject::EndObject()
{
    nKernelServer::Instance()->GetPersistServer()->EndObjectLoad(this);
}

//------------------------------------------------------------------------------
/**
*/
void 
nObject::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
nObject::Dispatch(nCmd *cmd)
{
    n_assert(cmd->GetProto());
    cmd->Rewind();
    cmd->GetProto()->Dispatch((void*)this, cmd);
    cmd->Rewind();
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void 
nObject::GetCmdProtos(nHashList *cmd_list)
{
    // for each superclass attach it's command proto names
    // to the list
    nClass *cl = this->instanceClass;
    
    // for each superclass...
    do 
    {
        nHashList *cl_cmdprotos = cl->GetCmdList();
        if (cl_cmdprotos)
        {
            nCmdProto *cmd_proto;
            for (cmd_proto=(nCmdProto *) cl_cmdprotos->GetHead(); 
                 cmd_proto; 
                 cmd_proto=(nCmdProto *) cmd_proto->GetSucc()) 
            {
                nHashNode* node = n_new(nHashNode(cmd_proto->GetName()));
                node->SetPtr((void*)cmd_proto);
                cmd_list->AddTail(node);
            }
        }
    } while ( 0 != (cl = cl->GetSuperClass()) );
}

//------------------------------------------------------------------------------
/**
*/
bool 
nObject::SaveFile(nFile* file)
{
    n_assert(file);
    n_assert(file->IsOpen() );
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // check that no file is saving now
    if( ! ps->IsSaving() )
    {
        // save the name in the dependency server
        nDependencyServer::PushParentFileObject( this, file->GetFilename() );
    }

    bool retval = false;
    if (ps->BeginObject(this, file, false))
    {
        retval = this->SaveCmds(ps);
        ps->EndObject(false);
    } 
    else 
    {
        n_error("nObject::SaveAs(): BeginObject() failed!");
    }

    // check that no file is saving now
    if( ! ps->IsSaving() )
    {
        nDependencyServer::PopParentFileObject();

        // save dependencies of object
        nDependencyServer::SaveDependencies();
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nObject::SaveAs(const char *name)
{
    n_assert(name);
    bool retval = false;

    nFile* file = kernelServer->GetFileServer()->NewFileObject();
    if (file->Open(name, "w"))
    {
        retval = this->SaveFile( file );
        file->Close();
    }
    file->Release();
    return retval;
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nObject::Clone(const char *, bool initInstance)
{
    nObject *clone = NULL;
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    nPersistServer::nSaveMode oldMode = ps->GetSaveMode();
    ps->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
    if (ps->BeginObject(this, (const char*)0, false)) 
    {
        this->SaveCmds(ps);
        ps->EndObject(false);
        clone = ps->GetClone();
    } 
    else 
    {
        n_error("nObject::Clone(): BeginObject() failed!");
    }
    ps->SetSaveMode(oldMode);

    if (clone && initInstance)
    {
        clone->InitInstance(nObject::ClonedInstance);
    }

    return clone;
}

//------------------------------------------------------------------------------
/**
    Save object state for later restoring on an already created object
    
    The saved state can later be restored onto this object or another with the
    same type through the LoadState method. The difference with the "normal"
    SaveAs is that here the state is saved using nPersistServer::BeginObjectWithCmd
    with the 'this' command.
*/
bool 
nObject::SaveStateAs(const char * filename)
{
    n_assert(filename);
    nPersistServer * ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // check that no file is saving now
    if( ! ps->IsSaving() )
    {
        // save the name in the dependency server
        nDependencyServer::PushParentFileObject( 0, filename );
    }

    // save object with BeginObjectWithCmd this
    bool retval = false;
    nCmd * cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, filename)) 
    {
        retval = this->SaveCmds(ps);
        ps->EndObject(false);
    } 
    else 
    {
        n_error("nObject::SaveStateAs(): BeginObjectWithCmd() with command 'this' failed!");
    }

    // check that no file is saving now
    if( ! ps->IsSaving() )
    {
        nDependencyServer::PopParentFileObject();

        // save dependencies of object
        nDependencyServer::SaveDependencies();
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
    Load object state replacing the current state

    Load onto this object a previous state saved with the SaveStateAs method,
    or more exactly, a state saved using nPersistServer::BeginObjectWithCmd
    with the 'this' command.
*/
bool 
nObject::LoadState(const char * filename)
{
    n_assert(filename);

    nKernelServer::Instance()->PushCwd(this);

    nObject * obj = nKernelServer::Instance()->Load(filename, false);

    nKernelServer::Instance()->PopCwd();

    return (obj == this);
}

//------------------------------------------------------------------------------
/**
    This method is usually derived by subclasses to write their peristent 
    attributes to the file server.
*/
bool 
nObject::SaveCmds(nPersistServer *)
{ 
    return true;
}

//------------------------------------------------------------------------------
/**
    Get byte size of this instance. For more accuracy, subclasses should
    add the size of allocated memory.
*/
int
nObject::GetInstanceSize() const
{
    n_assert(this->instanceClass);
    return this->instanceClass->GetInstanceSize();
}

//------------------------------------------------------------------------------
/**
    @brief Checks if a class of the given name is part of the class hierarchy
           for this object.
*/
bool 
nObject::IsA(const char *className) const
{
    return this->IsA(nKernelServer::Instance()->FindClass(className));
}

//------------------------------------------------------------------------------
/**
    @param filename file in which the nRoot is saved
    @param noh path in the Nebula Object Hierarchy of Dependency Server
    @param command command to execute with the nRoot path in the noh after load
*/
void
nObject::SetDependency( const nString & filename, const nString & noh, const nString & command )
{
    // get nRoot object
    nDependencyServer * depServer = static_cast<nDependencyServer*>( nKernelServer::ks->Lookup( noh.Get() ) );
    n_assert2( depServer, "luis.cabellos: No Dependency Server Found" );

    nRoot * obj = static_cast<nRoot*>( depServer->GetObject( filename, command ) );
    n_assert3( obj, ( "Not resolve the dependency, the object not found , %s" , filename.Get() ));
    if (obj)
    {
        // execute command
        nClass * cl = this->GetClass();

        nCmdProto * cmdProto = cl->FindCmdByName( command.Get() );
        n_assert2( cmdProto, "luis.cabellos: command not found in SetDependency" );

        nCmd * cmd = cmdProto->NewCmd();

        cmd->In()->SetS( obj->GetFullName().Get() );
        cmd->Rewind();

        cmdProto->Dispatch( this, cmd );
        cmdProto->RelCmd( cmd );
    }
}

//------------------------------------------------------------------------------
/**
    @param filename file in which the nObject is saved
    @param noh path in the Nebula Object Hierarchy of Dependency Server
    @param command command to execute with the nObject after load
*/
void
nObject::SetDependencyObject( const nString & filename, const nString & noh, const nString & command )
{
    // get nRoot object
    nDependencyServer * depServer = static_cast<nDependencyServer*>( nKernelServer::ks->Lookup( noh.Get() ) );
    n_assert2( depServer, "luis.cabellos: No Dependency Server Found" );

    nObject * obj = depServer->GetObject( filename, command );
    
    // execute command
    nClass * cl = this->GetClass();

    nCmdProto * cmdProto = cl->FindCmdByName( command.Get() );
    n_assert2( cmdProto, "luis.cabellos: command not found in SetDependency" );

    nCmd * cmd = cmdProto->NewCmd();

    cmd->In()->SetO( obj );
    cmd->Rewind();

    cmdProto->Dispatch( this, cmd );
    cmdProto->RelCmd( cmd );
}

//------------------------------------------------------------------------------
/**
Check is the object has the specific command
*/
bool
nObject::HasCommand(const char * commandName)
{
    nClass * cl = this->instanceClass;
    nCmdProto * cmd_proto;
    nHashList * commands;

    do 
    {
        commands = cl->GetCmdList();
        if( commands )
        {
            cmd_proto = static_cast< nCmdProto * >( commands->GetHead() );
            while( cmd_proto )
            {
                if( 0 == strcmp( cmd_proto->GetName(), commandName ) )
                {
                    return true;
                }
                cmd_proto = static_cast< nCmdProto * >( cmd_proto->GetSucc() );
            }
        }
        cl = cl->GetSuperClass();
    }while( cl );

    return false;
}

//------------------------------------------------------------------------------
/**
    @param function function name
    @param numoutargs number of out parameters expected
    @param outargs space to put the out parameters
    @param ...
*/
void
nObject::Call( const char * functionname, int numoutargs, nArg * outargs, ... )
{
    va_list marker;
    va_start( marker, outargs );
    this->CallArgs( functionname, numoutargs, outargs, marker );
    va_end( marker );
}

//------------------------------------------------------------------------------
/**
    @param function function name
    @param numoutargs number of out parameters expected
    @param outargs space to put the out parameters
    @param args varible list of arguments
*/
void
nObject::CallArgs( const char * functionname, int numoutargs, nArg * outargs, va_list args )
{
    nCmdProto *cp = this->GetClass()->FindCmdByName( functionname );
    n_assert2( cp, "Cmd not found");
    if( cp )
    {
        nCmd * cmd = cp->NewCmd();
        n_assert2( cmd, "Cant create cmd" );
        if( cmd ){
            cmd->CopyInArgsFrom( args );

            cmd->Rewind();
            cmd->GetProto()->Dispatch( (void*)this, cmd );

            // get output
            if( outargs )
            {
                int i = 0;

                cmd->Rewind();
                while( i < numoutargs && i < cmd->GetProto()->GetNumOutArgs() )
                {
                    nArg * argDst = &outargs[ i++ ];
                    nArg * argSrc = cmd->Out();
                    argDst->Copy( *argSrc );
                }
            }

            cp->RelCmd( cmd );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param function function name
    @param numoutargs number of out parameters expected
    @param outargs space to put the out parameters
    @param args array of nArg arguments
*/
void
nObject::CallArgs( const char * functionname, int numoutargs, nArg * outargs, int numargs, nArg * args )
{
    nCmdProto *cp = this->GetClass()->FindCmdByName( functionname );
    n_assert2( cp, "Cmd not found");
    if( cp )
    {
        nCmd * cmd = cp->NewCmd();
        n_assert2( cmd, "Cant create cmd" );
        if( cmd ){
            cmd->CopyInArgsFrom( numargs, args );

            cmd->Rewind();
            cmd->GetProto()->Dispatch( (void*)this, cmd );

            // get output
            if( outargs )
            {
                int i = 0;

                cmd->Rewind();
                while( i < numoutargs && i < cmd->GetProto()->GetNumOutArgs() )
                {
                    nArg * argDst = &outargs[ i++ ];
                    nArg * argSrc = cmd->Out();
                    argDst->Copy( *argSrc );
                }
            }

            cp->RelCmd( cmd );
        }
    }
}

#ifdef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
/**
*/
void 
nObject::SetComment(const nString& comment)
{
    this->comment = comment; 
}
#endif


#ifdef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
/**
*/
const nString& 
nObject::GetComment() const
{
    return this->comment;
}
#endif


#ifdef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
/**
*/
void 
nObject::AppendComment(const nString& comment)
{
    this->comment += "\n"+comment;
}
#endif

#ifdef __ZOMBIE_EXPORTER__
//------------------------------------------------------------------------------
/**
*/
void _cdecl
nObject::AppendCommentFormat(const char* fmtString, ...)
{
    va_list argList;
    va_start(argList, fmtString);
    nString comment;
    comment.FormatWithArgs(fmtString, argList);
    this->comment += "\n"+comment;
    va_end(argList);
}
#endif


//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
bool
nObject::IsObjectDirty() const
{
    return this->objectDirty;
}
#endif

//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
void
nObject::SetObjectDirty(bool dirty)
{
    if (  dirty )
    {
        if ( !this->objectDirty)
        {
            this->SignalObjectDirty(this,this);
        }
        this->SignalObjectModified(this,this);
        this->SignalObjectChanges(this);
    }

    this->objectDirty =  dirty;
}
#endif

//------------------------------------------------------------------------------
/**
*/
#ifndef NGAME
void
nObject::TriggerObjectDirty(nObject* /*emitter*/)
{
    this->SetObjectDirty(true);
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool 
nObject::CopyStateFrom(nObject * obj)
{
    bool retval = false;

    if ( obj && this->IsA(obj->GetClass()) )
    {
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);

        nPersistServer::nSaveMode oldMode = ps->GetSaveMode();
        ps->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
        nObject * oldClone = ps->GetClone();
        ps->SetClone(this);

        nKernelServer::Instance()->PushCwd(this);
        if (obj->SaveCmds(ps))
        {
            retval = true;
        }
        nKernelServer::Instance()->PopCwd();

        ps->SetClone(oldClone);
        ps->SetSaveMode(oldMode);
    }

    return retval;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
