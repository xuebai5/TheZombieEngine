#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file nobjecteditorstate_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Mateu Batle Sastre
    @author Juan Jose Luna

    @brief nObjectEditorState persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/nobjecteditorstate.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nObjectEditorState)

    NSCRIPT_ADDCMD('GTOC', int, GetToolCount , 0, (), 0, ());
    NSCRIPT_ADDCMD('GSTI', int, GetSelectedToolIndex , 0, (), 0, ());
    NSCRIPT_ADDCMD('SETT', void, SelectTool, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SETP', void, SelectToolWithParameter, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD('JSWM', void, SetWorldMode, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('JSMT', void, SetCanMoveThroughTerrain, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGSC', int, GetSelectionCount , 0, (), 0, ());
    NSCRIPT_ADDCMD('JRSE', void, ResetSelection , 0, (), 0, ());
    NSCRIPT_ADDCMD('JAES', void, AddEntityToSelection, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('JASS', void, AddEntityToSelectionSilently, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('JRES', void, RemoveEntityFromSelection, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('JRSS', void, RemoveEntityFromSelectionSilently, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('JGSE', nEntityObject*, GetSelectedEntity, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGSP', vector3, GetSelectionCenterPoint, 0, (), 1, (float&));
    NSCRIPT_ADDCMD('JGSM', int, GetSelectionMode, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSSM', bool, SetSelectionMode, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JGME', nEntityObject*, GetSelectionModeEntity , 0, (), 0, ());
    NSCRIPT_ADDCMD('ECUT', void, CutEntities , 0, (), 0, ());
    NSCRIPT_ADDCMD('ECOP', void, CopyEntities , 0, (), 0, ());
    NSCRIPT_ADDCMD('EPAS', void, PasteEntities , 0, (), 0, ());
    NSCRIPT_ADDCMD('EDEL', void, DelEntities , 0, (), 0, ());
    NSCRIPT_ADDCMD('YSST', bool, SnapSelectionToTerrain , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGEP', nEntityObject*, GetEntityPlaced , 0, (), 0, ());
    NSCRIPT_ADDCMD('EBAT', void, BatchEntities, 0, (), 0, ());
    NSCRIPT_ADDCMD('ERBE', void, RemoveEntitiesFromBatch , 0, (), 0, ());
    NSCRIPT_ADDCMD('ERLE', void, ReloadEntities , 0, (), 0, ());
    NSCRIPT_ADDCMD('ELDE', void, LoadEntities , 0, (), 0, ());
    NSCRIPT_ADDCMD('EUDE', void, UnloadEntities , 0, (), 0, ());
    NSCRIPT_ADDCMD('MSTF', bool, SaveSelectionToFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('MLFF', bool, LoadSelectionFromFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('MALA', void, AddToLoadArea, 1, (const char *), 0, () );
    NSCRIPT_ADDCMD('MRLA', void, RemoveFromLoadArea, 1, (const char *), 0, () );
    NSCRIPT_ADDCMD('MCTB', void, CreateTerrainBatches, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSBR', void, SetBatchRebuildResource, 1, (bool), 0, () );
    NSCRIPT_ADDCMD('MGBR', bool, GetBatchRebuildResource, 0, (), 0, () );
    NSCRIPT_ADDCMD('MSBT', void, SetBatchTestSubentities, 1, (bool), 0, () );
    NSCRIPT_ADDCMD('MGBT', bool, GetBatchTestSubentities, 0, (), 0, () );
    NSCRIPT_ADDCMD('MSBD', void, SetBatchMinDistance, 1, (float), 0, () );
    NSCRIPT_ADDCMD('MGBD', float, GetBatchMinDistance, 0, (), 0, () );
    NSCRIPT_ADDCMD('ZRSE', void, ResetSelectionExceptions, 0, (), 0, () );
    NSCRIPT_ADDCMD('ZASE', void, AddSelectionException, 1, (const char *), 0, () );

    cl->BeginSignals( 5 );
    N_INITCMDS_ADDSIGNAL( ToolSelected );
    N_INITCMDS_ADDSIGNAL( LockedModeChanged );
    N_INITCMDS_ADDSIGNAL( SelectionChanged );
    N_INITCMDS_ADDSIGNAL( EntityModified );
    N_INITCMDS_ADDSIGNAL( SingleEntityPlaced );
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
