#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  ncspecialfxclass.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/ncspecialfxclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSpecialFXClass,nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpecialFXClass)
    NSCRIPT_ADDCMD_COMPCLASS('MSFL', void, SetLifeTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGFL', float, GetLifeTime, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSFU', void, SetUnique, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGFU', bool, GetUnique, 0, (), 0, ());
    /*NSCRIPT_ADDCMD_COMPCLASS('MXXX', void, Set..., 1, (...), 0, ());*/
    NSCRIPT_ADDCMD_COMPCLASS('AAEI', void, SetFxType, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAEJ', const char*, GetFxType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFA', void, SetFXGlobal, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAFB', bool, GetFXGlobal, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ASSM', void, SetStartRandom, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AGSR', bool, GetStartRandom, 0, (), 0, ());

#ifndef NGAME
    NSCRIPT_ADDCMD_COMPCLASS('CAEI', void, SetFxTypeEdition, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CAEJ', int, GetFxTypeEdition, 0, (), 0, ());
#endif

NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSpecialFXClass::ncSpecialFXClass() :
    unique(false),
    lifetime(0.0f),
    // need a valid type  because the property editor call the getFx commadnd of empty nespecialfx
    type(FX_SPAWN),
    global(false),
    randomTimeOffset(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSpecialFXClass::~ncSpecialFXClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
const char*
ncSpecialFXClass::TypeToString(FxType t)
{
    switch (t)
    {
        case FX_DECAL:      return "Decal";
        case FX_SPAWN:  return "Spawn";
        default:
            n_error("ncSpecialFXClass::TypeToString(): invalid fx type value '%d'!", t);
            return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
ncSpecialFXClass::FxType
ncSpecialFXClass::StringToType(const char* str)
{
    n_assert(str);
    if (0 == strcmp(str, "Decal")) return FX_DECAL;
    else if (0 == strcmp(str, "Spawn")) return FX_SPAWN;
    else
    {
        n_error("ncSpecialFXClass::StringToType(): invalid fx type string '%s'!", str);
        return INVALIDTYPE;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpecialFXClass::SaveCmds(nPersistServer* ps)
{
    if (nComponentClass::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- setlifetime ---
        cmd = ps->GetCmd(this->GetEntityClass(), 'MSFL');
        cmd->In()->SetF(this->GetLifeTime());
        ps->PutCmd(cmd);

        // --- setunique ---
        cmd = ps->GetCmd(this->GetEntityClass(), 'MSFU');
        cmd->In()->SetB(this->GetUnique());
        ps->PutCmd(cmd);

        // --- settype ---
        if( this->type != INVALIDTYPE )
        {
            cmd = ps->GetCmd(this->GetEntityClass(), 'AAEI');
            cmd->In()->SetS(this->GetFxType());
            ps->PutCmd(cmd);
        }

        // --- SetFXGlobal ---
        cmd = ps->GetCmd(this->GetEntityClass(), 'AAFA');
        cmd->In()->SetB(this->GetFXGlobal());
        ps->PutCmd(cmd);

        //--- setstartatrandom ---
        cmd = ps->GetCmd(this->GetEntityClass(), 'ASSM');
        cmd->In()->SetB(this->GetStartRandom());
        ps->PutCmd(cmd);

        return true;
    }

    return false;
}
