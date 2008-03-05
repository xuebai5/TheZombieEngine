#ifndef NCGAMEPLAYSQUAD_H
#define NCGAMEPLAYSQUAD_H

#include "ncgameplaygroup/ncgameplaygroup.h"

class nEntityObject;

class ncGameplaySquad : public ncGameplayGroup
{
    enum 
    {
        FLAG_INFORMATION    = 1<<0,
        FORMATION_COLUMN    = 1<<1,
        FORMATION_WEDGE     = 1<<2,
        FORMATION_DIAMOND   = 1<<3,
        FORMATION_4         = 1<<4,
        FORMATION_5         = 1<<5,
        FORMATION_ALL       = FORMATION_COLUMN | FORMATION_WEDGE | FORMATION_DIAMOND | FORMATION_4 | FORMATION_5,
        // ROE Orders
        ROE_FIREATWILL      = 1<<6,
        ROE_HOLDFIRE        = 1<<7, 
        ROE_ALL             = ROE_FIREATWILL | ROE_HOLDFIRE,
    };

public:
    /// Constructor
    ncGameplaySquad();
    /// Destructor
    ~ncGameplaySquad();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);

    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplaySquad)
        /// Sets/gets the squad leader
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISSL',, void, SetSquadLeader, 1, (unsigned int));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGSL',, unsigned int, GetSquadLeader) const;
        /// Sets/gets if the squad is in formation
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISIF',, void, SetInFormation, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGIF',, bool, GetInFormation) const;
        /// Sets/gets the formation in column
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISFC',, void, SetFormationColumn, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGFC',, bool, GetFormationColumn) const;
        /// Sets/gets the formation in wedge
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISFW',, void, SetFormationWedge, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGFW',, bool, GetFormationWedge) const;
        /// Sets/gets the formation in diamond
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISFD',, void, SetFormationDiamond, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGFD',, bool, GetFormationDiamond) const;
        /// Sets/gets the formation state (for persistency only)
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISFS',, void, SetFormationState, 1, (unsigned int));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGFS',, unsigned int, GetFormationState) const;
        /// Sets/gets the ROE option of fire at will
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISFW',, void, SetFireAtWill, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGFW',, bool, GetFireAtWill) const;
        /// Sets/gets the ROE option of hold fire
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISHF',, void, SetHoldFire, 1, (bool));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGHF',, bool, GetHoldFire) const;
    NCMDPROTONATIVECPP_DECLARE_END (ncGameplaySquad)

    NCOMPONENT_BEGIN_PARENT (ncGameplayLiving)
        NCOMPONENT_ADDCMD (SetSquadLeader);
        NCOMPONENT_ADDCMD (GetSquadLeader);
        NCOMPONENT_ADDCMD (SetInFormation);
        NCOMPONENT_ADDCMD (GetInFormation);
        NCOMPONENT_ADDCMD (SetFormationColumn);
        NCOMPONENT_ADDCMD (GetFormationColumn);
        NCOMPONENT_ADDCMD (SetFormationWedge);
        NCOMPONENT_ADDCMD (GetFormationWedge);
        NCOMPONENT_ADDCMD (SetFormationDiamond);
        NCOMPONENT_ADDCMD (GetFormationDiamond);
        NCOMPONENT_ADDCMD (SetFormationState);
        NCOMPONENT_ADDCMD (GetFormationState);
        NCOMPONENT_ADDCMD (SetFireAtWill);
        NCOMPONENT_ADDCMD (GetFireAtWill);
        NCOMPONENT_ADDCMD (SetHoldFire);
        NCOMPONENT_ADDCMD (GetHoldFire);
    NCOMPONENT_END()    

private:    
    void ResetFormations();
    void ResetROEs();

    nEntityObject*  leader;
    unsigned int    formationState;
};

//------------------------------------------------------------------------------
/**
    ResetFormations
*/
inline
void
ncGameplaySquad::ResetFormations()
{
    this->formationState &= ~FORMATION_ALL;
}

