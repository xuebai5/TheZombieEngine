#ifndef NCGAMEPLAYWHEEL_H
#define NCGAMEPLAYWHEEL_H

#include "rnsgameplay/ncgameplay.h"

class ncGameplayWheel : public ncGameplay
{

    NCOMPONENT_DECLARE(ncGameplayWheel,ncGameplay);

public:

    /// Constructor
    ncGameplayWheel();
    /// Destructor
    ~ncGameplayWheel();

    /// runs the gameplay logic 
    void Run( const float deltaTime );

private:

    /// updates wheel sound
    void UpdateSound();

    /// stores if the wheel was already sliding
    bool wasSliding;

};

#endif
