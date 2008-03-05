#ifndef N_CMDPROTONATIVECPP_H
#define N_CMDPROTONATIVECPP_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaScriptServices
    @ingroup NebulaScriptAutoWrapper
    @class nCmdProtoNativeCPP

    nCmdProtoNativeCPP classes are used to make easier thet definition of the script
    commands in Nebula classes (programmed in C++). It avoids to have to program
    the wrapper manually in plain C language.

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/ncmdproto.h"
#include "kernel/ncmdprototraits.h"

//#include "kernel/nroot.h"
//#include "kernel/nkernelserver.h"
#include "kernel/npersistserver.h"
#include "kernel/ncmdargtypes.h"

//------------------------------------------------------------------------------
/**
    @def NSCRIPTCMD_TEMPLATE(FourCC,NAME,PREFIX,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)
    @brief Declare a C++ function with input and/or output arguments (generic).
    @param TEMPLATE     template to use for the nCmdProto
    @param FourCC       fourcc code assigned to the scripting function
    @param NAME         name of the scripting command
    @param TR           return type
    @param MemberName   name of the function in C++
    @param NUMINPARAM   number of input parameters
    @param INPARAM      list of input parameter types between parenthesis and separated by commas
    @param NUMOUTPARAM  number of output parameters
    @param OUTPARAM     list of output parameter types between parenthesis and separated by commas
*/
#define NSCRIPTCMD_TEMPLATE(TEMPLATE,FourCC,NAME,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)  \
{                                                                       \
    char ncmd_signature[N_MAXPATH];                                     \
    unsigned int fourcc = FourCC;                                       \
    typedef TEMPLATE< ScriptClass_, TR, TYPELIST_ ## NUMINPARAM ## INPARAM, TYPELIST_ ## NUMOUTPARAM ## OUTPARAM, nObject >          \
        TCmdProtoNativeCPP;                                             \
    TCmdProtoNativeCPP::Traits::CalcPrototype(ncmd_signature, #NAME);   \
    fourcc = TCmdProtoNativeCPP::Traits::CalcFourCC(#NAME, fourcc);     \
    TCmdProtoNativeCPP * val =                                          \
        n_new( TCmdProtoNativeCPP(                                      \
            ncmd_signature,fourcc,&ScriptClass_::MemberName)            \
        );                                                              \
    cl->AddCmd(val);                                                    \
}

//------------------------------------------------------------------------------
/**
    @def NSCRIPT_INITCMDS_BEGIN(ClassName)
    @brief Begin n_initcmds function & cl->BeginCmds()
    @param ClassName Name of the C++ class having the commands.
*/
#define NSCRIPT_INITCMDS_BEGIN(ClassName)                   \
void                                                        \
n_initcmds_ ## ClassName(nClass* cl)                        \
{                                                           \
    typedef ClassName ScriptClass_;                         \
    cl->BeginCmds();

//------------------------------------------------------------------------------
/**
    @def NSCRIPT_ADDCMD(FourCC,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)
    @brief Adds the command to the nClass in n_initcmds functions.
    @param FourCC       fourcc code assigned to the scripting function
    @param TR           return type
    @param MemberName   name of the function in C++
    @param NUMINPARAM   number of input parameters
    @param INPARAM      list of input parameter types between parenthesis and separated by commas
    @param NUMOUTPARAM  number of output parameters
    @param OUTPARAM     list of output parameter types between parenthesis and separated by commas
*/
#define NSCRIPT_ADDCMD(FourCC,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)        \
    NSCRIPTCMD_TEMPLATE(nCmdProtoNativeCPP,FourCC,MemberName,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)

//------------------------------------------------------------------------------
/**
    @def NSCRIPT_INITCMDS_END()
    @brief End of n_initcmds function & cl->EndCmds()
*/
#define NSCRIPT_INITCMDS_END()                              \
    cl->EndCmds();                                          \
}

//------------------------------------------------------------------------------

#pragma warning ( disable : 4127 4100 )

template < class TClass, class TR, class TListIn, class TListOut, class TClassCast >
class nCmdProtoNativeCPP : public nCmdProto
{
public:

    typedef nCmdProtoNativeCPP<TClass,TR,TListIn,TListOut,TClassCast> MyCmdProtoNativeType;

    typedef nCmdProtoTraits<TR,TListIn,TListOut> Traits;

    typedef typename Traits::TCmdDispatcher<TClass> TDispatcher;

    /// constructor
    nCmdProtoNativeCPP( const char * signature, int fourcc, typename TDispatcher::TMemberFunction memf ) :
        nCmdProto(signature, fourcc),
        memf_(memf)
    {
        /// empty
    }

    /// constructor with const method
    nCmdProtoNativeCPP( const char * signature, int fourcc, typename TDispatcher::TMemberFunctionConst memf ) :
        nCmdProto(signature, fourcc),
        memf_(reinterpret_cast<typename TDispatcher::TMemberFunction> (memf))
    {
        /// empty
    }

    /// copy constructor 
    nCmdProtoNativeCPP(const MyCmdProtoNativeType & rhs) :
        nCmdProto(rhs),
        memf_(rhs.memf_)
    {
        /// empty
    }

    /// destructor
    virtual ~nCmdProtoNativeCPP()
    {
        /// empty
    }

    /// clone the command proto
    virtual nCmdProto * Clone() const
    {
        return n_new(MyCmdProtoNativeType(*this));
    }

    /// dispatch command
    virtual bool Dispatch(void * slf, nCmd * cmd)
    {
        TClass * obj = static_cast<TClass *> (reinterpret_cast<TClassCast *> (slf));
        return TDispatcher::Dispatch(obj, memf_, cmd);
    }

private:

    typename TDispatcher::TMemberFunction memf_;

};

#pragma warning ( default : 4127 4100  )

//------------------------------------------------------------------------------

#endif
