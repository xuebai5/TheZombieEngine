#include "precompiled/pchconjurerapp.h"
//-----------------------------------------------------------------------------
//  nobjinspector_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "conjurer/nobjinspector.h"

//-----------------------------------------------------------------------------
/**
    @scriptclass
    nobjinspector

    @cppclass
    nObjInspector

    @superclass
    nscriptserver

    @classinfo
    The nObjInspector provide an interface with the scripting language for
    get information about the state of the object and all the class commands.
    It have two work modes, one of them works over the state of the command,
    the other returns information about the commands related to the class
    passed as a parameter to the function InitCmdsClass.

    You can know the type of the command, the number of arguments, and her
    actual value in case of state commands, all this information is very
    usefull for build dynamic guis.
*/
NSCRIPT_INITCMDS_BEGIN(nObjInspector)
    NSCRIPT_ADDCMD('OIII', void, InitCmdsState, 1, (nObject *), 0, ());
    NSCRIPT_ADDCMD('CIII', void, InitCmdsClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('SIII', void, InitSignalsClass, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GCCC', int, GetCmdsCount, 0, (), 0, ());
    NSCRIPT_ADDCMD('GIAC', int, GetInArgsCount, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GOAC', int, GetOutArgsCount, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GCPN', const char *, GetCmdProtoName, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GCNN', const char *, GetCmdName, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GTTT', int, GetType, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GIII', int, GetI, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GFFF', float, GetF, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GSSS', const char *, GetS, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GBBB', bool, GetB, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GOOO', void *, GetO, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GWMM', int, GetState, 0, (), 0, ());
    NSCRIPT_ADDCMD('GPNC', const char *, GetClassProperName, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()