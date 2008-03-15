//------------------------------------------------------------------------------
//  nobject_cmds.cc
//  (c) 2004 Vadim Macagon
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/ncmdproto.h"
#include "kernel/nobject.h"
#include "kernel/nref.h"
#include "kernel/ncmdprotonativecpp.h"

static void n_beginnewobject(void *o, nCmd *cmd);
static void n_endobject(void *o, nCmd *cmd);
static void n_this(void *o, nCmd *cmd);
static void n_saveas(void *, nCmd *);
static void n_clone(void *, nCmd *);
static void n_savestateas(void *, nCmd *);
static void n_loadstate(void *, nCmd *);
static void n_getrefcount(void *, nCmd *);
static void n_getclass(void *, nCmd *);
static void n_getclasses(void *, nCmd *);
static void n_isa(void *, nCmd *);
static void n_isinstanceof(void *, nCmd *);
static void n_getcmds(void *, nCmd *);
static void n_getinstancesize(void*, nCmd*);
static void n_setdependency(void*, nCmd*);
static void n_setdependencyobject(void*, nCmd*);
static void n_hascommand(void*, nCmd*);

#ifndef NGAME
static void n_ishideineditor(void*, nCmd*);
static void n_hideineditor(void*, nCmd*);
static void n_unhideineditor(void*, nCmd*);
static void n_isobjectdirty(void*, nCmd*);
static void n_setobjectdirty(void*, nCmd*);
#endif

//-------------------------------------------------------------------
/**
    @scriptclass
    nobject

    @cppclass
    nObject

    @superclass
    ---

    @classinfo
    nobject is the superclass of all higher level Nebula classes
    and defines this basic behaviour and properties for all
    nobject derived classes:
    - runtime type information
    - object persistency
    - language independent scripting interface
*/
NSCRIPT_INITCMDS_BEGIN(nObject)

    cl->AddCmd("o_beginnewobject_ss",   'BENO', n_beginnewobject);
    cl->AddCmd("v_endobject_v",         'ENDO', n_endobject);
    cl->AddCmd("o_this_v",              'THIS', n_this);
    cl->AddCmd("b_saveas_s",            'SVAS', n_saveas);
    cl->AddCmd("o_clone_s",             'CLON', n_clone);
    cl->AddCmd("b_savestateas_s",       'ESSA', n_savestateas);
    cl->AddCmd("b_loadstate_s",         'ELDS', n_loadstate);
    cl->AddCmd("i_getrefcount_v",       'GRCT', n_getrefcount);
    cl->AddCmd("s_getclass_v",          'GCLS', n_getclass);
    cl->AddCmd("l_getclasses_v",        'GCLL', n_getclasses);
    cl->AddCmd("b_isa_s",               'ISA_', n_isa);
    cl->AddCmd("b_isinstanceof_s",      'ISIO', n_isinstanceof);
    cl->AddCmd("l_getcmds_v",           'GMCD', n_getcmds);
    cl->AddCmd("i_getinstancesize_v",   'GISZ', n_getinstancesize);
    cl->AddCmd("v_setdependency_sss",   'SDCY', n_setdependency);
    cl->AddCmd("v_setdependencyobject_sss", 'SDCO', n_setdependencyobject);
    cl->AddCmd("b_hascommand_s", 'HCMD', n_hascommand);
#ifndef NGAME
    cl->AddCmd("b_ishideineditor_v", 'FHEE', n_ishideineditor);
    cl->AddCmd("v_hideineditor_v", 'FHIE', n_hideineditor);
    cl->AddCmd("v_unhideineditor_v", 'FUHE', n_unhideineditor);
    cl->AddCmd("b_isobjectdirty_v", 'CIDT', n_isobjectdirty);
    cl->AddCmd("v_setobjectdirty_b", 'CSDT', n_setobjectdirty);
#endif

    n_initcmds_nSignalEmitter(cl);

#ifndef NGAME
    cl->BeginSignals( 3 );
    N_INITCMDS_ADDSIGNAL(ObjectModified);
    N_INITCMDS_ADDSIGNAL(ObjectDirty);
    N_INITCMDS_ADDSIGNAL(ObjectChanges);
    cl->EndSignals();
