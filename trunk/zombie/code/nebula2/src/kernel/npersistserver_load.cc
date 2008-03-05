//------------------------------------------------------------------------------
//  npersistserver_load.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/npersistserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nfile.h"
#include "kernel/ndependencyserver.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    Scan starting 256 bytes of a file for a "$keyword:string$" and return
    keyword belonging to string.
*/
char *
nPersistServer::ReadEmbeddedString(nFile * file, const char *keyword, char *buf, int buf_size)
{
    char header_buf[256];
    int num_bytes;

    n_assert(file->IsOpen());

    // reposition at the beginning of the file
    file->Seek(0, nFile::START);

    num_bytes = file->Read(header_buf, sizeof(header_buf));
    // fixed #bug 277: made a buffer to be terminated by null.
    int headerEnd = sizeof(header_buf) - 1;
    if (num_bytes < headerEnd)
    {
        headerEnd = num_bytes;
    }
    header_buf[headerEnd] = 0;
    
    if (num_bytes > 0) {

        // suche nach $
        char *str = header_buf;
        char *end_str = str + num_bytes;
        while (str < end_str) 
        {
            if (*str == '$') 
            {
                char *tmp;
                char *end = 0;
                *str++ = 0;

                // liegt das Ende im Buffer?
                if ( 0 != (end = strchr(str,'$')) ) 
                {
                    *end++ = 0;
                    // Trennzeichen ':'
                    if ( 0 != (tmp = strchr(str,':')) ) 
                    {
                        *tmp++ = 0;
                        // korrektes Keyword?
                        if (strcmp(str,keyword)==0) 
                        {
                            n_strncpy2(buf, tmp, buf_size);
                            return buf;
                        }
                    } 
                    else 
                    {
                        kernelServer->Message("$...$ definition in object file broken!"); 
                        return 0;
                    }
                    // Scan-Pointer auf Ende des $..$ Blocks
                    str = end;
                } 
                else
                {
                    kernelServer->Message("$...$ definition in object file outside first 256 bytes!");
                    return 0;
                }            
            } 
            else 
            {
                str++;
            }
        }
    }
    
    // $..$ Block not found, or File unreadable
    return 0;
}
//------------------------------------------------------------------------------
bool
nPersistServer::IsNewCmd(const char * objClass) const
{
    if (strchr(objClass,'('))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
bool
nPersistServer::ParseNewCmd(const char * objClass, nArray<nString> & newCmdTokens) const
{
    // new cmd and arguments are separated by commas
    if (this->IsNewCmd(objClass))
    {
        nString newCmd = objClass;
        newCmd.Tokenize(" (,)", newCmdTokens);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
nString 
nPersistServer::MakeNewCmdString(nCmd * cmd) const
{
    nString cmdStr;

    cmd->Rewind();

    cmdStr = cmd->GetProto()->GetName();
    cmdStr += "(";
    for(int i = 0;i < cmd->GetNumInArgs();i++)
    {
        if (i > 0)
        {
            cmdStr += ",";
        }
        nArg * arg = cmd->In();
        n_assert(arg->GetType() == nArg::String);
        cmdStr += arg->GetS();
    }
    cmdStr += ")";
    return cmdStr;
}

//------------------------------------------------------------------------------
/**
    Create a Nebula object from a persistent object file.

     - 10-Nov-98   floh    created
     - 30-Nov-98   floh    + wertet jetzt override_class aus
     - 10-Jan-99   floh    + Fehlermeldungen verbessert
     - 30-Jul-99   floh    + Override-Class gekillt
     - 20-Jan-00   floh    + rewritten for ref_ss
     - 28-Sep-00   floh    + PushCwd()/PopCwd()
*/
nObject*
nPersistServer::LoadFoldedObject(nFile * file, const char *objName,
                                 const char* parserClass, const char* objClass)
{
    nObject *obj = 0;
    
    n_assert(parserClass);
    n_assert(objClass);

    // store current target object
    nScriptServer* loader = this->GetLoader(parserClass);

    // create the object
    nArray<nString> newCmd;
    bool isNewCmd = this->ParseNewCmd(objClass, newCmd);
    if (isNewCmd)
    {
        // creation of new object by cmd (the string has the following format)
        // cmd arg1 arg2 ... argN

        // get the object on which execute the new cmd (current cwd)
        obj = nKernelServer::Instance()->GetCwd();
        n_assert(obj);

        // get the command prototype and create a new command
        nCmdProto * cmdproto = obj->GetClass()->FindCmdByName(newCmd[0].Get());
        if (!cmdproto)
        {
            n_message("file %s invalid command %s", file->GetFilename().Get(), newCmd[0].Get());
            return 0;
        }
        nCmd * cmd = cmdproto->NewCmd();
        n_assert(cmd);

        // fill the input arguments (the last argument will be the object name)
        for(int i = 0;i < cmdproto->GetNumInArgs();i++)
        {
            nArg * arg = cmd->In();
            if (arg->GetType() == nArg::String)
            {
                if ((i + 1) < newCmd.Size())
                {
                    arg->SetS(newCmd[i+1].Get());
                }
                else
                {
                    arg->SetS(objName);
                }
            }
            else
            {
                arg->SetS("");
            }
        }
        cmd->Rewind();

        // execute the new command
        if (cmdproto->Dispatch(obj, cmd))
        {
            cmd->Rewind();
            // get the result of the command (the object created)
            obj = static_cast<nObject *> (cmd->Out()->GetO());
        }

        // release the command
        cmdproto->RelCmd(cmd);
    }
    else
    {
        if (objName && objName[0])
        {
            obj = nKernelServer::Instance()->New(objClass, objName, false);
        }
        else
        {
            obj = nKernelServer::Instance()->New(objClass, false);
        }

        if (obj)
        {
            this->BeginObjectLoad(obj, NoInit);
        }
    }

    if (obj)
    {
        nString result;
        loader->RunScript(file, result);

        this->EndObjectLoad(obj);
    }

    return obj; 
}

//------------------------------------------------------------------------------
/**
    @brief Frontend to load an object from a persistent object file.
*/
nObject*
nPersistServer::LoadObject(nFile *file, const char* objName)
{
    n_assert(file);
    n_assert(file->IsOpen());
    n_assert(file->Tell() == 0 );
    char parserBuf[128];
    char objBuf[128];
    const char* parserClass;
    const char* objClass;

    // read parser and object class meta data from file
    parserClass = this->ReadEmbeddedString(file, "parser", parserBuf, sizeof(parserBuf));
    objClass = this->ReadEmbeddedString(file, "class", objBuf, sizeof(objBuf));
    if (!parserClass)
    {
        return 0;
    }
        
    nObject *obj = 0;
    nString tmpName;
        
    // if we need to create an nRoot make sure it will have a valid name
    if (!this->IsNewCmd(objClass) && 
        0 != nKernelServer::Instance()->FindClass(objClass) &&
        nKernelServer::Instance()->FindClass(objClass)->IsA("nroot"))
    {
        // isolate object name from path, object path can have 2 forms:
        //
        //  (1) xxx/blub.n/_main.n      -> a folded object
        //  (2) xxx/blub.n              -> an unfolded object
        //
        if (0 == objName)
        {
            nString path(file->GetFilename());
            path.ConvertBackslashes();
            tmpName = path.ExtractFileName();
            tmpName.StripExtension();
            objName = tmpName.Get();
        }
        n_assert(objName);

        // drop out if trying to load existing object
        obj = kernelServer->Lookup(objName);
        if (obj)
        {
            NLOG(persistence, (NLOGUSER | 0, "nPersistServer: trying to overwrite existing object '%s'!\n", 
                    ((nRoot *)obj)->GetFullName().Get()));
            return 0;
        }
    }
    
    // set the dependency server
    nDependencyServer::PushParentFileObject( 0, file->GetFilename()  );

    // try to load object as folded object, if that fails try unfolded
    obj = this->LoadFoldedObject(file, objName, parserClass, objClass);
#if 0
    // mateu.batle: unfolded object support disabled since change to avoid so many file reopen
    if (!obj)
    {
        char unfoldedName[N_MAXPATH];
        sprintf(unfoldedName, "%s/_main.n2", fileName);
        obj = this->LoadFoldedObject(unfoldedName, objName, parserClass, objClass);
        if (!obj) // couldn't load object!
        {
            n_message("nPersistServer: Could not load object '%s'!\n", file->GetFilename().Get() );
        }
    }
#endif

    // pop the parent info of dependency server
    nDependencyServer::PopParentFileObject( );
    return obj;
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
