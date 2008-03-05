#ifndef NCGAMEPLAYSTRUCTURE_H
#define NCGAMEPLAYSTRUCTURE_H

#include "gameplay/ncgameplay.h"

class ncGameplayStructure : public ncGameplay
{
public:
    /// Constructor
    ncGameplayStructure();
    /// Destructor
    ~ncGameplayStructure();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
     
    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplayStructure)
        // Sets/gets the current health of the vehicle
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISSH',, void, SetHealth, 1, (int));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGSH',, int, GetHealth) const;
    NCMDPROTONATIVECPP_DECLARE_END (ncGameplayStructure)

    NCOMPONENT_BEGIN_PARENT (ncGameplay)
        NCOMPONENT_ADDCMD (SetHealth);
        NCOMPONENT_ADDCMD (GetHealth);
    NCOMPONENT_END()

private:
    int health;
};

//------------------------------------------------------------------------------
/**
    SetHealth
*/
inline
void
ncGameplayStructure::SetHealth (int health)
{
    this->health = health;
}

//------------------------------------------------------------------------------
/**
    GetHealth
*/
inline
int
ncGameplayStructure::GetHealth() const
{
    return this->health;
}

#endif
