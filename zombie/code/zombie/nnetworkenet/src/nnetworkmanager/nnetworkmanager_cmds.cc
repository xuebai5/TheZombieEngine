//------------------------------------------------------------------------------
//  nnetworkmanager_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nnetworkmanager.h"

//------------------------------------------------------------------------------
static void n_getservers( void *, nCmd *);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nNetworkManager);

    // No macro support
    cl->AddCmd("l_getservers_v", 'GTSR', n_getservers);

    NSCRIPT_ADDCMD('ISRV', bool, IsServer , 0, (), 0, ());
    NSCRIPT_ADDCMD('STSG', void, StartSearching, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('SPSG', void, StopSearching , 0, (), 0, ());
    NSCRIPT_ADDCMD('GNSR', int, GetNumServers , 0, (), 0, ());
    NSCRIPT_ADDCMD('CONN', bool, Connect, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('CONH', bool, ConnectHost, 2, (const char * const, const char * const), 0, ());
    NSCRIPT_ADDCMD('SPNG', void, Ping , 0, (), 0, ());
    NSCRIPT_ADDCMD('DISC', void, Disconnect , 0, (), 0, ());
    NSCRIPT_ADDCMD('GRTT', int, GetClientRTT , 0, (), 0, ());
    NSCRIPT_ADDCMD('SMCL', void, SetMaxClients, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('SPRT', void, SetServerPort, 1, (const nString &), 0, ());
    NSCRIPT_ADDCMD('STSR', void, StartServer , 0, (), 0, ());
    NSCRIPT_ADDCMD('STPS', void, StopServer , 0, (), 0, ());
    NSCRIPT_ADDCMD('KICK', void, Kick, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('LPIP', void, PushIntArg, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('LPOP', void, PushObjectArg, 1, (nObject*), 0, ());
    NSCRIPT_ADDCMD('LPFP', void, PushFloatArg, 1, (const float), 0, ());
    NSCRIPT_ADDCMD('LPSP', void, PushStringArg, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('LPCP', void, PushCharArg, 1, (const char), 0, ());
    NSCRIPT_ADDCMD('LPBP', void, PushBoolArg, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD('LSAC', void, SendActionScript, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD('LSQA', void, SendQueueActionScript, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD('LGCL', nTime, GetClock, 0, (), 0, ());

NSCRIPT_INITCMDS_END();

//------------------------------------------------------------------------------
/**
    @param slf object with execute the command
    @param cmd the command
*/
static void n_getservers( void * slf, nCmd * cmd )
{
    nNetworkManager* network = static_cast<nNetworkManager*>( slf );

    int num = network->GetNumServers();

    nArg * args = n_new_array( nArg, num);
    char ** cads = n_new_array( char*, num);

    network->GetServers( cads );

    for(int i=0 ; i<num ;++i)
    {
        args[i].SetS( cads[i] );
        n_delete_array( cads[i] );
    }

    n_delete_array( cads );

    cmd->Out()->SetL(args, num);
}

//------------------------------------------------------------------------------
