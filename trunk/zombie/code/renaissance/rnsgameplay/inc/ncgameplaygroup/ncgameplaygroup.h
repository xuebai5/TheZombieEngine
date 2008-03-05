#ifndef NCGAMEPLAYGROUP_H
#define NCGAMEPLAYGROUP_H

#include "gameplay/ncgameplay.h"

class nEntityObject;

class ncGameplayGroup : public ncGameplay
{
public:
    /// Constructor
    ncGameplayGroup();
    /// Destructor
    ~ncGameplayGroup();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
/*
    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplayGroup)
        /// Sets/gets the squad leader
        NCMDPROTONATIVECPP_DECLARE_INARG ();
        NCMDPROTONATIVECPP_DECLARE_NOARG ();
    NCMDPROTONATIVECPP_DECLARE_END (ncGameplayGroup)

    NCOMPONENT_BEGIN_PARENT (ncGameplayLiving)
        NCOMPONENT_ADDCMD ();
        NCOMPONENT_ADDCMD ();
    NCOMPONENT_END()    */

private:
    nArray<nEntityObject*>  members;  
};

#endif