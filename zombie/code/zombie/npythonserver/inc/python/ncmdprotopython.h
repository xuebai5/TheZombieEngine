//------------------------------------------------------------------------------
//   ncmdprotopython.h
//
//   (C)2004 Kim, Hyoun Woo.
//------------------------------------------------------------------------------
#ifndef N_CMDPROTOPYTHON_H
#define N_CMDPROTOPYTHON_H
//------------------------------------------------------------------------------
/**
    @class nCmdProtoPython
    @ingroup ScriptServices
    @ingroup PythonScriptServices
    @brief A factory for nCmd objects that correspond to Python
           implemented script commands.

    (c) 2004 Kim, Hyoun Woo

    nCmdProtoPython is licensed under the terms of the Nebula License
*/

#include "kernel/ncmdproto.h"

//------------------------------------------------------------------------------
class nCmdProtoPython : public nCmdProto
{
public:
    /// constructor
    nCmdProtoPython(const char* protodef);
    /// copy constructor
    nCmdProtoPython(const nCmdProtoPython& rhs);

    /// clone the command proto
    virtual nCmdProto * Clone() const;
    /// dispatch command
    virtual bool Dispatch(void *, nCmd *);
};
//------------------------------------------------------------------------------
#endif
