#ifndef NGPPLAYERADJUST_H
#define NGPPLAYERADJUST_H
//------------------------------------------------------------------------------
/**
    @class nGPPlayerAdjust
    @ingroup NebulaGameplayBasicActions

    Movement adjust for a client

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class nGPPlayerAdjust : public nGPBasicAction
{
public:
    /// Constructor
    nGPPlayerAdjust();

    /// Destructor
    ~nGPPlayerAdjust();

    /// Init
    bool Init(nEntityObject*, const float, const int, 
        const vector3 &, const vector3 &, const vector2 &, const vector2 & );

private:
};

#endif//NGPPLAYERADJUST_H
