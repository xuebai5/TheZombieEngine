#ifndef NGPPLAYERMOVEMENT_H
#define NGPPLAYERMOVEMENT_H
//------------------------------------------------------------------------------
/**
    @class nGPPlayerMovement
    @ingroup NebulaGameplayBasicActions

    Movement of a player

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
class polar2;

//------------------------------------------------------------------------------
class nGPPlayerMovement : public nGPBasicAction
{
public:
    /// Constructor
    nGPPlayerMovement();

    /// Destructor
    ~nGPPlayerMovement();

    /// Init
    bool Init(nEntityObject*, const float, const vector3&, const int, const int, 
        const vector2&,const float, const float, const int);
private:
};

//------------------------------------------------------------------------------
#endif//NGPPLAYERMOVEMENT_H