//------------------------------------------------------------------------------
/**
    ResetROEs
*/
inline
void
ncGameplaySquad::ResetROEs()
{
    this->formationState &= ~ROE_ALL;
}

//------------------------------------------------------------------------------
/**
    SetInFormation
*/
inline
void
ncGameplaySquad::SetInFormation (bool inFormation)
{
    if ( inFormation )
    {
        formationState |= FLAG_INFORMATION;
    }
    else
    {
        formationState &= ~FLAG_INFORMATION;
    }
}

//------------------------------------------------------------------------------
/**
    GetInFormation
*/
inline
bool
ncGameplaySquad::GetInFormation() const
{
    return (this->formationState&FLAG_INFORMATION) != 0;
}

//------------------------------------------------------------------------------
/**
    SetFormationColumn
*/
inline
void
ncGameplaySquad::SetFormationColumn (bool setColumn)
{
    this->ResetFormations();

    if ( setColumn )
    {
        this->formationState |= FORMATION_COLUMN;
    }
    else
    {
        this->formationState &= ~FORMATION_COLUMN;
    }
}

//------------------------------------------------------------------------------
/**
    GetFormationColumn
*/
inline
bool
ncGameplaySquad::GetFormationColumn() const
{
    return (this->formationState&FORMATION_COLUMN) != 0;
}

//------------------------------------------------------------------------------
/**
    SetFormationWedge
*/
inline
void
ncGameplaySquad::SetFormationWedge (bool setWedge)
{
    this->ResetFormations();
    
    if ( setWedge )
    {
        this->formationState |= FORMATION_WEDGE;
    }
    else
    {
        this->formationState &= ~FORMATION_WEDGE;
    }
}

//------------------------------------------------------------------------------
/**
    GetFormationWedge
*/
inline
bool
ncGameplaySquad::GetFormationWedge() const
{
    return (this->formationState&FORMATION_WEDGE) != 0;
}

//------------------------------------------------------------------------------
/**
    SetFormationDiamond
*/
inline
void
ncGameplaySquad::SetFormationDiamond (bool setDiamond)
{
    this->ResetFormations();

    if ( setDiamond )
    {
        this->formationState |= FORMATION_DIAMOND;
    }
    else
    {
        this->formationState &= ~FORMATION_DIAMOND;
    }
}

//------------------------------------------------------------------------------
/**
    GetFormationDiamond
*/
inline
bool
ncGameplaySquad::GetFormationDiamond() const
{
    return (this->formationState&FORMATION_DIAMOND) != 0;
}

//------------------------------------------------------------------------------
/**
    SetFormationState
*/
inline
void
ncGameplaySquad::SetFormationState (unsigned int state)
{
    this->formationState = state;
}

//------------------------------------------------------------------------------
/**
    GetFormationState
*/
inline
unsigned int
ncGameplaySquad::GetFormationState() const
{
    return this->formationState;
}

//------------------------------------------------------------------------------
/**
    SetFireAtWill
*/
inline
void
ncGameplaySquad::SetFireAtWill (bool setFireAtWill)
{
    this->ResetROEs();
    
    if ( setFireAtWill )
    {
        this->formationState |= ROE_FIREATWILL;
    }
    else
    {
        this->formationState &= ~ROE_FIREATWILL;
    }
}

//------------------------------------------------------------------------------
/**
    GetFireAtWill
*/
inline
bool
ncGameplaySquad::GetFireAtWill() const
{
    return (this->formationState&ROE_FIREATWILL) != 0;
}

//------------------------------------------------------------------------------
/**
    SetHoldFire
*/
inline
void
ncGameplaySquad::SetHoldFire (bool setHoldFire)
{
    this->ResetROEs();

    if ( setHoldFire )
    {
        this->formationState |= ROE_HOLDFIRE;
    }
    else
    {
        this->formationState &= ~ROE_HOLDFIRE;
    }
}

//------------------------------------------------------------------------------
/**
    GetHoldFire
*/
inline
bool
ncGameplaySquad::GetHoldFire() const
{
    return (this->formationState&ROE_HOLDFIRE) != 0;
}

#endif
