#ifndef N_ENTITY_H
#define N_ENTITY_H
//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntitySystem
    @brief Main header for entities
    @author Mateu Batle

    (c) 2005 Conjurer Services, S.A.
*/ 
//------------------------------------------------------------------------------
#include "entity/nentityobject.h"
#include "entity/nentityclass.h"
#include "entity/ncomponentobject.h"
#include "entity/ncomponentclass.h"
#include "entity/nclassentityclass.h"
#include "entity/nclasscomponentclass.h"
#include "entity/nclasscomponentobject.h"
#include "entity/ncomponentclassserver.h"
#include "entity/ncomponentobjectserver.h"
#include "entity/ncmdprotonativecppcomponentclass.h"
#include "entity/ncmdprotonativecppcomponentobject.h"
#include "kernel/nmacros.h"

//------------------------------------------------------------------------------
void n_initcmds_nEntityClass(nClass * cl);
void n_initcmds_nEntityObject(nClass * cl);

//------------------------------------------------------------------------------
#define N_CMDARG_COMPONENT_OBJECT(TYPE)         \
    class TYPE;                                 \
    template <>                                 \
    inline                                      \
    const char * nGetSignatureStr< TYPE * >()   \
    {                                           \
        return "o";                             \
    };                                          \
    template <>                                 \
    inline                                      \
    void                                        \
    nGetCmdArg< TYPE * >( nCmd * cmd, TYPE * & ret )\
    {                                           \
        nObject * obj = static_cast<nObject*> (cmd->In()->GetO()); \
        n_assert2(obj->IsA("nentityobject"), "Script command expecting entity object argument"); \
        nEntityObject * ent = static_cast<nEntityObject *> (obj); \
        ret = ent->GetComponent<TYPE>();        \
    };                                          \
    template <>                                 \
    inline                                      \
    void                                        \
    nSetCmdArg< TYPE * >( nCmd * cmd, TYPE * const & val )  \
    {                                           \
        cmd->Out()->SetO(static_cast<nObject *> (val->GetEntityObject())); \
    };

#define N_CMDARG_COMPONENT_CLASS(TYPE)          \
    class TYPE;                                 \
    template <>                                 \
    inline                                      \
    const char * nGetSignatureStr<TYPE * >()    \
    {                                           \
        return "o";                             \
    };                                          \
    template <>                                 \
    inline                                      \
    void                                        \
    nGetCmdArg( nCmd * cmd, TYPE * & ret ) \
    {                                           \
        nObject * obj = static_cast<nObject*> (cmd->In()->GetO()); \
        if(obj->IsA("nentityobject"))           \
        {                                       \
            nEntityObject * ent = static_cast<nEntityObject *> (obj); \
            ret = ent->GetClassComponent<TYPE>(); \
        }                                       \
        else if(obj->IsA("nentityclass"))       \
        {                                       \
            nEntityClass * ent = static_cast<nEntityClass *> (obj); \
            ret = ent->GetComponent<TYPE>();    \
        }                                       \
        else                                    \
        {                                       \
            n_assert2_always("Script command expecting entity object/class argument"); \
        }                                       \
    };                                          \
    template <>                                 \
    inline                                      \
    void                                        \
    nSetCmdArg( nCmd * cmd, TYPE * const & val )  \
    {                                           \
        cmd->Out()->SetO(static_cast<nObject *> (val->GetEntityClass())); \
    };

