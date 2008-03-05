#ifndef N_GPACTIONSCRIPT_H
#define N_GPACTIONSCRIPT_H

//------------------------------------------------------------------------------
/**
    @class nGPActionScript
    @ingroup NebulaGameplayBasicActions

    @brief Action class that runs a scripted action

    Instead of loading the script each time an instance of this class is created,
    the script is accessed through an object that serves as an interface to that
    script. That object (and its associated script) is managed by the FSM server,
    which is expected to load it only once, so no performance penalty results in
    successive loads of the same script (the script will be already loaded and
    ready to play with it).
*/

#include "ngpbasicaction/ngpbasicaction.h"

//------------------------------------------------------------------------------
struct nGPActionScript : public nGPBasicAction
{
    // Empty
};

//------------------------------------------------------------------------------
#endif // N_GPACTIONSCRIPT_H
