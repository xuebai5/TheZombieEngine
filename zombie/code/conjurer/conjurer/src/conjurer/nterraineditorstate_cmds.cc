#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterraineditorstate_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nterraineditorstate.h"
#include "kernel/npersistserver.h"

static void n_createterrainclass(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nTerrainEditorState)

    cl->AddCmd("b_createterrainclass_siiiiiif", 'JCTC', n_createterrainclass);

    NSCRIPT_ADDCMD('JSTC', bool, SetTerrainClassParams, 5, (nString, float, float, bool, float), 0, ());
    NSCRIPT_ADDCMD('JCTI', bool, CreateTerrainInstance, 1, (nString), 0, ());
    NSCRIPT_ADDCMD('SOUT', void, SetOutdoor, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD('IMPO', bool, ImportHeightMap, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('EXPO', bool, ExportHeightMap, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('PBCN', int, PaintbrushCount , 0, (), 0, ());
    NSCRIPT_ADDCMD('GPTF', nString, GetPaintbrushThumbnail, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SEPB', void, SelectPaintbrush, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GSPB', int, GetSelectedPaintbrush , 0, (), 0, ());
    NSCRIPT_ADDCMD('ISPB', bool, IsSizeablePaintbrush, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SPBS', void, SetPaintbrushSize, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GPBS', int, GetPaintbrushSize , 0, (), 0, ());
    NSCRIPT_ADDCMD('LDPB', int, LoadPaintbrushes , 0, (), 0, ());
    NSCRIPT_ADDCMD('SELT', void, SelectTool, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GSET', int, GetSelectedTool , 0, (), 0, ());
    NSCRIPT_ADDCMD('JGMI', bool, GetHeightSlope, 1, (vector3), 2, (float&, float&));
    NSCRIPT_ADDCMD('AACU', void, CreateTerrainLightMaps, 9, (const char*, int, int, int, nEntityObjectId, bool, const float, const vector2, const float), 0, ());
    NSCRIPT_ADDCMD('BCTG', void, CreateTerrainGlobalTexture, 0, (), 0, ());
    NSCRIPT_ADDCMD('BCTW', void, CreateTerrainWeightmaps, 0, (), 0, ());
    NSCRIPT_ADDCMD('BPPH', void, PreprocessHorizon, 0, (), 0, ());
    NSCRIPT_ADDCMD('EGPS', int, GetPaintbrushSizeByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('CCTL', bool, CreateTerrainGlobalLightMap, 2, (const char*, int), 0, ());

    cl->BeginSignals( 1 );
    N_INITCMDS_ADDSIGNAL( PaintbrushSizeChanged );
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    @cmd
    createterrainclass
    @input
    s(ClassName)
    i(HeighMapSize)
    i(BlockSize)
    i(VegetationCellsPerBlock)
    i(VegetationCelSize)
    i(WeightMapResolution)
    i(GlobalTextureResolution)
    f(GridScale)
    @output
    b(Success)
    @info
    Create a terrain class
*/
static void
n_createterrainclass(void* slf, nCmd* cmd)
{
    nTerrainEditorState* self = (nTerrainEditorState*) slf;
    const char * a = cmd->In()->GetS();
    int b = cmd->In()->GetI();
    int c = cmd->In()->GetI();
    int d = cmd->In()->GetI();
    int e = cmd->In()->GetI();
    int f = cmd->In()->GetI();
    int g = cmd->In()->GetI();
    float h = cmd->In()->GetF();

    bool result = self->CreateTerrainClass(a,b,c,d,e,f,g,h);

    cmd->Out()->SetB( result );
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