#endif

NSCRIPT_INITCMDS_END()
//-------------------------------------------------------------------
/**
*/
static void n_beginnewobject(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    const char * className = cmd->In()->GetS();
    const char * objName = cmd->In()->GetS();
    nObject * obj = self->BeginNewObject( className, objName );
    cmd->Out()->SetO( obj );
}

//-------------------------------------------------------------------
/**
*/
static void n_endobject(void *o, nCmd * /*cmd*/)
{
    nObject *self = (nObject *) o;
    self->EndObject();
}

//-------------------------------------------------------------------
/**
*/
static void n_this(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;

    // report this to persist server
    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(self, nObject::NoInit);

    cmd->Out()->SetO( self );
}

//-------------------------------------------------------------------
/**
    @cmd
    saveas

    @input
    s (Name)

    @output
    b (Success)

    @info
    Save the object under a given name into a file.
*/
static void n_saveas(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetB(self->SaveAs(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    clone

    @input
    s (CloneName)

    @output
    o (CloneHandle)

    @info
    Creates a clone of this object.
    - If the object's class hierarchy doesn't contain nroot then
    'CloneName' is ignored. Otherwise 'CloneName' is the name given
    to the new cloned object.
    - If the original object has child objects, they will be cloned
    as well.
*/
static void n_clone(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetO(self->Clone(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    savestateas

    @input
    s (FileName)

    @output
    b (Success)

    @info
    Save object state for later restoring on an already created object.
*/
static void n_savestateas(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetB(self->SaveStateAs(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    loadstate

    @input
    s (FileName)

    @output
    b (Success)

    @info
    Load object state replacing the current state.
*/
static void n_loadstate(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetB(self->LoadState(cmd->In()->GetS()));
}

//-------------------------------------------------------------------
/**
    @cmd
    getrefcount

    @input
    v

    @output
    i (Refcount)

    @info
    Return current ref count of object.
*/
static void n_getrefcount(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetI(self->GetRefCount());
}

//-------------------------------------------------------------------
/**
    @cmd
    getclass

    @input
    v

    @output
    s (Classname)

    @info
    Return name of class which the object is an instance of.
*/
static void n_getclass(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetS(self->GetClass()->GetName());
}

//-------------------------------------------------------------------
/**
    @cmd
    getclasses

    @input
    v

    @output
    l (ClassnameList)

    @info
    Return the list of classes which the object is an instance of.
*/
static void n_getclasses(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    nClass* classObject;

    // count classes
    int numClasses = 0;
    for (classObject = self->GetClass();
         classObject;
         classObject = classObject->GetSuperClass())
    {
        numClasses++;
    }
    // Allocate
    nArg* args = n_new_array(nArg, numClasses);
    // And fill
    int i = 0;
    classObject = self->GetClass();
    do
    {
        args[i++].SetS(classObject->GetName());
    }
    while ( 0 != (classObject = classObject->GetSuperClass()) );
    cmd->Out()->SetL(args, numClasses);
}

//-------------------------------------------------------------------
/**
    @cmd
    isa

    @input
    s (Classname)

    @output
    b (Success)

    @info
    Check whether the object is instantiated or derived from the
    class given by 'Classname'.
*/
static void n_isa(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    const char *arg0 = cmd->In()->GetS();
    nClass *cl = nRoot::kernelServer->FindClass(arg0);
    if (cl)
    {
        cmd->Out()->SetB(self->IsA(cl));
    }
    else
    {
        cmd->Out()->SetB(false);
    }
}

//-------------------------------------------------------------------
/**
    @cmd
    isinstanceof

    @input
    s (Classname)

    @output
    b (Success)

    @info
    Check whether the object is an instance of the class given
    by 'Classname'.
*/
static void n_isinstanceof(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    const char *arg0 = cmd->In()->GetS();
    nClass *cl = nRoot::kernelServer->FindClass(arg0);
    if (cl)
    {
        cmd->Out()->SetB(self->IsInstanceOf(cl));
    }
    else
    {
        cmd->Out()->SetB(false);
    }
}

//-------------------------------------------------------------------
/**
    @cmd
    getcmds

    @input
    v

    @output
    l (Commands)

    @info
    Return a list of all script command prototypes the object accepts.
*/
static void n_getcmds(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    nHashList cmdList;
    nHashNode* node;
    int numCmds = 0;

    self->GetCmdProtos(&cmdList);
    // count commands
    for (node = cmdList.GetHead(); node; node = node->GetSucc())
    {
        numCmds++;
    }

    nArg* args = n_new_array(nArg, numCmds);
    int i = 0;
    while ( 0 != (node = cmdList.RemHead()) )
    {
        args[i++].SetS(((nCmdProto*) node->GetPtr())->GetProtoDef());
        n_delete(node);
    }
    cmd->Out()->SetL(args, numCmds);
}

//-------------------------------------------------------------------
/**
    @cmd
    getinstancesize

    @input
    v

    @output
    i (InstanceSize)

    @info
    Get byte size of this object. This may or may not accurate,
    depending on whether the object uses external allocated memory,
    and if the object's class takes this into account.
*/
static void n_getinstancesize(void* o, nCmd* cmd)
{
    nObject* self = (nObject*) o;
    cmd->Out()->SetI(self->GetInstanceSize());
}

//-------------------------------------------------------------------
/**
    @cmd
    setdependency

    @input
    s (filename)
    s (noh path)
    s (command)

    @output
    v

    @info
    set a dependency with a nRoot object saved in a persistent file
*/
static void n_setdependency(void* o, nCmd* cmd)
{
    nRoot * self = static_cast<nRoot*>( o );

    nString filename( cmd->In()->GetS() );
    nString noh( cmd->In()->GetS() );
    nString command( cmd->In()->GetS() );
    self->SetDependency( filename, noh, command );
}

//-------------------------------------------------------------------
/**
    @cmd
    setdependencyobject

    @input
    s (filename)
    s (noh path)
    s (command)

    @output
    v

    @info
    set a dependency with a nObject object saved in a persistent file
*/
static void n_setdependencyobject(void* o, nCmd* cmd)
{
    nRoot * self = static_cast<nRoot*>( o );

    nString filename( cmd->In()->GetS() );
    nString noh( cmd->In()->GetS() );
    nString command( cmd->In()->GetS() );
    self->SetDependencyObject( filename, noh, command );
}
//-------------------------------------------------------------------
/**
@cmd
hasscommand

@input
s (Classname)

@output
b (Success)

@info
Check if object has a specific command.
*/
static void n_hascommand(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    const char *arg0 = cmd->In()->GetS();
    cmd->Out()->SetB(self->HasCommand(arg0));
}
//-------------------------------------------------------------------
/**
@cmd
ishideineditor

@input
v

@output
b (Is hidden)

@info
Check if object must be dipslayed in editor tools.
*/
#ifndef NGAME
static void n_ishideineditor(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetB(self->IsHideInEditor());
}
#endif
//-------------------------------------------------------------------
/**
@cmd
hideineditor

@input
v

@output
v

@info
Make this object displayable in editor
*/
#ifndef NGAME
static void n_hideineditor(void *o, nCmd * /*cmd*/)
{
    nObject *self = (nObject *) o;
    self->HideInEditor();
}
#endif
//-------------------------------------------------------------------
/**
@cmd
unhideineditor

@input
v

@output
v

@info
Make this object not displayable in editor
*/
#ifndef NGAME
static void n_unhideineditor(void *o, nCmd * /*cmd*/)
{
    nObject *self = (nObject *) o;
    self->UnHideInEditor();
}
#endif

//-------------------------------------------------------------------
/**
    @cmd
    isobjectdirty

    @input
    v

    @output
    b(objectdirty)

    @info
    if this object is dirty
*/
#ifndef NGAME
static void n_isobjectdirty(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    cmd->Out()->SetB(self->IsObjectDirty() );
}
#endif

//-------------------------------------------------------------------
/**
    @cmd
    setobjectdirty

    @input
    b (objectdirty)

    @output
    v

    @info
    Set object is dirty
*/
#ifndef NGAME
static void  n_setobjectdirty(void *o, nCmd *cmd)
{
    nObject *self = (nObject *) o;
    self->SetObjectDirty( cmd->In()->GetB() ) ;
}
#endif

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
