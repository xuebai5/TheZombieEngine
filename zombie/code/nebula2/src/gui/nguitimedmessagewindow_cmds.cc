#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguilinelayout_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gui/nguitimedmessagewindow.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nGuiTimedMessageWindow )

    NSCRIPT_ADDCMD('JSTL', void, SetTemplateTextLabel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JGTL', const char *, GetTemplateTextLabel, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSFT', void, SetMessageFadeoutTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD('JGFT', float, GetMessageFadeoutTime, 0, (), 0, ());
    NSCRIPT_ADDCMD('JADM', void, AddMessage, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JSLM', bool, SetLastMessage, 4, (int, const char *, const char *, const char *), 0, ());
    NSCRIPT_ADDCMD('JGLM', const char *, GetLastMessage, 0, (), 0, ());
    NSCRIPT_ADDCMD('JCLR', void, ClearWindow, 0, (), 0, ());

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
