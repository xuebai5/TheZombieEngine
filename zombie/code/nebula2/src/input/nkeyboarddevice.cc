//------------------------------------------------------------------------------
//  nkeyboarddevice.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "input/nkeyboarddevice.h"

//------------------------------------------------------------------------------
/**
*/
nKeyboardDevice::nKeyboardDevice(nKernelServer* ks, nInputServer* is, int devNum, int numAxs, int numBtns, int numPvs) :
    nInputDevice(ks, is, devNum, numAxs, numBtns,numPvs)
{
    n_assert(0 == this->numAxes);
    n_assert(0 == this->numPovs);
}

//------------------------------------------------------------------------------
/**
*/
nKeyboardDevice::~nKeyboardDevice()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nKeyboardDevice::Export(nRoot* N_IFDEF_ASSERTS(dir) )
{
    n_assert(dir);

    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nKeyboardDevice::EmitButtonEvents(int N_IFDEF_ASSERTS(btnNum), bool /*pressed*/)
{
    n_assert((btnNum >= 0) && (btnNum < this->numButtons));

    // empty
}

//------------------------------------------------------------------------------


