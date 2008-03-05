#ifndef NGPDELETESPECIALITEM_H
#define NGPDELETESPECIALITEM_H
//------------------------------------------------------------------------------
/**
    @class nGPDeleteSpecialItem
    @ingroup NebulaGameplayBasicActions

    Remove a special item from the inventory

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class nGPDeleteSpecialItem : public nGPBasicAction
{
public:
    /// Constructor
    nGPDeleteSpecialItem();
    /// Destructor
    ~nGPDeleteSpecialItem();

        /// Initial condition
        bool Init(nEntityObject*,int);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
};

//------------------------------------------------------------------------------
#endif//NGPDELETESPECIALITEM_H
