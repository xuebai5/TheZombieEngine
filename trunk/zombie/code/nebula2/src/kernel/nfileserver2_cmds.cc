//------------------------------------------------------------------------------
//  (C) 2001 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nfileserver2.h"
#include "kernel/npersistserver.h"

static void n_setassign(void* slf, nCmd* cmd);
static void n_setassigngroup2(void* slf, nCmd* cmd);
static void n_getassign(void* slf, nCmd* cmd);
static void n_getassigngroup(void* slf, nCmd* cmd);
static void n_getallmanglepath(void* slf, nCmd* cmd);
static void n_manglepath(void* slf, nCmd* cmd);
static void n_manglepath2(void* slf, nCmd* cmd);
static void n_fileexists(void* slf, nCmd* cmd);
static void n_directoryexists(void* slf, nCmd* cmd);
static void n_createfilenode(void* slf, nCmd* cmd);
static void n_makepath(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nfileserver2

    @cppclass
    nFileServer2
    
    @superclass
    nroot

    @classinfo
    New style fileserver.
*/
void
n_initcmds_nFileServer2(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_setassign_ss",        'SASS', n_setassign);
    cl->AddCmd("v_setassigngroup2_sss", 'SAS2', n_setassigngroup2);
    cl->AddCmd("s_getassign_s",         'GASS', n_getassign);
    cl->AddCmd("l_getassigngroup_s",    'GASG', n_getassigngroup);
    cl->AddCmd("l_getallmanglepath_s",  'GAMP', n_getallmanglepath);
    cl->AddCmd("s_manglepath_s",        'MNGP', n_manglepath);
    cl->AddCmd("s_manglepath2_sb",      'MNG2', n_manglepath2);
    cl->AddCmd("b_fileexists_s",        'FLEX', n_fileexists);
    cl->AddCmd("b_directoryexists_s",   'DREX', n_directoryexists);
    cl->AddCmd("b_makepath_s",          'CMKP', n_makepath);
    cl->AddCmd("o_createfilenode_s",    'CFLN', n_createfilenode);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    setassign

    @input
    s(AssignName), s(Path)

    @output
    v

    @info
    Defines an assign with the specified name and links it to the specified
    path.
*/
static void
n_setassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    const char* s0 = cmd->In()->GetS();
    const char* s1 = cmd->In()->GetS();
    self->SetAssign(s0, nString(s1));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setassigngroup2

    @input
    s(AssignName), s(path1), s(path2)

    @output
    v

    @info
    Defines an assign with the specified name and links it to the specified
    path group.
*/
static void
n_setassigngroup2(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    const char* s0 = cmd->In()->GetS();
    nArray<nString> group;
    /*
    nArg * args;
    int numargs = cmd->In()->GetL(args);
    for(int i = 0;i < numargs;i++)
    {
        if (args[i].GetType() == nArg::String)
        {
            group.Append(args[i].GetS());
        }
    }
    */
    group.Append(nString(cmd->In()->GetS()));
    group.Append(nString(cmd->In()->GetS()));
    self->SetAssignGroup(s0, group);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getassign

    @input
    s(AssignName)

    @output
    s(Path)

    @info
    Get a path associated with an assign.
*/
static void
n_getassign(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetS(self->GetAssign(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getassigngroup

    @input
    s(AssignName)

    @output
    l(PathList)

    @info
    Get a path list associated with an assign group.
*/
static void
n_getassigngroup(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;

    nArray<nString> paths;
    self->GetAssignGroup(cmd->In()->GetS(), paths);

    // create a nArg list (allocate here and let nArg manage from now on)
    nArg * args = n_new(nArg)[paths.Size()];
    for(int i = 0;i < paths.Size();i++)
    {
        // make sure trailing slash exists
        nString pathString = paths[i];
        pathString.StripTrailingSlash();
        pathString.Append("/");
        args[i].SetS(pathString.Get());
    }

    cmd->Out()->SetL(args, paths.Size());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getallmanglepath

    @input
    s(AssignName)

    @output
    l(PathList)

    @info
    Get a all manglepath path list associated with an assign group.
*/
static void n_getallmanglepath(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;

    nArray<nString> paths;
    self->GetAllManglePath(cmd->In()->GetS(), paths);

    // create a nArg list (allocate here and let nArg manage from now on)
    nArg * args = n_new(nArg)[paths.Size()];
    for(int i = 0;i < paths.Size();i++)
    {
        args[i].SetS(paths[i].Get());
    }

    cmd->Out()->SetL(args, paths.Size());
}

//------------------------------------------------------------------------------
/**
    @cmd
    manglepath

    @input
    s(UnmangledPath)

    @output
    s(MangledPath)

    @info
    Convert a path with assigns into a native absolute path.
*/
static void
n_manglepath(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    nString str = self->ManglePath( nString( cmd->In()->GetS() ) );
    cmd->Out()->SetS(str.Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    manglepath

    @input
    s(UnmangledPath)
    b(read)

    @output
    s(MangledPath)

    @info
    Convert a path with assigns into a native absolute path. Use read = true
    for read path resolution, and read = false for write path resolution.
*/
static void
n_manglepath2(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    const char * assignName = cmd->In()->GetS();
    bool read = cmd->In()->GetB();
    nString str = self->ManglePath(assignName, read);
    cmd->Out()->SetS(str.Get());
}

//------------------------------------------------------------------------------
/**
    @cmd
    fileexists

    @input
    s(PathName)

    @output
    b(Exists)

    @info
    Return true if file exists. Note: does not work for directories!
*/
static void
n_fileexists(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetB(self->FileExists(cmd->In()->GetS()));
}


//------------------------------------------------------------------------------
/**
    @cmd
    makepath

    @input
    s(PathName)

    @output
    b(Creater)

    @info
    Create a path. Return true if directory is created. 
*/
static void
n_makepath(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetB(self->MakePath(cmd->In()->GetS()));
}


//------------------------------------------------------------------------------
/**
*/
static void
n_directoryexists(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetB(self->DirectoryExists(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    createfilenode

    @input
    s(UniqueName)

    @output
    o(Filenode)

    @info
    Create a Nebula2 filenode and return it. A filenode
    wraps a nFile object into a nRoot node, and offers a script interface.
    Use a nfilenode object for file access from within MicroTcl, or other
    script functions which don't have their own file access functions.
*/
static void
n_createfilenode(void* slf, nCmd* cmd)
{
    nFileServer2* self = (nFileServer2*) slf;
    cmd->Out()->SetO(self->CreateFileNode(cmd->In()->GetS()));
}

