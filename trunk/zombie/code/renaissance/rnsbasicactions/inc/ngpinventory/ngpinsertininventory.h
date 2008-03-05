#ifndef NGPINSERTININVENTORY_H
#define NGPINSERTININVENTORY_H
//------------------------------------------------------------------------------
/**
    @class nGPInsertInInventory
    @ingroup NebulaGameplayBasicActions
    
    Insert a new item in the entity inventory
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class nGPInsertInInventory : public nGPBasicAction
{
public:
    /// constructor
    nGPInsertInInventory();
    /// destructor
    ~nGPInsertInInventory();

        /// Initial condition
        bool Init(nEntityObject*,const char *,int,int);

    /// main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:
};

//------------------------------------------------------------------------------
#endif//NGPINSERTININVENTORY_H
