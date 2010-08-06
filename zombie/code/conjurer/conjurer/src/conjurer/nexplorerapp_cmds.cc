#include "precompiled/pchexplorer.h"
//------------------------------------------------------------------------------
//  nexplorerapp_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nexplorerapp.h"

static void n_getreleasebuildinfo(void *, nCmd *);
static void n_getreleasesubversioninfo(void *, nCmd *);
//------------------------------------------------------------------------------
/**
    @scriptclass
    nExplorerApp

    @cppclass
    nExplorerApp

    @superclass
    ncommonapp

    @classinfo
    The main conjurer editor application.
*/
NSCRIPT_INITCMDS_BEGIN(nExplorerApp)
    NSCRIPT_ADDCMD('SCVP', void, SetCurrentViewport, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GCVP', const char *, GetCurrentViewport, 0, (), 0, ());
    //NSCRIPT_ADDCMD('MVTE', void, MoveCameraToEntity, 1, (nEntityObjectId), 0, ());
    //NSCRIPT_ADDCMD('JMCS', void, MoveCameraToSelection , 0, (), 0, ());
    NSCRIPT_ADDCMD('JABM', void, AddBookmark, 2, (const vector3&, const polar2&), 0, ());
    NSCRIPT_ADDCMD('JSBM', void, SetBookmark, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGNB', int, GetNumBookmarks, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSVB', void, SaveBookmarks, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('MLDB', void, LoadBookmarks, 1, (const char *), 0, ());
    //NSCRIPT_ADDCMD('MPRV', void, OpenPreview, 1, (const char *), 0, ());
    //NSCRIPT_ADDCMD('MCPR', void, ClosePreview, 0, (), 0, ());
    NSCRIPT_ADDCMD('SWCD', void, SetWorkingCopyDir, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GWCD', const char *, GetWorkingCopyDir, 0, (), 0, ());
    NSCRIPT_ADDCMD('STWC', void, SetTempWorkingCopyDir, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GTWC', const char *, GetTempWorkingCopyDir, 0, (), 0, ());
    NSCRIPT_ADDCMD('STMP', void, SetTemporaryModeEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GTMP', bool, GetTemporaryModeEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESWT', void, SetWindowTitle, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('BCTS', nString, CalcTitleString, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('NLEV', void, NewLevel, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('LLEV', void, LoadLevel, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('DLEV', void, DeleteLevel, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('SLEV', void, SaveLevel, 0, (), 0, ());
    NSCRIPT_ADDCMD('YSLA', bool, SaveCurrentLevelAs, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('BSES', void, SaveEditorState, 0, (), 0, ());
    NSCRIPT_ADDCMD('BRES', void, RestoreEditorState, 0, (), 0, ());
NSCRIPT_INITCMDS_END()