//------------------------------------------------------------------------------
/**
    @def NCOMPONENT_DECLARE(COMPNAME)
    @brief Declaration of component objects and classes
    @param COMPNAME Name of the component object C++ class
    @param SUPERCLASS Name of the parent component object C++ class
*/
#define NCOMPONENT_DECLARE(COMPNAME,SUPERCLASS)                 \
public:                                                         \
    static const nComponentId & GetComponentIdStatic()          \
    {                                                           \
        static const nComponentId componentId(#COMPNAME);              \
        return componentId;                                     \
    }                                                           \
    virtual const nComponentId & GetComponentId() const         \
    {                                                           \
        return GetComponentIdStatic();                          \
    }                                                           \
    static const nComponentId & GetParentComponentIdStatic()    \
    {                                                           \
        return SUPERCLASS::GetComponentIdStatic();              \
    }                                                           \
    virtual const nComponentId & GetParentComponentId() const         \
    {                                                           \
        return GetParentComponentIdStatic();                    \
    }                                                           \
protected:                                                      \
    typedef COMPNAME ScriptClass_;                              \
    typedef COMPNAME ComponentType_;                            \
    typedef SUPERCLASS ParentComponentType_;                    \
private:

//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntitySystem

    nNebulaComponentObject() creates a Nebula component object with script interface 
    (you'll have to provide a function void n_initcmds(nClass*)).
    It takes the C name of the component object:

    @code
    nNebulaComponentObject(ncTransform);
    @endcode
*/
#define nNebulaComponentObject(CLASS,SUPERCLASS) \
    extern nClassComponentObject * n_init_ ## CLASS(const char * name, nComponentObjectServer * cos); \
    extern void * n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClassComponentObject * n_init_ ## CLASS(const char * name, nComponentObjectServer * cos) \
    { \
        nClassComponentObject * clazz = n_new(nClassComponentObject(name, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        cos->AddClass(CLASS::GetParentComponentIdStatic(), clazz); \
        n_initcmds_ ## CLASS(clazz); \
        return clazz; \
    } \
    void* n_create_ ## CLASS() { return n_new(CLASS()); };

#define nNebulaComponentObjectAbstract(CLASS,SUPERCLASS) \
    extern nClassComponentObject * n_init_ ## CLASS(const char * name, nComponentObjectServer * cos); \
    extern void * n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClassComponentObject * n_init_ ## CLASS(const char * name, nComponentObjectServer * cos) \
    { \
        nClassComponentObject * clazz = n_new(nClassComponentObject(name, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(0); \
        cos->AddClass(CLASS::GetParentComponentIdStatic(), clazz); \
        n_initcmds_ ## CLASS(clazz); \
        return clazz; \
    } \
    void* n_create_ ## CLASS() { n_assert2_always("error, component cannot be instanced"); return 0; };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntitySystem

    nNebulaComponentClass() creates a Nebula component class with script interface 
    (you'll have to provide a function void n_initcmds_ClassName(nClass*)).
    It takes the C name of the component class:

    @code
    nNebulaComponentClass(ncTransformClass);
    @endcode
*/
#define nNebulaComponentClass(CLASS,SUPERCLASS) \
    extern nClassComponentClass * n_init_ ## CLASS(const char * name, nComponentClassServer * ccs); \
    extern void * n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClassComponentClass * n_init_ ## CLASS(const char * name, nComponentClassServer * ccs) \
    { \
        nClassComponentClass * clazz = n_new(nClassComponentClass(name, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        ccs->AddClass(CLASS::GetParentComponentIdStatic(), clazz); \
        n_initcmds_ ## CLASS(clazz); \
        return clazz; \
    } \
    void* n_create_ ## CLASS() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
#define NSCRIPTCMD_TEMPLATE_COMP(TEMPLATE,FourCC,NAME,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)  \
{                                                                       \
    char ncmd_signature[N_MAXPATH];                                     \
    unsigned int fourcc = FourCC;                                       \
    typedef TEMPLATE< ScriptClass_, TR, TYPELIST_ ## NUMINPARAM ## INPARAM, TYPELIST_ ## NUMOUTPARAM ## OUTPARAM>          \
        TCmdProtoNativeCPP;                                             \
    TCmdProtoNativeCPP::Traits::CalcPrototype(ncmd_signature, #NAME);   \
    fourcc = TCmdProtoNativeCPP::Traits::CalcFourCC(#NAME, fourcc);     \
    TCmdProtoNativeCPP * val =                                          \
        n_new( TCmdProtoNativeCPP(                                      \
            ncmd_signature,fourcc,&ScriptClass_::MemberName,ScriptClass_::GetComponentIdStatic()) \
        );                                                              \
    cl->AddCmd(val);                                                    \
}

#define NSCRIPT_ADDCMD_COMPCLASS(FourCC,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)        \
    NSCRIPTCMD_TEMPLATE_COMP(nCmdProtoNativeCPPComponentClass,FourCC,MemberName,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)

#define NSCRIPT_ADDCMD_COMPOBJECT(FourCC,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)        \
    NSCRIPTCMD_TEMPLATE_COMP(nCmdProtoNativeCPPComponentObject,FourCC,MemberName,TR,MemberName,NUMINPARAM,INPARAM,NUMOUTPARAM,OUTPARAM)

#define NCOMPONENT_ADDSIGNAL(MemberName) \
    cl->AddSignal(n_new(ScriptClass_::TSignal ## MemberName(#MemberName)));

//------------------------------------------------------------------------------
/**
    nNebulaEntityClass creates code for C++ native classes, but there is only
    one of this (nEntityClass)
    @param CLASS C++ entity class 
    @param SUPERCLASSNAME C++ superclass of the entity class
*/
#define nNebulaEntityClass(CLASS, SUPERCLASS) \
    extern nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer); \
    extern void* n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) { \
        nClassEntityClass * clazz = n_new(nClassEntityClass(name, kernelServer, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(nEntityClass)); \
        kernelServer->AddClass(SUPERCLASS, clazz); \
        n_initcmds_nEntityClass(clazz); \
        return clazz; \
    }; \
    void* n_create_ ## CLASS() { return n_new(nEntityClass()); };

//------------------------------------------------------------------------------
/**
    @deprecated nNebulaEntityObject was the way to go before having automatic
    generation of component glue up code in the entity. Now not needed
    @param CLASS C++ entity class 
    @param SUPERCLASSNAME string with the nClass name of the superclass
    @param NCLASSENTITYCLASS string with the name of the entity class
*/
#define nNebulaEntityObject(CLASS,SUPERNCLASS,NCLASSENTITYCLASS) \
    extern nClass* n_init_ ## CLASS(const char *, nKernelServer* kernelServer); \
    extern void* n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) { \
        nString fullName(name); \
        fullName = "/" + fullName; \
        nClass * cl = kernelServer->FindClass(SUPERNCLASS); \
        while(0 != cl) { \
            fullName = cl->GetName() + fullName; \
            fullName = "/" + fullName; \
            cl = cl->GetSuperClass(); \
        } \
        fullName = "/sys" + fullName; \
        cl = kernelServer->FindClass(NCLASSENTITYCLASS); \
        cl->AddRef(); \
        nEntityClass * entClass = static_cast<nEntityClass *> (nKernelServer::Instance()->New(NCLASSENTITYCLASS, fullName.Get())); \
        entClass->nClass::InitClass(name, nKernelServer::Instance(), n_create_ ## CLASS, true); \
        entClass->nClass::SetProperName(#CLASS); \
        entClass->nClass::SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERNCLASS, entClass); \
        n_initcmds_nEntityObject(entClass); \
        return entClass; \
    } \
    void* n_create_ ## CLASS() { return n_new(CLASS()); } 

//------------------------------------------------------------------------------
/**
    Define a function returning a list of the component ids needed for a given
    class. This function is only used at startup time for configuring the
    native entity classes & objects.

    The NGAME version skips the edit components.
*/
#define _NENTITY_COMPID(NUMITEMS,LIST,NUM)  nComponentId(N_SELECTSTR(NUMITEMS,LIST,NUM)),

#ifndef NGAME

#define NENTITY_COMPLIST(CLASS, NUMCOMP, COMPLIST, NUMCOMPEDIT, COMPLISTEDIT) \
    const nComponentId * n_complist_ ## CLASS() { \
        static const nComponentId comps[] = { \
            N_REPEAT(_NENTITY_COMPID, NUMCOMPEDIT, COMPLISTEDIT) \
            N_REPEAT(_NENTITY_COMPID, NUMCOMP, COMPLIST) \
            compIdInvalid }; \
        return comps; \
    }
    
#else

#define NENTITY_COMPLIST(CLASS, NUMCOMP, COMPLIST, NUMCOMPEDIT, COMPLISTEDIT) \
    const nComponentId * n_complist_ ## CLASS() { \
        static const nComponentId comps[] = { \
            N_REPEAT(_NENTITY_COMPID, NUMCOMP, COMPLIST) \
            compIdInvalid }; \
        return comps; \
    }

#endif

//------------------------------------------------------------------------------
/**
*/
#define NENTITYCLASS_DEFINE(CLASS, NSUPERCLASS, NUMCOMP, COMPLIST, NUMCOMPEDIT, COMPLISTEDIT) \
    NENTITY_COMPLIST(CLASS, NUMCOMP, COMPLIST, NUMCOMPEDIT, COMPLISTEDIT) \
    void* n_create_ ## CLASS(); \
    extern nClass* n_init_ ## CLASS(const char * name, nKernelServer * kernelServer); \
    nClass* n_init_ ## CLASS(const char * name, nKernelServer * kernelServer) { \
        nClassEntityClass * clazz = n_new(nClassEntityClass(name, kernelServer, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        const nComponentId * compList = n_complist_ ## CLASS(); \
        clazz->SetupComponents( compList ); \
        clazz->SetInstanceSize(sizeof(nEntityClass)); \
        nString superClassName(# NSUPERCLASS); \
        superClassName.ToLower(); \
        kernelServer->AddClass(superClassName.Get(), clazz); \
        n_initcmds_nEntityClass(clazz); \
        return clazz; \
    } \
    void* n_create_ ## CLASS() { return n_new(nEntityClass()); }

//------------------------------------------------------------------------------
/**
*/
#define NENTITYOBJECT_DEFINE(CLASS,SUPERCLASS,ENTITYCLASS,NUMCOMP,COMPLIST,NUMCOMPEDIT,COMPLISTEDIT) \
    NENTITY_COMPLIST(CLASS, NUMCOMP, COMPLIST, NUMCOMPEDIT, COMPLISTEDIT) \
    void* n_create_ ## CLASS(); \
    extern nClass* n_init_ ## CLASS(const char * name, nKernelServer * kernelServer); \
    nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) { \
        nString fullName(name); \
        fullName = "/" + fullName; \
        nString supernclass(#SUPERCLASS); \
        supernclass.ToLower(); \
        nClass * cl = kernelServer->FindClass(supernclass.Get()); \
        while(0 != cl) { \
            fullName = cl->GetName() + fullName; \
            fullName = "/" + fullName; \
            cl = cl->GetSuperClass(); \
        } \
        fullName = "/sys" + fullName; \
        nString nclassentityclass(#ENTITYCLASS); \
        nclassentityclass.ToLower(); \
        cl = kernelServer->FindClass(nclassentityclass.Get()); \
        cl->AddRef(); \
        nEntityClass * entClass = static_cast<nEntityClass *> (nKernelServer::Instance()->New(nclassentityclass.Get(), fullName.Get(), false)); \
        entClass->nClass::InitClass(name, nKernelServer::Instance(), n_create_ ## CLASS, true); \
        entClass->nClass::SetProperName(#CLASS); \
        entClass->nClass::SetInstanceSize(sizeof(nEntityObject)); \
        entClass->InitInstance(nObject::NewInstance); \
        const nComponentId * compList = n_complist_ ## CLASS(); \
        entClass->SetupComponents( compList ); \
        kernelServer->AddClass(supernclass.Get(), entClass); \
        n_initcmds_nEntityObject(entClass); \
        return entClass; \
    } \
    void* n_create_ ## CLASS() { return n_new(nEntityObject()); }

//------------------------------------------------------------------------------
#endif //N_ENTITY_H
