//------------------------------------------------------------------------------
//  npersistentserver_beginend.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/npersistserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "entity/nentityobject.h"
#include "entity/nentityobjectserver.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    Write persistent file header.

     - 05-Nov-98   floh    created
     - 29-Mar-99   floh    + rewritten to FOpen()
     - 23-May-02   floh    + rewritten to nFile
     - 08-Nov-04   enlight + now takes nObject* instead of nRoot*
*/
bool
nPersistServer::PutFoldedObjectHeader(nScriptServer *saver, nFile *file, nObject* obj, nCmd * cmd)
{
    n_assert(saver);
    //n_assert(fname);

    //check if the file already exist
#if 0 
    // These lines were only in binscriptserver, and they have been moved here
    // after a refactoring. It causes troubles with assign groups if the read
    // param is not false.
    if (kernelServer->GetFileServer()->FileExists(fname, false))
    {
        //delete the old file before writing the new
        kernelServer->GetFileServer()->DeleteFile(fname, false);
    }
#endif

    //nFile * file = this->GetPersistFile();
    n_assert(file);
    if (file)
    {
        if (file->IsOpen())
        {
            if (saver->BeginWrite(file, obj, cmd))
            {
                return true;
            }
            //file->Close();
        }
        //file->Release();
        //file = 0;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Start writing a persistent object into a persistent object file.

     - 04-Nov-98    floh     created
     - 11-Nov-98    floh    + Removed support for root dir,
                               now always saves to cwd
     - 20-Jan-00    floh    + rewritten for ref_ss
     - 29-Feb-00    floh    + optional constructor cmd object
     - 06-Mar-00    floh    + 'sel_only' argument
                            + 'owner' object handling, which is the
                              object at the top of stack
     - 16-Feb-04    floh    + no longer appends an ".n2" to path
     - 09-Nov-04    enlight + nObject support
*/
bool 
nPersistServer::BeginFoldedObject(nObject *obj, nCmd *cmd, nFile* file, bool /*selOnly*/,
                                  bool /*isObjNamed*/)
{
    bool fileOk = false;
    n_assert( file || this->file);

    // if file not available -> open file with header 
    if (0 == this->file) 
    {
        this->file = file;
        if ( PutFoldedObjectHeader(this->refSaver.get(), file, obj, cmd)  )
        {
            fileOk = true;
            #ifdef __ZOMBIE_EXPORTER__
            if ( ! obj->GetComment().IsEmpty() )
            {
                this->PutComment( obj->GetComment().Get() );
                this->PutComment( " " );
            }
            #endif
        }
        else 
        {
            n_printf("nPersistServer: could not open file '%s' for writing!\n", name);
        }
    }
    else
    {
        fileOk = true;

        // get owner/container object 
        nObject * owner = 0;
        if (! this->objectStack.IsEmpty())
        {
            owner = this->objectStack.Top();
        }
        else
        {
            owner = this;
        }

        #ifdef __ZOMBIE_EXPORTER__
        if ( ! obj->GetComment().IsEmpty() )
        {
            this->PutComment( " " );
            this->PutComment( obj->GetComment().Get() );
        }
        #endif

        // write begin new object
        if (cmd) 
        {
            this->refSaver->WriteBeginNewObjectCmd(this->file, obj, owner, cmd);
        }
        else
        {
            this->refSaver->WriteBeginNewObject(this->file, obj, owner);
        }
    }

    // push object in stack
    if (fileOk)
    {
        this->objectStack.Push(obj);
    }

    return fileOk;
}

//------------------------------------------------------------------------------
/** 
    Finish writing persistent object.

     - 04-Nov-98   floh    created
     - 20-Jan-00   floh    + rewritten for ref_ss
     - 06-Mar-00   floh    + object_stack/fp_stack now templates
                           + owner object handling
     - 09-Nov-04   enlight + nObject support
*/
bool 
nPersistServer::EndFoldedObject(bool /*isObjNamed*/)
{
    n_assert(!this->objectStack.IsEmpty());

    // get object from stack
    nObject *obj = this->objectStack.Pop();
    n_assert(obj);

    // get owner
    
    if (!this->objectStack.IsEmpty()) 
    {
        nObject * owner = this->objectStack.Top();

        // write end object
        this->refSaver->WriteEndObject(this->file, obj, owner);
    }
    else
    {
        // end write to file if last object
        n_assert(this->file);

        // top object reached, close file
        this->refSaver->EndWrite(this->file);
        if ( this->releaseFile ) 
        {
            this->file->Close();
            this->file->Release();
            this->releaseFile = false;
        }
        this->file = 0;
    }

    return true;
}

//------------------------------------------------------------------------------
/** 
    Begin an object clone operation.
 
     - 18-Dec-98   floh    created
     - 09-Nov-04   enlight nObject support
*/
bool 
nPersistServer::BeginCloneObject(nObject *obj, nCmd * cmd, const char *name, bool isObjNamed)
{
    n_assert(obj);
    
    bool retval = false;
    const char *cl = obj->GetClass()->GetName();

    // active cwd must be got before calling cmd (overwritten by beginnewobject)
    nObject *actCwd = kernelServer->GetCwd();

    if (cmd)
    {
        cmd->Rewind();
        cmd->GetProto()->Dispatch(this->cloneTarget, cmd);
        cmd->Rewind();
        this->cloneTarget = static_cast<nObject *> (cmd->Out()->GetO());
    }
    else
    {
        if (isObjNamed)
        {
            this->cloneTarget = kernelServer->New(cl, name, false);
        }
        else
        {
            if ( ! obj->IsA("nentityobject") )
            {
                this->cloneTarget = nKernelServer::Instance()->New(cl, false);
            }
            else
            {
                // get entity object type for current object
                nEntityObject * eo = static_cast<nEntityObject *> (obj);
                nEntityObjectServer::nEntityObjectType otype = nEntityObjectServer::Instance()->GetEntityObjectType(eo->GetId());
                // case when the it is an entity object (a new local id will be assigned)
                this->cloneTarget = nEntityObjectServer::Instance()->NewEntityObjectFromType(cl, otype, 0, false);
            }
        }
    }

    if (this->cloneTarget) 
    {
        n_assert(this->cloneTarget->GetRefCount() == 1);
        if (this->objectStack.IsEmpty()) 
        {
            this->origCwd = actCwd;
        }

        // push current object cwd onto stack, make clone target  the cwd object
        this->objectStack.Push(actCwd);
        kernelServer->SetCwd(this->cloneTarget);
        retval = true;
    }

    return retval;
}

//------------------------------------------------------------------------------
/** 
    Finish an object clone operation.
  
     - 18-Dec-98   floh    created
     - 08-Nov-04   enlight + nObject support
*/
bool 
nPersistServer::EndCloneObject(bool /*isObjNamed*/)
{
    n_assert(!this->objectStack.IsEmpty())

    nObject *cwd = this->objectStack.Pop();
    if (cwd != this->origCwd) 
    {
        this->cloneTarget = cwd;
    }
    kernelServer->SetCwd(cwd);

    return true;
}

//------------------------------------------------------------------------------
/** 
    Get the pointer to the cloned object.

     - 18-Dec-98   floh    created
     - 08-Nov-04   enlight now returns nObject* instead of nRoot*
*/
nObject*
nPersistServer::GetClone(void)
{
    return this->cloneTarget;
}

//------------------------------------------------------------------------------
/** 
    Set the pointer to the cloned object.
*/
void
nPersistServer::SetClone(nObject * obj)
{
    this->cloneTarget = obj;
}

//------------------------------------------------------------------------------
/** 
*/
void
nPersistServer::BeginObjectLoad(nObject * obj, InitInstanceMsg initType)
{
    NLOG(persistence, (1, "BeginObjectLoad obj=%x init=%d class=%s %s", obj, initType, (obj? obj->GetClass()->GetName() : 0), this->file ? this->file->GetFilename().Get() : ""));
    kernelServer->PushCwd(obj);
    this->refSaver->SetCurrentTargetObject(obj);
    this->initInstanceMsgStack.Push(initType);
}

//------------------------------------------------------------------------------
/** 
*/
void
nPersistServer::EndObjectLoad(nObject * obj)
{
    nObject * endObject = kernelServer->PopCwd();
    InitInstanceMsg initType = this->initInstanceMsgStack.Pop();
    NLOG(persistence, (1, "EndObjectLoad obj=%x init=%d obj->class=%s endObject=%x endObject->class=%s file=%s getcwd=%x", obj, initType, obj->GetClass()->GetName(), endObject, endObject->GetClass()->GetName(), this->file ? this->file->GetFilename().Get() : "", kernelServer->GetCwd()));
    n_assert(endObject);
    n_assert(obj == endObject);
    if (this->saveMode == SAVEMODE_CLONE)
    {
        this->EndCloneObject(false);
    }
    this->refSaver->SetCurrentTargetObject(kernelServer->GetCwd());
    if (endObject && initType != nObject::NoInit)
    {
        if (this->saveMode == SAVEMODE_CLONE)
        {
            endObject->InitInstance(nObject::ClonedInstance);
        }
        else
        {
            endObject->InitInstance(initType);
        }
    }
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
