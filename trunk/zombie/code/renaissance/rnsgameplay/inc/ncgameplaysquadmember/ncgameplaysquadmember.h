#ifndef NCGAMEPLAYSQUADMEMBER_H
#define NCGAMEPLAYSQUADMEMBER_H

//------------------------------------------------------------------------------
/**
    @class ncGameplaySquadMember

    (C) 2005 Conjurer Services, S.A.
*/

#include "ncgameplayliving/ncgameplayliving.h"

class ncGameplaySquadMember : public ncGameplayLiving
{
public:
    /// Constructor
    ncGameplaySquadMember();
    /// Destructor
    ~ncGameplaySquadMember();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
    
    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplaySquadMember)
        /// Sets/tests can be killed condition
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISBK',, void, SetCanBeKilled, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('ICBK',, bool, CanBeKilled) const;
        /// Sets/tests tac random condition
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISTR',, void, SetTacRandom, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IITR',, bool, IsTacRandom) const;
        /// Sets/gets the pherocamo
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISPC',, void, SetPherocamo, 1, (int));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGPC',, int, GetPherocamo) const;
    NCMDPROTONATIVECPP_DECLARE_END (ncGameplaySquadMember)

    NCOMPONENT_BEGIN_PARENT (ncGameplay)
        NCOMPONENT_ADDCMD (SetCanBeKilled);
        NCOMPONENT_ADDCMD (CanBeKilled);
        NCOMPONENT_ADDCMD (SetTacRandom);
        NCOMPONENT_ADDCMD (IsTacRandom);
        NCOMPONENT_ADDCMD (SetPherocamo);
        NCOMPONENT_ADDCMD (GetPherocamo);
    NCOMPONENT_END()

    /// Sets/gets the current squad of the member
    void SetSquad (nEntityObject* squad);
    nEntityObject* GetSquad() const;

private:
    int pherocamo;
    nEntityObject* squad;
};

//------------------------------------------------------------------------------
/**
    SetCanBeKilled
*/
inline
void
ncGameplaySquadMember::SetCanBeKilled (bool activate)
{
    this->ToggleFlag (GP_CANBEKILLED, activate);
}

//------------------------------------------------------------------------------
/**
    CanBeKilled
*/
inline
bool
ncGameplaySquadMember::CanBeKilled() const
{
    this->ExistFlag (GP_CANBEKILLED);
}

//------------------------------------------------------------------------------
/**
    SetTacRandom
*/
inline
void
ncGameplaySquadMember::SetTacRandom (bool activate)
{
    this->ToggleFlag (GP_TACRANDOM, activate);
}

//------------------------------------------------------------------------------
/**
    IsTacRandom
*/
inline
bool
ncGameplaySquadMember::IsTacRandom() const
{
    return this->ExistFlag (GP_TACRANDOM);
}

//------------------------------------------------------------------------------
/**
    SetPherocamo
*/
inline
void
ncGameplaySquadMember::SetPherocamo (int value)
{
    this->pherocamo = value;
}

//------------------------------------------------------------------------------
/**
    GetPherocamo
*/
inline
int
ncGameplaySquadMember::GetPherocamo() const
{
    return this->pherocamo;
}

//------------------------------------------------------------------------------
/**
    SetSquad
*/
inline
void
ncGameplaySquadMember::SetSquad (nEntityObject* squad)
{
    this->squad = squad;
}

//------------------------------------------------------------------------------
/**
    GetSquad
*/
inline
nEntityObject*
ncGameplaySquadMember::GetSquad() const
{
    return this->squad;
}

#endif