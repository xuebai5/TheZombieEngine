#include "precompiled/pchconjurerexp.h"
//------------------------------------------------------------------------------
//  nlayer_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//  Author: Miquel Angel Rujula
//------------------------------------------------------------------------------
#include "nlayermanager/nlayer.h"
#include "kernel/npersistserver.h"
#include "signals/nsignalnative.h"

static void n_lock           (void* slf, nCmd* cmd);
static void n_unlock         (void* slf, nCmd* cmd);
static void n_islocked       (void* slf, nCmd* cmd);
static void n_setactive      (void* slf, nCmd* cmd);
static void n_isactive       (void* slf, nCmd* cmd);
static void n_setlayername   (void* slf, nCmd* cmd);
static void n_getlayername   (void* slf, nCmd* cmd);
static void n_setid          (void* slf, nCmd* cmd);
static void n_getid          (void* slf, nCmd* cmd);
static void n_setpassword    (void* slf, nCmd* cmd);
static void n_haspassword    (void* slf, nCmd* cmd);
static void n_clearpassword  (void* slf, nCmd* cmd);
static void n_checkpassword  (void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nLayer, LayerLocked);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlayer

    @cppclass
    nLayer

    @superclass
    nroot

    @classinfo
    nLayer groups elements.
*/
NSCRIPT_INITCMDS_BEGIN(nLayer)

    cl->AddCmd("v_lock_v",           'LCKL', n_lock);
    cl->AddCmd("v_unlock_v",         'ULKL', n_unlock);
    cl->AddCmd("b_islocked_v",       'ISLL', n_islocked);
    cl->AddCmd("v_setactive_b",      'STAV', n_setactive);
    cl->AddCmd("b_isactive_v",       'ISAL', n_isactive);
    cl->AddCmd("v_setlayername_s",   'SLNM', n_setlayername);
    cl->AddCmd("s_getlayername_v",   'GLNM', n_getlayername);
    cl->AddCmd("v_setid_i",          'SLID', n_setid);
    cl->AddCmd("i_getid_v",          'GLID', n_getid);
    cl->AddCmd("v_setpassword_s",    'YSLP', n_setpassword);
    cl->AddCmd("b_haspassword_v",    'YHPW', n_haspassword);
    cl->AddCmd("v_clearpassword_v",  'YCLP', n_clearpassword);
    cl->AddCmd("b_checkpassword_s",  'YCPW', n_checkpassword);

    cl->BeginSignals(1);
    N_INITCMDS_ADDSIGNAL(LayerLocked);
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    @cmd
    lock
    @input
    v()
    @output
    v()
    @info
    Locks the layer.
*/
static void
n_lock(void* slf, nCmd* /*cmd*/)
{
    nLayer *self = (nLayer*) slf;
    self->Lock();
}

//------------------------------------------------------------------------------
/**
    @cmd
    unlock
    @input
    v()
    @output
    v()
    @info
    Unlocks the layer.
*/
static void
n_unlock(void* slf, nCmd* /*cmd*/)
{
    nLayer *self = (nLayer*) slf;
    self->Unlock();
}

//------------------------------------------------------------------------------
/**
    @cmd
    islocked
    @input
    v()
    @output
    b(LayerLocked)
    @info
    Says if the layer is locked or not.
*/
static void
n_islocked(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    cmd->Out()->SetB(self->IsLocked());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setactive
    @input
    b(ActivateLayer)
    @output
    v()
    @info
    Activates/deactivates the layer.
*/
static void
n_setactive(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    self->SetActive(cmd->In()->GetB());
}

//------------------------------------------------------------------------------
/**
    @cmd
    isactive
    @input
    v()
    @output
    b(LayerActive)
    @info
    Says if the layer is active or not.
*/
static void
n_isactive(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    cmd->Out()->SetB(self->IsActive());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlayername
    @input
    s(LayerName)
    @output
    v()
    @info
    Sets the name of the layer.
*/
static void
n_setlayername(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    self->SetLayerName(cmd->In()->GetS());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlayername
    @input
    v()
    @output
    s(LayerName)
    @info
    Returns the name of the layer.
*/
static void
n_getlayername(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    cmd->Out()->SetS(self->GetLayerName());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setid
    @input
    i(LayerId)
    @output
    v()
    @info
    Sets the layer id.
*/
static void
n_setid(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    self->SetId(cmd->In()->GetI());
}

//------------------------------------------------------------------------------
/**
    @cmd
    getid
    @input
    v()
    @output
    i(LayerId)
    @info
    Returns the layer id.
*/
static void
n_getid(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    cmd->Out()->SetI(self->GetId());
}

//------------------------------------------------------------------------------
/**
    @cmd
    setpassword
    @input
    s(Password)
    @output
    b(success)
    @info
    Sets the password for the layer.
*/
static void
n_setpassword(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    self->SetPassword( cmd->In()->GetS() );
}

//------------------------------------------------------------------------------
/**
    @cmd
    haspassword
    @input
    v()
    @output
    b(has password)
    @info
    Confirms whether or not the layer has a password set.
*/
static void
n_haspassword(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    cmd->Out()->SetB(self->HasPassword());
}

//------------------------------------------------------------------------------
/**
    @cmd
    clearpassword
    @input
    v()
    @output
    v()
    @info
    Clear the password by setting it to 0
*/
static void
n_clearpassword(void* slf, nCmd* /*cmd*/)
{
    nLayer *self = (nLayer*) slf;
    self->ClearPassword();
}

//------------------------------------------------------------------------------
/**
    @cmd
    checkpassword
    @input
    s(Password)
    @output
    b(success)
    @info
    Return true if given password is correct.
*/
static void
n_checkpassword(void* slf, nCmd* cmd)
{
    nLayer *self = (nLayer*) slf;
    cmd->Out()->SetB( self->CheckPassword( cmd->In()->GetS() ) );
}

//------------------------------------------------------------------------------
/**
*/
bool
nLayer::SaveCmds(nPersistServer* /*ps*/)
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


