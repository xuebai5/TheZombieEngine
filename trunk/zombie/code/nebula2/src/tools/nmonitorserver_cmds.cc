//------------------------------------------------------------------------------
/**
    @file nmonitorserver_cmds.cc
    @author Juan Jose Luna

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchntoollib.h"
#include "tools/nmonitorserver.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nMonitorServer)
    NSCRIPT_ADDCMD('JADG', int, AddGraph, 4, (const char *, float, float, vector4), 0, ());
    NSCRIPT_ADDCMD('JRMG', void, RemoveGraph, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGNG', int, GetNumGraphs, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSVP', void, SetVisualParameters, 3, (vector2, vector2,vector4), 0, ());
    NSCRIPT_ADDCMD('JSHP', void, SetHistoryPeriod, 1, (float), 0, ());
    NSCRIPT_ADDCMD('JGHP', float, GetHistoryPeriod, 0, (), 0, ());
    NSCRIPT_ADDCMD('JGDA', bool, GetDisplayActivated, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSDA', void, SetDisplayActivated, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('JGSA', bool, GetSamplingActivated, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSSA', void, SetSamplingActivated, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('JGFS', bool, GetFullScreen, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSFS', void, SetFullScreen, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('JGVN', const char *, GetGraphVarName, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JSGP', void, SetGraphParameters, 4, (int, float, float, vector4), 0, ());
    NSCRIPT_ADDCMD('JGGP', void, GetGraphParameters, 1, (int), 3, (float&, float&, vector4&));
    NSCRIPT_ADDCMD('JGGA', bool, GetGraphActive, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JSGA', void, SetGraphActive, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JGMI', float, GetGraphMinValue, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGMA', float, GetGraphMaxValue, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGME', float, GetGraphMeanValue, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JSCG', void, SetCurrentGraph, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGCG', int, GetCurrentGraph, 0, (), 0, ());
    NSCRIPT_ADDCMD('JGGS', void, GetCurrentGraphStats, 0, (), 4, (float&, float&, float&, float&));
    NSCRIPT_ADDCMD('JRAH', void, ResetAllHistory, 0, (), 0, ());
NSCRIPT_INITCMDS_END()
