#ifndef NCGAMEPLAYSQUADCLASS_H
#define NCGAMEPLAYSQUADCLASS_H

//------------------------------------------------------------------------------
/**
    @class ncGameplaySquadClass

    (C) 2005 Conjurer Services, S.A.
*/

#include "ncgameplaygroup/ncgameplaygroupclass.h"

class ncGameplaySquadClass : public ncGameplayGroupClass
{
public:
    /// Constructor
    ncGameplaySquadClass();
    /// Destructor
    ~ncGameplaySquadClass();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
/*
    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplaySquadClass)
        /// Sets/gets the max number of members in the squad
        NCMDPROTONATIVECPP_DECLARE_INARG ();
        NCMDPROTONATIVECPP_DECLARE_NOARG ();
        NCMDPROTONATIVECPP_DECLARE_END (ncGameplaySquadClass)

    NCOMPONENT_BEGIN_PARENT (ncGameplay)
        NCOMPONENT_ADDCMD ();
        NCOMPONENT_ADDCMD ();
    NCOMPONENT_END() */

private:
    int maxMembers;
};

#endif