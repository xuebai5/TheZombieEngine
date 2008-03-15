#ifndef NGPINVESTIGATE_H
#define NGPINVESTIGATE_H

//------------------------------------------------------------------------------
/**
    @class nGPInvestigate
    @ingroup NebulaGameplayBasicActions
    
    This basic action makes the entity look around for events in its memory
    or walk to them.
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------

class ncAIMovEngine;

class nGPInvestigate : public nGPBasicAction 
{
public:

    enum {
        GET_POINT,
        GOTO_POINT,
        FINISH
    };

    /// contructor
    nGPInvestigate();

    /// Initial condition
    bool Init(nEntityObject*, bool move, nTime time);

    /// Main execution loop
    bool Run();

    /// Stop condition
    bool IsDone() const;

private:

    // Whether to move or to look
    bool move;
    /// Search the next point to invest from entity memory
    bool FindInvestPoint( vector3& );

    // Move engine
    ncAIMovEngine* movEngine;

    nTime totalTime;
    nTime initTime;

    bool applyTimeOut;
};

#endif//NGPINVESTIGATE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
