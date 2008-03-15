#ifndef NCGPTHROWABLE_H
#define NCGPTHROWABLE_H

//------------------------------------------------------------------------------
/**
   @file ncgpthrowable.h
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/

#include "rnsgameplay/ncgameplay.h"

//------------------------------------------------------------------------------
class ncGPThrowable : public ncGameplay
{

    NCOMPONENT_DECLARE(ncGPThrowable,ncGameplay);

public:
    /// set initial state of the object
    virtual void SetInitial( const vector3 & dir, const vector3 & pos ) = 0;

    /// create the object
    virtual void Create() = 0;

    /// say if object is finaliced
    virtual bool IsDone() = 0;

private:

};

#endif//NCGPTHROWABLE_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
