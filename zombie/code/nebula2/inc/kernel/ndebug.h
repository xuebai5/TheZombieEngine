#ifndef N_DEBUG_H
#define N_DEBUG_H
//------------------------------------------------------------------------------
/**
    Nebula debug macros.
  
    n_assert()  - the vanilla assert() Macro
    n_verify()  - like assert() except that the statement in parens is simply evaluated, unchecked, if __NEBULA_NO_ASSERT is set
    n_assert2() - an assert() plus a message from the programmer

   (C) 2006 Conjurer Services, S.A.
*/
#include <stdlib.h>

#define __NEBULA_SILENT_ASSERT__

#ifdef __NEBULA_NO_ASSERT__

#define n_assert(exp)
#define n_assert_always()
#define n_verify(exp) (exp)
#define n_verify2(exp,msg) (exp)
#define n_assert2(exp,msg)
#define n_assert2_always(msg)
#define n_assert3(exp,msg)
#define n_assert3_always(msg)
#define n_dxtrace(hr,msg)
#define n_dxverify(exp) (exp)
#define n_dxverify2(exp,msg) (exp)
#define N_IFDEF_ASSERTS(exp)

#ifdef __NEBULA_SILENT_ASSERT__

// assert returns with retcode if condition fails
#define n_assert_return(exp, retcode) { \
    if ( !(exp) ) { \
        return retcode; \
    } \
}

// assert returns with retcode if condition fails, displays msg
#define n_assert_return2(exp, retcode, msg) { \
    if (!(exp)) { \
        return retcode; \
    } \
}

// assert returns with retcode if condition fails, displays msg with variable arguments
#define n_assert_return3(exp, retcode, w) { \
    if (!(exp)) { \
        return retcode; \
    } \
}

// assert protecting execution of next block don't end with ;
#define n_assert_if(exp) \
    if ((exp))

// assert protecting execution of next block (display msg) don't end with ; 
#define n_assert_if2(exp, msg) \
    if ((exp))

// // assert protecting execution of next block (display msg with varargs) don't end with ; 
#define n_assert_if3(exp, msg) \
    if ((exp))

#else

// assert returns with retcode if condition fails
#define n_assert_return(exp, retcode)
// assert returns with retcode if condition fails, displays msg
#define n_assert_return2(exp, retcode, msg)
// assert returns with retcode if condition fails, displays msg with variable arguments
#define n_assert_return3(exp, retcode, msg)

// assert protecting execution of next block don't end with ;
#define n_assert_if(exp)
// assert protecting execution of next block (display msg) don't end with ; 
#define n_assert_if2(exp, msg)
// // assert protecting execution of next block (display msg with varargs) don't end with ; 
#define n_assert_if3(exp, msg)

#endif // __NEBULA_SILENT_ASSERT__


#else

#define n_assert(exp) { \
    static bool ignoreAlways = false; \
    if ( !(exp) && !ignoreAlways ) { \
        n_barf(#exp, __FILE__, __LINE__, &ignoreAlways); \
    } \
}

#define n_assert_always() { \
    static bool ignoreAlways = false; \
    if ( !ignoreAlways ) { \
        n_barf("assert always", __FILE__, __LINE__, &ignoreAlways); \
    } \
}

#define n_verify(exp) { \
    static bool ignoreAlways = false; \
    if ( !(exp) && !ignoreAlways ) { \
        n_barf(#exp, __FILE__, __LINE__, &ignoreAlways); \
    } \
}

#define n_verify2(exp,msg) {\
    static bool ignoreAlways = false; \
    if ( !(exp) && !ignoreAlways ) { \
        n_barf2(#exp,msg, __FILE__, __LINE__, &ignoreAlways); \
    } \
}

// an assert with a verbose hint from the programmer
#define n_assert2(exp,msg) { \
    static bool ignoreAlways = false; \
    if ( !(exp) && !ignoreAlways ) { \
        n_barf2(#exp,msg, __FILE__, __LINE__, &ignoreAlways); \
    } \
}

