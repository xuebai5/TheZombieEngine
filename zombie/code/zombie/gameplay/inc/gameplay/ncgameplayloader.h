#ifndef NC_GAMEPLAYLOADER_H
#define NC_GAMEPLAYLOADER_H
//------------------------------------------------------------------------------
/**
    @class ncGameplayLoader
    @ingroup Entities
    @author Miquel Angel Rujula

    @brief Base entity component to load the asset data for gameplay entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "entity/nentity.h"
#include "zombieentity/ncloader.h"

//------------------------------------------------------------------------------
class ncGameplayLoader : public ncLoader
{

    NCOMPONENT_DECLARE(ncGameplayLoader,ncLoader);

public:
    /// constructor
    ncGameplayLoader();
    /// destructor
    virtual ~ncGameplayLoader();

    /// load object components
    bool LoadComponents();

};

//------------------------------------------------------------------------------
#endif