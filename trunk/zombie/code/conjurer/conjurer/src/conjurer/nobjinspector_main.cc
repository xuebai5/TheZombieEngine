#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nobjinspector_main.cc 
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/nobjinspector.h"
#include "kernel/nkernelserver.h"
#include "conjurer/nstatelist.h"
#include "signals/nsignal.h"
#include "file/nmemfile.h"


nNebulaScriptClass(nObjInspector, "nroot");

int nObjInspector::uniqueId = 0;

nObjInspector::nObjInspector()
{
    //empty
}

nObjInspector::~nObjInspector()
{
    if (this->mode == CLASS)
    {
        // Delete class cmds
        for (int i=0; i < this->classList.Size(); i++)
        {
            this->classList[i]->GetProto()->RelCmd(this->classList[i]);
        }

        this->classList.Clear();
    }
    else if (this->mode == STATE)
    {
        // Delete state cmds
        for (int i=0; i < this->stateList.Size(); i++)
        {
            this->stateList[i]->GetProto()->RelCmd(this->stateList[i]);        
        }
                 
        this->stateList.Clear();

    }
    else if (this->mode == SIGNALS)
    {
        // Delete class cmds
        for (int i=0; i < this->signalList.Size(); i++)
        {
            this->signalList[i]->GetProto()->RelCmd(this->signalList[i]);
        }

        this->signalList.Clear();
    }
}

//------------------------------------------------------------------------------
/** @brief
    The method for get the state is very dirty...
    I override a method used for persist an object (Writecmd)
    This way when I try to save the object instead of persist the object,
    It create a new object of this class and fill the array
    StateList, the I fill my stateList array getting it from the recently
    Created object.
    Once time I have the array filled, I can delete the temporary persist server 
    and the temporary object
*/
void
nObjInspector::InitCmdsState(nObject* obj)
{
    n_assert(obj)
    nStateList* stateInspector;
    nMemFile* fakeFile = n_new(nMemFile);
    nString emptyFilename;
    n_verify(fakeFile->Open( emptyFilename, "r" ));

    // Get the persist server path, multiple name for multiple instances
    nString path("/editor/inspector/inspector");
    path.AppendInt(nObjInspector::uniqueId++);

    // Create the temporary persist server
    this->ps = (nPersistServer*) kernelServer->New("npersistserver",path.Get());

    // Select this class as saver for get the state commands
    this->ps->SetSaverClass("nstatelist");
    //this->ps->SetPersistFile(fakeFile);

    // Get the state, persist server will use the overloaded WriteCmd function
    if (this->ps->BeginObject(this, fakeFile, true)) 
    {
        obj->SaveCmds(this->ps);
        this->ps->EndObject(true);
    }
    path.Append("/nstatelist");

    // Get the state list created by the inspector
    stateInspector = (nStateList *) kernelServer->Lookup(path.Get());
    this->stateList = stateInspector->GetStateList(); 
    this->ps->Release();
    fakeFile->Release();

    this->mode = STATE;      
}

void
nObjInspector::InitCmdsClass(const char* className)
{
    nClass* cl = kernelServer->FindClass(className);
    if (cl)
    {
        nHashList* cmdList = cl->GetCmdList();

        if (cmdList != 0)
        {        
            nCmdProto* cmdProto = static_cast<nCmdProto*>( cmdList->GetHead() );
            while (cmdProto != 0)
            {
                this->classList.Append(cmdProto->NewCmd());
                cmdProto = static_cast<nCmdProto*>(cmdProto->GetSucc());
            }
        }        

        this->mode = CLASS;
    }
    else
    {
        n_assert2_always("Class not found");
    }        
}

void
nObjInspector::InitSignalsClass(const char* className)
{
    nClass* cl = kernelServer->FindClass(className);
    if (cl)
    {
        nHashList* signalList = cl->GetSignalList();

        if (signalList)
        {        
            nSignal * signal;
            for (signal=(nSignal*) signalList->GetHead(); 
                signal; 
                signal=(nSignal*) signal->GetSucc()) 
            {
                nHashNode* node = n_new(nHashNode(signal->GetName()));
                node->SetPtr((void*)signal);
                this->signalList.Append(signal->NewCmd());
                n_delete(node);
            }
                
        }        

        this->mode = SIGNALS;
    }
    else
    {
        n_assert2_always("Class not found");
    }        
}

int
nObjInspector::GetCmdsCount()
{
    if (this->mode == STATE)
    {
        return stateList.Size();
    }
    else if (this->mode == CLASS)
    {
        return classList.Size();
    }
    else
    {
        return signalList.Size();
    }
}

int
nObjInspector::GetInArgsCount(int numCmd)
{
    
    nArray<nCmd*> list = SelectCommandList(this->mode);

    return list[numCmd]->GetNumInArgs();
}

int
nObjInspector::GetOutArgsCount(int numCmd)
{
    
    nArray<nCmd*> list = SelectCommandList(this->mode);

    return list[numCmd]->GetNumOutArgs();
}

const char*
nObjInspector::GetCmdName(int numCmd)
{
    
    nArray<nCmd*> list = SelectCommandList(this->mode);

    return list[numCmd]->GetProto()->GetName();
}

