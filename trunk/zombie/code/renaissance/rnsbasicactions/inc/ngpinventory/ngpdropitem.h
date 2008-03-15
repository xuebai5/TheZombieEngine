#ifndef NGPDROPITEM_H
#define NGPDROPITEM_H
//------------------------------------------------------------------------------
/**
    @class nGPDropItem
    @ingroup NebulaGameplayBasicActions

    Drop an item from the entity inventory to the ground

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class nGPDropItem : public nGPBasicAction
{
public:
    /// constructor
    nGPDropItem();
    /// destructor
    ~nGPDropItem();

        /// Initial condition
        bool Init(nEntityObject*,bool,int);

    /// main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
};

//------------------------------------------------------------------------------
#endif//NGPDROPITEM_H
