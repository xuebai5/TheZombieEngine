#ifndef N_CMDPROTONATIVECPPCOMPONENTOBJECT_H
#define N_CMDPROTONATIVECPPCOMPONENTOBJECT_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaScriptServices
    @ingroup NebulaScriptAutoWrapper
    @class nCmdProtoNativeCPPComponentObject

    nCmdProtoNativeCPPComponentObject is used to build the scripting wrapper 
    automatically for component objects.

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
class nCmdProtoNativeCPPComponentObject : public nCmdProto
{
public:

    typedef nCmdProtoNativeCPPComponentObject<TClass,TR,TListIn,TListOut> MyCmdProtoNativeCPPCOType;

    typedef nCmdProtoTraits<TR,TListIn,TListOut> Traits;

    typedef typename Traits::TCmdDispatcher<TClass> TDispatcher;

    /// constructor
    nCmdProtoNativeCPPComponentObject( const char * signature, int fourcc, typename TDispatcher::TMemberFunction memf, const nComponentId & id ) :
        nCmdProto(signature, fourcc),
        memf_(memf),
        compId(id)
    {
        /// empty
    }

    /// constructor with const method
    nCmdProtoNativeCPPComponentObject( const char * signature, int fourcc, typename TDispatcher::TMemberFunctionConst memf, const nComponentId & id ) :
        nCmdProto(signature, fourcc),
        memf_(reinterpret_cast<typename TDispatcher::TMemberFunction> (memf)),
        compId(id)
    {
        /// empty
    }

    /// destructor
    virtual ~nCmdProtoNativeCPPComponentObject()
    {
        /// empty
    }

    /// copy constructor
    nCmdProtoNativeCPPComponentObject(const MyCmdProtoNativeCPPCOType & rhs) :
        nCmdProto(rhs),
        compId(rhs.compId),
        memf_(rhs.memf_)
    {
        /// empty
    }

    /// clone the cmd proto
    virtual nCmdProto * Clone() const
    {
        return n_new(MyCmdProtoNativeCPPCOType(*this));
    }

    /// dispatch command
    virtual bool Dispatch(void * slf, nCmd * cmd)
    {
        n_assert(slf && reinterpret_cast<nObject *> (slf)->IsA("nentityobject"));
        nEntityObject * ent = static_cast<nEntityObject *> (reinterpret_cast<nEntityObject *> (slf));
        nComponentObject * comp = ent->GetComponent(this->compId);
        if (!comp)
        {
            NLOG(persistence, (0, "Dispatch command %s failed, not found component object %s in entity object 0x%x", cmd->GetProto(), this->compId.AsChar(), ent->GetId()));
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

#endif // N_CMDPROTONATIVECPPCOMPONENTOBJECT_H
