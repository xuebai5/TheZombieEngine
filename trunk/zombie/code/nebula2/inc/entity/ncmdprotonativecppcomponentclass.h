#ifndef N_CMDPROTONATIVECPPCOMPONENTCLASS_H
#define N_CMDPROTONATIVECPPCOMPONENTCLASS_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaScriptServices
    @ingroup NebulaScriptAutoWrapper
    @ingroup NebulaEntitySystem
    @class nCmdProtoNativeCPPComponentClass

    nCmdProtoNativeCPPComponentObject is used to build the scripting wrapper 
    automatically for component classes.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/ncmdproto.h"
#include "kernel/ncmdprototraits.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------

#pragma warning ( disable : 4127 4100 )

template < class TClass, class TR, class TListIn, class TListOut >
class nCmdProtoNativeCPPComponentClass : public nCmdProto
{
public:

    typedef nCmdProtoNativeCPPComponentClass<TClass,TR,TListIn,TListOut> MyCmdProtoNativeCPPCCType;

    typedef nCmdProtoTraits<TR,TListIn,TListOut> Traits;

    typedef typename Traits::TCmdDispatcher<TClass> TDispatcher;

    /// constructor
    nCmdProtoNativeCPPComponentClass( 
        const char * signature, 
        int fourcc, 
        typename TDispatcher::TMemberFunction memf, 
        const nComponentId & id 
    ) :
        nCmdProto(signature, fourcc),
        memf_(memf),
        compId(id)
    {
        /// empty
    }

    /// constructor with const method
    nCmdProtoNativeCPPComponentClass( 
        const char * signature, 
        int fourcc, 
        typename TDispatcher::TMemberFunctionConst memf, 
        const nComponentId & id 
    ) :
        nCmdProto(signature, fourcc),
        memf_(reinterpret_cast<typename TDispatcher::TMemberFunction> (memf)),
        compId(id)
    {
        /// empty
    }

    /// copy constructor
    nCmdProtoNativeCPPComponentClass(const MyCmdProtoNativeCPPCCType & rhs) :
        nCmdProto(rhs),
        compId(rhs.compId),
        memf_(rhs.memf_)
    {

    }

    /// clone the cmd proto
    virtual nCmdProto * Clone() const
    {
        return n_new(MyCmdProtoNativeCPPCCType(*this));
    }

    /// destructor
    virtual ~nCmdProtoNativeCPPComponentClass()
    {
        /// empty
    }

    /// dispatch command
    virtual bool Dispatch(void * slf, nCmd * cmd)
    {
        n_assert(slf && reinterpret_cast<nObject *> (slf)->IsA("nentityclass"));
        nEntityClass * ent = static_cast<nEntityClass *> (reinterpret_cast<nObject *> (slf));
        nComponentClass * comp = ent->GetComponent(this->compId);
        if (!comp)
        {
            NLOG(persistence, (0, "Dispatch command %s failed, not found component class %s in entity class %s", cmd->GetProto(), this->compId.AsChar(), ent->GetName()));
            return false;
        }
        TClass * obj = static_cast<TClass *>(comp);
        return TDispatcher::Dispatch(obj, memf_, cmd);
    }

private:

    nComponentId compId;
    typename TDispatcher::TMemberFunction memf_;

};

#pragma warning ( default : 4127 4100  )

//------------------------------------------------------------------------------

#endif // N_CMDPROTONATIVECPPCOMPONENTCLASS_H
