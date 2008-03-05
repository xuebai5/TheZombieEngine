#ifndef N_DEFCLASS_H
#define N_DEFCLASS_H
//------------------------------------------------------------------------------
/**
    Macros which wraps the Nebula class module functions.
    Replaces the old *_init.cc Files.

    - 24-Mar-04     floh    removed the static nKernelServer* in nRoot derived
                            classes

    (C) 2001 RadonLabs GmbH
*/

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    Wraps the Nebula class package initialization function.

    Use like this:

    @code
    nNebulaUsePackage(blub);

    void bla()
    {
        nKernelServer kernelServer;
        kernelServer.AddPackage(blub);
    }
    @endcode
*/
#define nNebulaUsePackage(PACKAGE) extern "C" void PACKAGE()

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem
    
    nNebulaClass() creates a simple Nebula class without script interface.
    It takes the C name of the class, and a string defining the superclass
    name:

    @code
    nNebulaClass(nTestClass, "nparentclass");
    @endcode
*/
#define nNebulaClass(CLASS, SUPERCLASSNAME) \
    extern nClass* n_init_ ## CLASS(const char *, nKernelServer* kernelServer); \
    extern void* n_create_ ## CLASS(); \
    nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) {\
        nClass * clazz = n_new(nClass(name, kernelServer, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        return clazz; \
    }; \
    void* n_create_ ## CLASS() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    nNebulaClassStaticInit(), declare Nebula class and call static
    data initialization function

    @code
    nNebulaClassStaticInit(nTestClass, "nparentclass", staticInitFunction);
    @endcode
*/
#define nNebulaClassStaticInit(CLASS, SUPERCLASSNAME, INITSTATICDATAFUNC) \
    extern nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer); \
    extern void* n_create_ ## CLASS(); \
    nClass * n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) {\
        nClass * clazz = n_new(nClass(name, kernelServer, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        INITSTATICDATAFUNC(); \
        return clazz; \
    }; \
    void* n_create_ ## CLASS() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    nNebulaScriptClass() creates a Nebula class with script interface (you'll
    have to provide a function void n_initcmds(nClass*)).
    It takes the C name of the class, and a string defining the superclass
    name:

    @code
    nNebulaScriptClass(nTestClass, "nparentclass");
    @endcode
*/
#define nNebulaScriptClass(CLASS, SUPERCLASSNAME) \
    extern nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer); \
    extern void* n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) {\
        nClass * clazz = n_new(nClass(name, kernelServer, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        n_initcmds_ ## CLASS(clazz); \
        return clazz; \
    }; \
    void* n_create_ ## CLASS() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    nNebulaScriptClassStaticInit(), declare Nebula class with script interface
    and call static data initialization function.

    @code
    nNebulaScriptClassStaticInit(nTestClass, "nparentclass", staticInitFunction);
    @endcode
*/
#define nNebulaScriptClassStaticInit(CLASS, SUPERCLASSNAME, INITSTATICDATAFUNC) \
    extern nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer); \
    extern void* n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) {\
        nClass * clazz = n_new(nClass(name, kernelServer, n_create_ ## CLASS, true)); \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass(SUPERCLASSNAME, clazz); \
        INITSTATICDATAFUNC(); \
        n_initcmds_ ## CLASS(clazz); \
        return clazz; \
    }; \
    void* n_create_ ## CLASS() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
/**
    @ingroup NebulaObjectSystem

    @code
    nNebulaRootClass(): Special macro for the Nebula root class
    @endcode
*/

#define nNebulaRootClass(CLASS) \
    nKernelServer* CLASS::kernelServer = 0; \
    extern nClass* n_init_ ## CLASS(const char * name, nKernelServer* kernelServer); \
    extern void* n_create_ ## CLASS(); \
    extern void n_initcmds_ ## CLASS(nClass *); \
    nClass * n_init_ ## CLASS(const char * name, nKernelServer* kernelServer) {\
        nClass * clazz = n_new(nClass(name, kernelServer, n_create_ ## CLASS, true)); \
        CLASS::kernelServer = kernelServer; \
        clazz->SetProperName(#CLASS); \
        clazz->SetInstanceSize(sizeof(CLASS)); \
        kernelServer->AddClass("nil", clazz); \
        n_initcmds_ ## CLASS(clazz); \
        return clazz; \
    }; \
    void* n_create_ ## CLASS() { return n_new(CLASS()); };

//------------------------------------------------------------------------------
#endif