const char*
nObjInspector::GetCmdProtoName(int numCmd)
{
    
    nArray<nCmd*> list = SelectCommandList(this->mode);

    return list[numCmd]->GetProto()->GetProtoDef();
}

const char*
nObjInspector::GetClassProperName(const char* className)
{
    nClass* cl = kernelServer->FindClass(className);
    if (cl)
    {
        return cl->GetProperName();  
    }
    else
    {
        return "";
    }        
}

int
nObjInspector::GetType(int numCmd, int numArg)
{
    
    nArray<nCmd*> list = SelectCommandList(this->mode);

    nArg* arg = 0;
    int type = 0;
    
    if (list.Size() < numCmd)
    {
        n_assert2_always("Index command not valid");
        return 0;
    }
    else if ( list[numCmd]->GetNumInArgs() < numArg)
    {
        n_assert2_always("Index arg not valid");
        return 0;
    }

    list[numCmd]->Rewind();

    // Get arg
    for (int i=0; i < numArg; i++) 
    {
        arg = list[numCmd]->In();
    }

    if (arg)
    {
        switch(arg->GetType())
        {
        case nArg::Int:
            type = INTEGER;
            break;
        case nArg::Float:
            type = FLOAT;
            break;
        case nArg::String:
            type = STRING;
            break;
        case nArg::Bool:
            type = BOOLEAN;
            break;
        case nArg::Object:
            type = OBJECT;
            break;
        case nArg::List:
            type = LIST;
            break;
        case nArg::Float4:
            type = FLOAT4;
            break;
        case nArg::Matrix44:
            type = MATRIX44;
            break;
        default:
            n_assert2_always("Argument type not found");
            type = INTEGER; // Allow program to continue
        }
    }
    
    return type;
}

int
nObjInspector::GetI(int numCmd, int numArg)
{    
    if (this->mode == STATE)
    {
        nArg* arg=0;

        // Get arg
        this->stateList[numCmd]->Rewind();
        for (int i=0; i < numArg; i++) 
        {
            arg = this->stateList[numCmd]->In();
        }

        if (arg)
        {
            if (arg->GetType() == nArg::Int)
            {
                return arg->GetI();
            }
            else
            {
                n_assert2_always("Bad use of GetI, if continue 0 will be returned");
                return 0;    
            }
        }
        else
        {
            n_error("Not argument in command");
            return 0;
        }        
    }    
    else
    {
        return 0;
    }
}

float
nObjInspector::GetF(int numCmd, int numArg)
{   
    if (this->mode == STATE)
    {
        nArg* arg = 0;

        // Get arg
        this->stateList[numCmd]->Rewind();
        for (int i=0; i < numArg; i++) 
        {
            arg = this->stateList[numCmd]->In();
        }

        if (arg)
        {
            if (arg->GetType() == nArg::Float)
            {
                return arg->GetF();
            }
            else
            {
                n_assert2_always("Bad use of GetF, if continue 0 will be returned");
                return 0.0f;    
            }
        }
        else
        {
            n_error("Not argument in command");
            return 0.0f;
        }

        
    }
    else 
    {
        return 0.0f;
    }    
}

const char *
nObjInspector::GetS(int numCmd, int numArg)
{    
    
    if (this->mode == STATE)
    {
        nArg* arg = 0;        

        // Get arg
        this->stateList[numCmd]->Rewind();
        for (int i=0; i < numArg; i++) 
        {
            arg = this->stateList[numCmd]->In();
        }

        if (arg)
        {
            if (arg->GetType() == nArg::String)
            {
                return arg->GetS();
            }
            else
            {
                n_assert2_always("Bad use of GetS, if continue \"\" will be returned");
                return "";    
            }
        }
        else
        {
            n_error("Not argument in command");
            return "";
        }        
    }
    else
    {
        return "";
    }
    
}

bool
nObjInspector::GetB(int numCmd, int numArg)
{    
    if (this->mode == STATE)
    {
        nArg* arg = 0;

        // Get arg
        this->stateList[numCmd]->Rewind();
        for (int i=0; i < numArg; i++) 
        {
            arg = this->stateList[numCmd]->In();
        }

        if (arg)
        {
            if (arg->GetType() == nArg::Bool)
            {
                return arg->GetB();
            }
            else
            {
                n_assert2_always("Bad use of GetB, if continue false will be returned");
                return false;    
            }
        }
        else
        {
            n_error("Not argument in command");
            return false;
        }        
    }    
    else 
    {
        return false;
    }
}

void*
nObjInspector::GetO(int numCmd, int numArg)
{    
    if (this->mode == STATE)
    {
        nArg* arg = 0;    

        // Get arg
        this->stateList[numCmd]->Rewind();
        for (int i=0; i < numArg; i++) 
        {
            arg = this->stateList[numCmd]->In();
        }

        if (arg)
        {
            if (arg->GetType() == nArg::Object)
            {
                return arg->GetO();
            }
            else
            {        
                n_assert2_always("Bad use of GetO, if continue void will be returned");
                void* obj = 0;
                return (obj);
            }
        }
        else
        {
            n_error("Not argument in command");
            void* obj = 0;
            return obj;
        }
        
    }
    else
    {
        void* obj = 0;
        return obj;
    }
    
}