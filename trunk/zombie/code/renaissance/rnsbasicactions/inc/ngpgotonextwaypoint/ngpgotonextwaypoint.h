//------------------------------------------------------------------------------
/**
@class nGPGotoNextWaypoint
@ingroup nGPGotoNextWaypoint

This basic action go to the next waypoint of a path associated to an the entityliving.

(C) 2005 Copyright holder
*/
//------------------------------------------------------------------------------
#ifndef N_GPGOTONEXTWAYPOINT_H
#define N_GPGOTONEXTWAYPOINT_H

#include "ngpbasicaction/ngpbasicaction.h"

class ncGameplayLiving;
class ncAIMovEngine;

class nGPGotoNextWaypoint : public nGPBasicAction 
{
public:
    /// contructor
    nGPGotoNextWaypoint();
    /// destructor
    virtual ~nGPGotoNextWaypoint();

        /// Initial condition
        bool Init(nEntityObject*);

    /// Stop condition
    bool IsDone() const;

    /// End basic action
    //virtual void End();

private:

    ncGameplayLiving*   livingEntity;
	ncAIMovEngine*		movEngine;

    int                 path;
    int                 nextWayPoint;    
};

#endif//N_GPGOTONEXTWAYPOINT_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
