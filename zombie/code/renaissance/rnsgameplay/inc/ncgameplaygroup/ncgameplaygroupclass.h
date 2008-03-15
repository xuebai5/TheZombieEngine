#ifndef NCGAMEPLAYGROUPCLASS_H
#define NCGAMEPLAYGROUPCLASS_H

//------------------------------------------------------------------------------
/**
    @class ncGameplayGroupClass

    (C) 2005 Conjurer Services, S.A.
*/

#include "gameplay/ncgameplayclass.h"

class ncGameplayGroupClass : public ncGameplayClass
{
public:
    /// Constructor
    ncGameplayGroupClass();
    /// Destructor
    ~ncGameplayGroupClass();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);

    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplayGroupClass)
        /// Sets/gets the max number of members in the squad
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISGM',, void, SetMaxMembers, 1, (int));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGGM',, int, GetMaxMembers) const;
    NCMDPROTONATIVECPP_DECLARE_END (ncGameplayGroupClass)

    NCOMPONENT_BEGIN_PARENT (ncGameplay)
        NCOMPONENT_ADDCMD (SetMaxMembers);
        NCOMPONENT_ADDCMD (GetMaxMembers);
    NCOMPONENT_END()

private:
    int maxMembers;
};

//------------------------------------------------------------------------------
/**
    SetMaxMembers
*/
inline
void
ncGameplayGroupClass::SetMaxMembers (int maxMembers)
{
    this->maxMembers = maxMembers;
}

//------------------------------------------------------------------------------
/**
    GetMaxMember
*/
inline
int
ncGameplayGroupClass::GetMaxMembers() const
{
    return this->maxMembers;
}

#endif