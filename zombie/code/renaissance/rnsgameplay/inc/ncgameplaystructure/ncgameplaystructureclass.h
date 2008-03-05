#ifndef NCGAMEPLAYSTRUCTURECLASS_H
#define NCGAMEPLAYSTRUCTURECLASS_H

#include "gameplay/ncgameplayclass.h"

class ncGameplayStructureClass : public ncGameplayClass
{
public:
    /// Constructor
    ncGameplayStructureClass();
    /// Destructor
    ~ncGameplayStructureClass();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
    
    // Script interface
    NCMDPROTONATIVECPP_DECLARE_BEGIN (ncGameplayStructureClass)
        /// Sets/gets the max health of the vehicle
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISMH',, void, SetMaxHealth, 1, (int));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGMH',, int, GetMaxHealth) const;
        /// Sets/gets the capacity of the vehicle
        NCMDPROTONATIVECPP_DECLARE_INARG ('ISVC',, void, SetCapacity, 1, (int));
        NCMDPROTONATIVECPP_DECLARE_NOARG ('IGVC',, int, GetCapacity) const;        
    NCMDPROTONATIVECPP_DECLARE_END (ncGameplayStructureClass)

    NCOMPONENT_BEGIN_PARENT (ncGameplayClass)
        NCOMPONENT_ADDCMD (SetMaxHealth);
        NCOMPONENT_ADDCMD (GetMaxHealth);
        NCOMPONENT_ADDCMD (SetCapacity);
        NCOMPONENT_ADDCMD (GetCapacity);
    NCOMPONENT_END()

private:
    int maxHealth;
    int capacity;
};

//------------------------------------------------------------------------------
/** 
    SetMaxHealth
*/
inline
void
ncGameplayStructureClass::SetMaxHealth (int maxHealth)
{
    this->maxHealth = maxHealth;
}

//------------------------------------------------------------------------------
/**
    GetMaxHealth
*/
inline
int
ncGameplayStructureClass::GetMaxHealth() const
{
    return this->maxHealth;
}

//------------------------------------------------------------------------------
/**
    SetCapacity
*/
inline
void
ncGameplayStructureClass::SetCapacity (int capacity)
{
    this->capacity = capacity;
}

//------------------------------------------------------------------------------
/**
    GetCapacity
*/
inline
int
ncGameplayStructureClass::GetCapacity() const
{
    return this->capacity;
}

#endif