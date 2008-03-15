#ifndef NCGAMEPLAYCLASS_H
#define NCGAMEPLAYCLASS_H

//------------------------------------------------------------------------------
/**
   @class ncGameplayClass
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"

//------------------------------------------------------------------------------
class nPersistServer;

//------------------------------------------------------------------------------
/**
    @class ncGameplayClass
    @ingroup Entities

    @brief Component Class that represent the game play.
*/
class ncGameplayClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncGameplayClass,nComponentClass);

public:
    /// constructor
    ncGameplayClass();
    /// destructor
    ~ncGameplayClass();

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Sets/gets the beauty name
    void SetBeautyName(nString);
    nString GetBeautyName() const;

private:
    /// General purpose attributes
    nString beautyName;

};

//------------------------------------------------------------------------------
/**
    SetBeautyName
*/
inline
void
ncGameplayClass::SetBeautyName (nString bName)
{
    this->beautyName = bName;
}

//------------------------------------------------------------------------------
/**
    GetBeautyName
*/
inline
nString
ncGameplayClass::GetBeautyName() const
{
    return this->beautyName;
}

#endif//NCGAMEPLAYCLASS_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
