//-----------------------------------------------------------------------------
//  ntriggerserver_cmds.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ntriggerserver.h"
#include "ntrigger/nscriptoperation.h"
#include "ntrigger/ntriggerstatecondition.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nTriggerServer )
    NSCRIPT_ADDCMD('ERTG', void, RegisterTrigger, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('EDTG', void, RemoveTrigger, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('ESUI', void, SetUpdateInterval, 1, (nGameEvent::Time), 0, ());
    NSCRIPT_ADDCMD('EGUI', nGameEvent::Time, GetUpdateInterval, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESPC', void, SetPerceivableCellsUpdateInterval, 1, (nGameEvent::Time), 0, ());
    NSCRIPT_ADDCMD('EGPC', nGameEvent::Time, GetPerceivableCellsUpdateInterval, 0, (), 0, ());
    NSCRIPT_ADDCMD('EPAE', nGameEvent::Id, PostAreaEvent, 5, (const char*, nEntityObject*, nEntityObject*, nGameEvent::Time, int), 0, ());
    NSCRIPT_ADDCMD('EPLE', nEntityObject*, PlaceAreaEvent, 5, (const vector3&, const char*, nEntityObject*, nGameEvent::Time, int), 0, ());
    NSCRIPT_ADDCMD('EDEV', void, DeleteEvent, 1, (nGameEvent::Id), 0, ());
    NSCRIPT_ADDCMD('EGSO', nScriptOperation*, GetScriptOperation, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EGTC', nTriggerStateCondition*, GetTriggerCondition, 1, (const char*), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('EGTN', int, GetEventTypesNumber , 0, (), 0, ());
    NSCRIPT_ADDCMD('EFIE', bool, IsAFsmInEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EAIE', bool, IsAnAreaTriggerInEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EAOE', bool, IsAnAreaTriggerOutEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EGIE', bool, IsAGenericTriggerInEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EGPI', const char*, GetEventPersistentId, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EGTI', int, GetEventTransientId, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EGEL', const char*, GetEventLabel, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EGET', int, GetEventType, 1, (nGameEvent::Id), 0, ());
    NSCRIPT_ADDCMD('EGES', nEntityObjectId, GetEventSource, 1, (nGameEvent::Id), 0, ());
    NSCRIPT_ADDCMD('EGED', nGameEvent::Time, GetEventDuration, 1, (nGameEvent::Id), 0, ());
    NSCRIPT_ADDCMD('EGEP', int, GetEventPriority, 1, (nGameEvent::Id), 0, ());
#endif
    // @todo Move the following commands to somewhere else (to a future motion server?)
    NSCRIPT_ADDCMD('ESAC', void, SetAvoidanceCellsUpdateInterval, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EGAC', int, GetAvoidanceCellsUpdateInterval, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
