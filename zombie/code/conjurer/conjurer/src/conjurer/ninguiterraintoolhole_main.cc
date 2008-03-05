#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolhole_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolhole.h"
#include "kernel/nkernelserver.h"
#include "tools/nmeshbuilder.h"
#include "nspatial/ncspatialquadtree.h"
#include "nphysics/ncphyterrain.h"
#include "ngeomipmap/ngeomipmapnode.h"
#include "nscene/ncsceneclass.h"
#include "conjurer/terrainholeundocmd.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nInguiTerrainToolHole, "ninguiterraintoolpolyline");
//------------------------------------------------------------------------------

/**
*/
nInguiTerrainToolHole::nInguiTerrainToolHole()
{
    this->closePolyMargin = 0.5f;
    this->displayColor = vector4 (1.0f, 1.0f, 1.0f, 1.0f);    
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolHole::~nInguiTerrainToolHole()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief End the line generating a hole with it and reset it
*/
void
nInguiTerrainToolHole::EndLine()
{
    if ( this->mustClose )
    {
        if ( !this->currentLine->Close() )
        {
            return;
        }
    }

    TerrainHoleUndoCmd * newCmd = n_new( TerrainHoleUndoCmd(this->currentLine) );

    n_assert2(newCmd, "Error creating undo command.");

    nString str = "Terrain Hole Undo Cmd";
    newCmd->SetLabel( str );
    nUndoServer::Instance()->NewCommand( newCmd );
    
    this->currentLine->Clear();
    this->SetState( nInguiTool::Inactive );
}
