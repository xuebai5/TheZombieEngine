//------------------------------------------------------------------------------
//  nkernelserverproxy_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"

#include "kernel/nkernelserverproxy.h"
#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
static void n_getclasslist( void *, nCmd *);
static void n_getsubclasslist( void *, nCmd *);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN( nKernelServerProxy )

    NSCRIPT_ADDCMD('FPLO', void, Lock, 0, (), 0, ());
    NSCRIPT_ADDCMD('FPUL', void, Unlock, 0, (), 0, ());
    NSCRIPT_ADDCMD('FPRC', void, RemClass, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FPCC', const char *, CreateClass, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FRCC', void, ReleaseClass, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FPFC', const char *, FindClass, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('CICL', bool, IsClassLoaded, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FPNR', nRoot *, New, 2, (const char* , const char*), 0, ());
    NSCRIPT_ADDCMD('FPGL', nObject*, NewObject, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FNNF', nRoot *, NewNoFail, 2, (const nString & , const nString &), 0, ());
    NSCRIPT_ADDCMD('FNOF', nObject*, NewObjectNoFail, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FPLL', nObject *, Load, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FPLA', nRoot *, LoadAs, 2, (const nString &, const nString &), 0, ());
    NSCRIPT_ADDCMD('SPLU', nRoot *, Lookup, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FPSC', void, SetCwd, 1, (nObject*), 0, ());
    NSCRIPT_ADDCMD('FPGC', nObject*, GetCwd, 0, (), 0, ());
    NSCRIPT_ADDCMD('FPPC', void, PushCwd, 1, (nObject*), 0, ());
    NSCRIPT_ADDCMD('FPCP', nObject*, PopCwd, 0, (), 0, ());
    /*NSCRIPT_ADDCMD('FPSL', void, SetLogHandler, 1, (nLogHandler*), 0, ());
    NSCRIPT_ADDCMD('FPGL', nRoot*, GetLogHandler, 0, (), 0, ());*/
    NSCRIPT_ADDCMD('FPRF', void, ReplaceFileServer, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('FGPS', nRoot*, GetPersistServer, 0, (), 0, ());
    NSCRIPT_ADDCMD('FGFS', nRoot*, GetFileServer, 0, (), 0, ());
    NSCRIPT_ADDCMD('FGRS', nRoot*, GetRemoteServer, 0, (), 0, ());
    NSCRIPT_ADDCMD('FGTS', nRoot*, GetTimeServer, 0, (), 0, ());
    NSCRIPT_ADDCMD('FTRG', void, Trigger, 0, (), 0, ());
    NSCRIPT_ADDCMD('EICN', bool, IsClassNative, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('EHCC', bool, HasClassCommand, 2, (const char *, const char*), 0, ());
    NSCRIPT_ADDCMD('EGSC', const char*, GetSuperClass, 1, (const char *), 0, ());

    // no macro support
    cl->AddCmd( "l_getclasslist_v", 'FGCL', n_getclasslist );
    cl->AddCmd( "l_getsubclasslist_s", 'EGSL', n_getsubclasslist );

#ifndef NGAME
    cl->BeginSignals( 2 );
    N_INITCMDS_ADDSIGNAL( ObjectDeleted );
    N_INITCMDS_ADDSIGNAL( ClassDeleted );
    cl->EndSignals();
#endif

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
static void n_getclasslist(void * /*o*/, nCmd *cmd)
{
    const nHashList * classlist = nKernelServer::Instance()->GetClassList();
    n_assert( classlist );

    // count classes
    int numClasses = 0;
    nHashNode * node = classlist->GetHead();
    while( node )
    {
        ++numClasses;
        node = node->GetSucc();
    }

    // Allocate
    nArg* args = n_new_array(nArg, numClasses);

    // And fill
    int i = 0;
    node = classlist->GetHead();
    while( node )
    {
        args[i++].SetS( node->GetName() );
        node = node->GetSucc();
    }
    cmd->Out()->SetL(args, numClasses);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getsubclasslist

    @input
    s(superclass)

    @output
    l(subclasses)

    @info
    Get a list with all the classes that inherit from the given class.
    Only one level of inheritance is searched, so a class is returned only if
    its immediate superclass is the given class.
*/
static void n_getsubclasslist(void * /*o*/, nCmd *cmd)
{
    // get the superclass
    const nClass* superclass = nKernelServer::Instance()->FindClass( cmd->In()->GetS() );

    // get the list with all the classes
    const nHashList * classlist = nKernelServer::Instance()->GetClassList();
    n_assert( classlist );

    // store a reference for each one of the classes that inherit from the given superclass
    nArray<nClass*> subclasslist;
    for ( nHashNode * node = classlist->GetHead(); node; node = node->GetSucc() )
    {
        if ( static_cast<nClass*>(node)->GetSuperClass() == superclass )
        {
            subclasslist.Append( static_cast<nClass*>(node) );
        }
    }

    // create subclass list for output argument
    nArg* args = n_new_array( nArg, subclasslist.Size() );
    for ( int i(0); i < subclasslist.Size(); ++i )
    {
        args[i].SetS( subclasslist[i]->GetName() );
    }
    cmd->Out()->SetL( args, subclasslist.Size() );
}

//------------------------------------------------------------------------------
