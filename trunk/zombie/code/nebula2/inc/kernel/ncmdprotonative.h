#ifndef N_CMDPROTONATIVE_H
#define N_CMDPROTONATIVE_H
//------------------------------------------------------------------------------
/**
  @class nCmdProtoNative
  @ingroup NebulaScriptServices
  @ingroup NebulaObjectSystem
  @brief A factory for nCmd objects that correspond to natively implemented
         script commands.

  (c) 2003 Vadim Macagon

  nCmdProtoNative is licensed under the terms of the Nebula License
*/
#include "kernel/ncmdproto.h"

//--------------------------------------------------------------------
class nCmdProtoNative : public nCmdProto
{
public:
    /// constructor
    nCmdProtoNative(const char *_proto_def, nFourCC _id, void (*)(void *, nCmd *));
    /// copy constructor
    nCmdProtoNative(const nCmdProtoNative& rhs);

    /// clone the command proto
    virtual nCmdProto * Clone() const;
    /// execute a command on the provided object
    virtual bool Dispatch(void *, nCmd *);

private:
    /// pointer to C style command handler
    void (*cmd_proc)(void *, nCmd *);
};
//--------------------------------------------------------------------
#endif

