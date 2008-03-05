#ifndef N_CMDPROTOLUA_H
#define N_CMDPROTOLUA_H
//------------------------------------------------------------------------------
/**
  @class nCmdProtoLua
  @ingroup ScriptServices
  @ingroup NLuaServerContribModule
  @brief A factory for nCmd objects that correspond to Lua implemented
         script commands.

  (c) 2003 Vadim Macagon
  
  nCmdProtoLua is licensed under the terms of the Nebula License.
*/

#include "kernel/ncmdproto.h"

//--------------------------------------------------------------------
class nCmdProtoLua : public nCmdProto 
{
  public:
    /// Class constructor
    nCmdProtoLua(const char* protoDef);
    /// copy constructor
    nCmdProtoLua(const nCmdProtoLua& rhs);
    
    /// clone the command proto
    virtual nCmdProto * Clone() const;
    /// execute a command on the provided object
    virtual bool Dispatch(void *, nCmd *);

private:
    /// put a Script function in the Lua stack
    bool PushScriptClassFunction( void *, const char * name );
};
//--------------------------------------------------------------------
#endif    