#define n_assert2_always(msg) { \
    static bool ignoreAlways = false; \
    if (!ignoreAlways) { \
        n_barf2("assert always",msg, __FILE__, __LINE__, &ignoreAlways); \
    } \
}


#define n_assert3(exp, w ) { \
    static bool ignoreAlways = false; \
    if ( !(exp) && !ignoreAlways ) { \
        nString msg;\
        msg.Format w;\
        n_barf2(#exp,msg.Get(), __FILE__, __LINE__, &ignoreAlways); \
    } \
}

#define n_assert3_always( w ) { \
    static bool ignoreAlways = false; \
    if ( !ignoreAlways ) { \
        nString msg;\
        msg.Format w ;\
        n_barf2("assert always", msg.Get(), __FILE__, __LINE__, &ignoreAlways); \
    } \
}


// dx9 specific: check HRESULT and display DX9 specific message box
#define n_dxtrace(hr, msg) { \
    if (FAILED(hr)) { \
        DXTrace(__FILE__,__LINE__,hr,msg,true); \
    } \
}

#define n_dxverify(exp) { \
    HRESULT hr = (exp); \
    n_dxtrace(hr, #exp); \
}

#define n_dxverify2(exp,msg) { \
    HRESULT hr = (exp); \
    n_dxtrace(hr, msg " " #exp); \
}

#define N_IFDEF_ASSERTS(exp) exp

// assert returns with retcode if condition fails
#define n_assert_return(exp, retcode) { \
    if ( !(exp) ) { \
        static bool ignoreAlways = false; \
        if ( !ignoreAlways ) { \
            n_barf(#exp,__FILE__,__LINE__,&ignoreAlways); \
        } \
        return retcode; \
    } \
}

// assert returns with retcode if condition fails, displays msg
#define n_assert_return2(exp, retcode, msg) { \
    if (!(exp)) { \
        static bool ignoreAlways = false; \
        if ( !ignoreAlways ) { \
            n_barf2(#exp,msg,__FILE__,__LINE__,&ignoreAlways); \
        } \
        return retcode; \
    } \
}

// assert returns with retcode if condition fails, displays msg with variable arguments
#define n_assert_return3(exp, retcode, w) { \
    if (!(exp)) { \
        static bool ignoreAlways = false; \
        if ( !ignoreAlways ) { \
            nString msg;\
            msg.Format w;\
            n_barf2(#exp,msg.Get(),__FILE__,__LINE__,&ignoreAlways); \
        } \
        return retcode; \
    } \
}

// assert protecting execution of next block don't end with ;
#define n_assert_if(exp) \
    if (!(exp)) { \
        static bool ignoreAlways = false; \
        if (!ignoreAlways) \
        { \
            n_barf(#exp,__FILE__,__LINE__,&ignoreAlways); \
        } \
    } \
    else

// assert protecting execution of next block (display msg) don't end with ; 
#define n_assert_if2(exp, msg) \
    if (!(exp)) { \
        static bool ignoreAlways = false; \
        if (!ignoreAlways) \
        { \
            n_barf2(#exp,msg,__FILE__,__LINE__,&ignoreAlways); \
        } \
    } \
    else

// // assert protecting execution of next block (display msg with varargs) don't end with ; 
#define n_assert_if3(exp, w) \
    if (!(exp)) { \
        static bool ignoreAlways = false; \
        if (!ignoreAlways) \
        { \
            nString msg;\
            msg.Format w;\
            n_barf2(#exp,msg.Get(),__FILE__,__LINE__,&ignoreAlways); \
        } \
    } \
    else

#endif

enum useroption { 
    UnknowUserOption=0,
    DumpUserOption,
    DebugUserOption
    };

/// Function that controls the user option for debug
void SetUserDebugOption(useroption option);
/// This funtion return the user option
useroption GetUserDebugOption(void);

//------------------------------------------------------------------------------
#endif
