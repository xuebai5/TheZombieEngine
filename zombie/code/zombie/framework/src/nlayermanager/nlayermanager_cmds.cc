#include "precompiled/pchconjurerexp.h"
//------------------------------------------------------------------------------
//  nlayermanager_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nlayermanager/nlayermanager.h"
#include "kernel/npersistserver.h"

static void n_createnewlayer     (void* slf, nCmd* cmd);
static void n_createnewlayer2    (void* slf, nCmd* cmd);
static void n_removelayerbyname  (void* slf, nCmd* cmd);
static void n_removelayerbyid    (void* slf, nCmd* cmd);
static void n_getnumlayers       (void* slf, nCmd* cmd);
static void n_searchlayerbyname  (void* slf, nCmd* cmd);
static void n_searchlayerbyid    (void* slf, nCmd* cmd);
static void n_getselectedlayerid (void* slf, nCmd* cmd);
static void n_selectlayer        (void* slf, nCmd* cmd);
static void n_setlayerlocked     (void* slf, nCmd* cmd);
static void n_setlayeractive     (void* slf, nCmd* cmd);
static void n_loadlayers         (void* slf, nCmd* cmd);
static void n_savelayers         (void* slf, nCmd* cmd);
static void n_setlayerpassword   (void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlayermanager

    @cppclass
    nLayerManager

    @superclass
    nroot

    @classinfo
    nLayerManager manages the layers system.
*/
void
n_initcmds_nLayerManager(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("ob_createnewlayer_si",   'CNLY', n_createnewlayer);
    cl->AddCmd("ob_createnewlayer2_sis", 'CNL2', n_createnewlayer2);
    cl->AddCmd("b_removelayerbyname_s",  'RLBN', n_removelayerbyname);
    cl->AddCmd("b_removelayerbyid_i",    'RLBI', n_removelayerbyid);
    cl->AddCmd("i_getnumlayers_v",       'GNLS', n_getnumlayers);
    cl->AddCmd("o_searchlayerbyname_s",  'SLBN', n_searchlayerbyname);
    cl->AddCmd("o_searchlayerbyid_i",    'SLBI', n_searchlayerbyid);
    cl->AddCmd("i_getselectedlayerid_v", 'GSLI', n_getselectedlayerid);
    cl->AddCmd("b_selectlayer_i",        'SLYR', n_selectlayer);
    
    cl->AddCmd("v_setlayerlocked_sbs", 'JSLL', n_setlayerlocked);
    cl->AddCmd("v_setlayeractive_sb", 'JSLA', n_setlayeractive);
    cl->AddCmd("b_loadlayers_s", 'JLLS', n_loadlayers);
    cl->AddCmd("b_savelayers_s", 'JSLS', n_savelayers);
    cl->AddCmd("v_setlayerpassword_ss", 'YSLP', n_setlayerpassword);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    createnewlayer
    @input
    s(LayerName)
    i(LayerId)
    @output
    o(LayerPointer)
    @info
    Creates a new layer.
*/
static void
n_createnewlayer(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    const char* layerName( cmd->In()->GetS() );
    int layerId( cmd->In()->GetI() );
    bool alreadyCreated;
    
    cmd->Out()->SetO( self->CreateNewLayer(layerName, layerId, alreadyCreated) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    createnewlayer2
    @input
    s(LayerName)
    i(LayerId)
    s(Password)
    @output
    o(LayerPointer)
    @info
    Creates a new layer by specifying the name, id and password.
*/
static void
n_createnewlayer2(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    const char* layerName( cmd->In()->GetS() );
    int layerId( cmd->In()->GetI() );
    const char* password( cmd->In()->GetS() );
    bool alreadyCreated;
    
    cmd->Out()->SetO( self->CreateNewLayer(layerName, layerId, password, alreadyCreated) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    removelayerbyname
    @input
    s(LayerName)
    @output
    b(WasRemoved)
    @info
    Removes a layer.
*/
static void
n_removelayerbyname(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetB(self->RemoveLayer(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    removelayerbyid
    @input
    i(LayerId)
    @output
    b(WasRemoved)
    @info
    Removes a layer.
*/
static void
n_removelayerbyid(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetB(self->RemoveLayer(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumlayers
    @input
    v()
    @output
    i(NumberOfLayers)
    @info
    Returns the number of layers in the layer manager.
*/
static void
n_getnumlayers(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetI(self->GetNumLayers());
}

//------------------------------------------------------------------------------
/**
    @cmd
    searchlayerbyname
    @input
    s(LayerName)
    @output
    o(Layer)
    @info
    Searches a layer.
*/
static void
n_searchlayerbyname(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetO(self->SearchLayer(cmd->In()->GetS()));
}


//------------------------------------------------------------------------------
/**
    @cmd
    searchlayerbyid
    @input
    i(LayerId)
    @output
    o(Layer)
    @info
    Searches a layer.
*/
static void
n_searchlayerbyid(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetO(self->SearchLayer(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    getselectedlayerid
    @input
    v
    @output
    i(LayerId)
    @info
    Get the selected layer id or -1 if no layer is currently selected.
*/
static void
n_getselectedlayerid(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetI(self->GetSelectedLayerId());
}

//------------------------------------------------------------------------------
/**
    @cmd
    selectlayer
    @input
    i(LayerId)
    @output
    b(WasSelected)
    @info
    Set the current selected layer.
*/
static void
n_selectlayer(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetB(self->SelectLayer(cmd->In()->GetI()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlayerlocked
    @input
    s(LayerName)
    b(Lock)
    @output
    v()
    @info
    Set lock state to layer by name.
*/
static void
n_setlayerlocked(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    const char * s1 = cmd->In()->GetS();
    bool b1 = cmd->In()->GetB();
    self->SetLayerLocked( s1, b1 );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlayeractive
    @input
    s(LayerName)
    @output
    b(Success)
    @info
    Set active state to layer by name.
*/
static void
n_setlayeractive(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    const char * s1 = cmd->In()->GetS();
    bool b1 = cmd->In()->GetB();
    self->SetLayerActive( s1, b1 );
}

//------------------------------------------------------------------------------
/**
    @cmd
    loadlayers
    @input
    s(FileName)
    @output
    b(Success)
    @info
    Load layers state from file
*/
static void
n_loadlayers(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetB(self->LoadLayers(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
    @cmd
    savelayers
    @input
    s(FileName)
    @output
    b(Success)
    @info
    Save layers state to file
*/
static void
n_savelayers(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    cmd->Out()->SetB(self->SaveLayers(cmd->In()->GetS()));
}

//------------------------------------------------------------------------------
/**
*/
bool
nLayerManager::SaveCmds(nPersistServer* /*ps*/)
{
    /*if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setspace ---
        cmd = ps->GetCmd(this, 'SSPF');
        cmd->In()->SetS(this->m_spaceFile.Get());
        ps->PutCmd(cmd);

        //--- settransform ---
        cmd = ps->GetCmd(this, 'SSTR');
        matrix44 trMatrix(this->GetTransformMatrix());
        for (int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {            
                cmd->In()->SetF(trMatrix.m[i][j]);
            }
        }
        ps->PutCmd(cmd);
        
        return true;
    }*/
    return false;
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlayerpassword
    @input
    s(LayerName)
    s(Password)
    @output
    @info
    Set password for named layer
*/
static void
n_setlayerpassword(void* slf, nCmd* cmd)
{
    nLayerManager *self = (nLayerManager*) slf;
    const char * s1 = cmd->In()->GetS();
    const char * s2 = cmd->In()->GetS();
    self->SetLayerPassword( s1, s2 );
}
