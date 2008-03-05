#ifndef NINPUTLOGEVENT_H
#define NINPUTLOGEVENT_H
//------------------------------------------------------------------------------
/**
   @file ninputlogevent.h
   @author Luis Jose Cabellos Gomez
   
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#include "util/nnode.h"

class nInputEvent;

//------------------------------------------------------------------------------
/**
    @class nInputLogEvent
    @ingroup NebulaInputSystem
    @brief recorded nInputEvent with the time when it ocurred
*/
class nInputLogEvent : public nNode
{
public:
    nInputEvent * event;
    double time;

private:

};

#endif//NINPUTLOGEVENT_H

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
