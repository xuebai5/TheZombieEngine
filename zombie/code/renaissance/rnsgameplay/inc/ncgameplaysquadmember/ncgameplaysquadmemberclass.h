#ifndef NCGAMEPLAYSQUADMEMBERCLASS_H
#define NCGAMEPLAYSQUADMEMBERCLASS_H

#include "ncgameplayliving/ncgameplaylivingclass.h"

class ncGameplaySquadMemberClass : public ncGameplayLivingClass
{
public:
    /// Constructor
    ncGameplaySquadMemberClass();
    /// Destructor
    ~ncGameplaySquadMemberClass();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
    /*
    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplaySquadClass)
        NCMDPROTONATIVECPP_DECLARE_INARG ()
        NCMDPROTONATIVECPP_DECLARE_NOARG ()
    NCMDPROTONATIVECPP_DECLARE_END (ncGameplaySquadClass)

    NCOMPONENT_BEGIN_PARENT (ncGameplay)
        NCOMPONENT_ADDCMD ();
        NCOMPONENT_ADDCMD ();
    NCOMPONENT_END()
    */

private:
};

#endif