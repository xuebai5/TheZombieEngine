#ifndef NGPPICKUPITEM_H
#define NGPPICKUPITEM_H
//------------------------------------------------------------------------------
/**
    @class nGPPickupItem
    @ingroup NebulaGameplayBasicActions

    Pickup an item from the floor

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class nGPPickupItem : public nGPBasicAction
{
public:
    /// constructor
    nGPPickupItem();
    /// destructor
    ~nGPPickupItem();

        /// Initial condition
        bool Init(nEntityObject*,int);

    /// main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
};

//------------------------------------------------------------------------------
#endif//NGPPICKUPITEM_H
