#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ngeometrystreameditorstate_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ngeometrystreameditorstate.h"

NSCRIPT_INITCMDS_BEGIN(nGeometryStreamEditorState)
    NSCRIPT_ADDCMD('NSTR', nRoot*, CreateStream, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('DSTR', void, DeleteStream, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('LLIB', void, LoadStreamLibrary, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLIB', void, SaveStreamLibrary, 0, (), 0, ());
    NSCRIPT_ADDCMD('BLDS', void, BuildAllStreams, 0, (), 0, ());
NSCRIPT_INITCMDS_END()
