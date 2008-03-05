#ifndef N_CMDARGTYPES_H
#define N_CMDARGTYPES_H
//------------------------------------------------------------------------------
/**
    @file ncmdargtypes.h
    @ingroup Kernel

    Command argument templates for each type.

    Normal types like bool, int, float, char *, nObject * (void *)
    are handled, and their respective const versions.
    Their pointer versions are not handled, there is no enough information
    to decide if handle them as a single value or as an array of values (and
    with which dimension). Anyway, it is not very usual to define types that
    way for functions aimed for scripting. In this case, it is safer to do
    the scripting wrapper manually (as before).

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/ncmd.h"

//------------------------------------------------------------------------------
class nObject;
class nRoot;
class nComponentObject;
class nComponentClass;
class nEntityObject;
class nEntityClass;

//------------------------------------------------------------------------------
#define N_CMDARGTYPE_NEW_TYPE(TYPE, SIGNATURE, GETCMD, SETCMD)   \
    N_CMDARGTYPE_NEW_SIGNATURE(TYPE, SIGNATURE)                  \
    N_CMDARGTYPE_NEW_GETCMD(TYPE, GETCMD)                        \
    N_CMDARGTYPE_NEW_SETCMD(TYPE, SETCMD)

#define N_CMDARGTYPE_NEW_TYPE_POINTER(TYPE, SIGNATURE, GETCMD, SETCMD)  \
    N_CMDARGTYPE_NEW_SIGNATURE(TYPE, SIGNATURE)                         \
    N_CMDARGTYPE_NEW_GETCMD(TYPE, GETCMD)                               \
    N_CMDARGTYPE_NEW_SETCMD(TYPE, SETCMD)                               \
    N_CMDARGTYPE_NEW_SPACE_FOR_POINTERS(TYPE)

    //N_CMDARGTYPE_NEW_SIGNATURE(const TYPE, SIGNATURE)
/*
    This compiles in VS8, but not in VS7.1 (error C2912) for basic types:
    N_CMDARGTYPE_NEW_SETCMD(const TYPE, SETCMD)

*/

#define N_CMDARGTYPE_NEW_SIGNATURE(TYPE, SIGNATURE) \
template <>                                         \
inline                                              \
const char * nGetSignatureStr< TYPE >()             \
{                                                   \
    return SIGNATURE;                               \
};

#define N_CMDARGTYPE_NEW_GETCMD(TYPE, GETCMD)                       \
template <>                                                         \
inline                                                              \
void                                                                \
nGetCmdArg< TYPE >( nCmd * cmd, TYPE & value )                      \
{                                                                   \
    (GETCMD);                                                       \
};

#define N_CMDARGTYPE_NEW_SETCMD(TYPE, SETCMD)                           \
template <>                                                             \
inline                                                                  \
void                                                                    \
nSetCmdArg< TYPE >( nCmd * cmd, TYPE const & value )                    \
{                                                                       \
    (SETCMD);                                                           \
};

#define N_CMDARGTYPE_NEW_SPACE_FOR_POINTERS(TYPE)                       \
template <>                                                             \
struct n_space_for_pointers<TYPE>                                       \
{                                                                       \
    n_space_for_pointers(TYPE & inarg) {                                \
        memset((void *) &space, 0, sizeof(space));                      \
        inarg = (TYPE) &space; };                                       \
    ~n_space_for_pointers() {};                                         \
    n_remove_pointer<TYPE>::Type space;                                 \
};

//------------------------------------------------------------------------------
template <typename T>
struct n_remove_modifiers_and_reference
{
    typedef T Type;
};

template<typename T> 
struct n_remove_modifiers_and_reference<T const>
{
    typedef typename n_remove_modifiers_and_reference<T>::Type Type;
};

template<typename T> 
struct n_remove_modifiers_and_reference<T &>
{
    typedef typename n_remove_modifiers_and_reference<T>::Type Type;
};

template<typename T> 
struct n_remove_modifiers_and_reference<T volatile>
{
    typedef typename n_remove_modifiers_and_reference<T>::Type Type;
};

template<typename T> 
struct n_remove_modifiers_and_reference<const T* >
{
    typedef typename n_remove_modifiers_and_reference<T*>::Type Type;
};

template<typename T> 
struct n_remove_modifiers_and_reference<volatile T* >
{
    typedef typename n_remove_modifiers_and_reference<T*>::Type Type;
};

//------------------------------------------------------------------------------
template <typename T>
struct n_remove_const
{
    typedef T Type;
};

template <typename T>
struct n_remove_const<const T>
{
    typedef T Type;
};

template <typename T>
struct n_remove_const<const T *>
{
    typedef T * Type;
};

template <typename T>
struct n_remove_pointer
{
    typedef T Type;
};

template <typename T>
struct n_remove_pointer<T *>
{
    typedef T Type;
};

template <>
struct n_remove_pointer<void *>
{
    typedef int Type;
};

template <typename T>
struct n_remove_reference
{
    typedef T Type;
};

template <typename T>
struct n_remove_reference<T &>
{
    typedef T Type;
};

//------------------------------------------------------------------------------

template <typename T>
struct n_space_for_pointers
{
    n_space_for_pointers(typename n_remove_modifiers_and_reference<T>::Type & /* inarg */) {}
    ~n_space_for_pointers() {}
};

//------------------------------------------------------------------------------
template<typename TType> 
struct conditionComponentObject 
{
    enum 
    {
        is = ((! Loki::SuperSubclass< nEntityObject, TType >::value) && Loki::SuperSubclass< nComponentObject, TType >::value)
    };
};

template<typename TType> 
struct conditionComponentClass 
{
    enum 
    {
        is = ( Loki::SuperSubclass< nComponentClass, TType >::value )
    };
};

template<typename TType> 
struct conditionnObject
{
    enum 
    {
        is = ( Loki::SuperSubclass< nObject, TType >::value )
    };
};

// binding with generic TClass
template<typename TSuperType, typename TClass> 
struct decide 
{
    static void GetCmdArg(nCmd * cmd, TClass & ret) 
    {
        n_assert3_always(("impossible to use this argument type in scripting cmd %s", cmd->GetProto()->GetProtoDef()));
        cmd->In()->GetO();
        ret = 0;
    }

    static void SetCmdArg(nCmd * cmd, TClass const & /*val*/) 
    {
        n_assert3_always(("impossible to use this argument type in scripting cmd %s", cmd->GetProto()->GetProtoDef()));
        cmd->Out()->SetO( 0 );
    }
};

// binding with generic TClass
template<typename TClass> 
struct decide<class nObject, TClass> 
{
    static void GetCmdArg(nCmd * cmd, TClass & ret) 
    {
        ret = static_cast<TClass> (reinterpret_cast<nObject *> (cmd->In()->GetO()));
    }

    static void SetCmdArg(nCmd * cmd, TClass const & val) 
    {
        cmd->Out()->SetO( (static_cast<nObject *> (val)) );
    }
};

// binding with nComponentObject
template<typename TClass> 
struct decide<class nEntityObject, TClass> 
{
    static void GetCmdArg(nCmd * cmd, TClass & ret) 
    {
        nEntityObject * obj = static_cast<nEntityObject *> (reinterpret_cast<nObject *> (cmd->In()->GetO()));
        ret = obj->GetComponent<n_remove_pointer<TClass>::Type>();
    }

    static void SetCmdArg(nCmd * cmd, TClass const & val) 
    {
        nEntityClass * obj = val->GetEntityObject();
        cmd->Out()->SetO( (void *) (static_cast<nObject *> (obj)) );
    }
};

// binding with nComponentClass
template<typename TClass> 
struct decide<class nEntityClass, TClass> 
{
    static void GetCmdArg(nCmd * cmd, TClass & ret) 
    {
        nEntityClass * obj = static_cast<nEntityClass *> (reinterpret_cast<nObject *> (cmd->In()->GetO()));
        ret = obj->GetComponent<TClass>();
    }

    static void SetCmdArg(nCmd * cmd, TClass const & val) 
    {
        nEntityClass * obj = val->GetEntityClass();
        cmd->Out()->SetO( (static_cast<nObject *> (obj)) );
    }
};

//------------------------------------------------------------------------------
template <typename T>
inline
const char * nGetSignatureStr()
{
    return "o";
};

template <typename TClass>
inline
void
nGetCmdArg( nCmd * cmd, TClass & ret )
{
    typedef n_remove_pointer<TClass>::Type TClassNude;

    typedef typename Loki::Select
    < 
        conditionnObject< TClassNude >::is , 
        nObject, 
        typename Loki::Select
        <
            conditionComponentClass< TClassNude >::is, 
            nEntityClass, 
            typename Loki::Select
            < 
                conditionComponentObject< TClassNude >::is , 
                nEntityObject, 
                TClassNude 
            >::Result 
        >::Result 
    >::Result TRefClass;

    decide< TRefClass, TClass >::GetCmdArg( cmd, ret );
};

template <typename TClass>
inline
void
nSetCmdArg( nCmd * cmd, TClass const & val ) 
{
    typedef n_remove_pointer<TClass>::Type TClassNude;

    typedef typename Loki::Select
    < 
        conditionnObject< TClassNude >::is , 
        nObject, 
        typename Loki::Select
        <
            conditionComponentClass< TClassNude >::is, 
            nEntityClass, 
            typename Loki::Select
            < 
                conditionComponentObject< TClassNude >::is , 
                nEntityObject, 
                TClassNude 
            >::Result 
        >::Result 
    >::Result TRefClass;

    decide< TRefClass, TClass >::SetCmdArg( cmd, val );
};

//------------------------------------------------------------------------------

// Special case for void
N_CMDARGTYPE_NEW_SIGNATURE(void, "v");

// Basic standard types
N_CMDARGTYPE_NEW_TYPE(bool, "b", ( value = (bool) (cmd->In()->GetB()) ), ( cmd->Out()->SetB(value)) );
N_CMDARGTYPE_NEW_TYPE(int, "i", (value = (int) cmd->In()->GetI()), (cmd->Out()->SetI(value))  );
N_CMDARGTYPE_NEW_TYPE(float, "f", (value = (float) cmd->In()->GetF()), (cmd->Out()->SetF(value)) );
N_CMDARGTYPE_NEW_TYPE(char *, "s", (value = (char *) cmd->In()->GetS()), (cmd->Out()->SetS(value)) );
N_CMDARGTYPE_NEW_TYPE(char, "i", (value = (char) cmd->In()->GetI()), (cmd->Out()->SetI(value))  );

//------------------------------------------------------------------------------
/**
    Declare new user types: vector2, vector3, vector4
*/

N_CMDARGTYPE_NEW_TYPE(nString, "s",
    (value.Set( (char *) cmd->In()->GetS()) ),
    (cmd->Out()->SetS( value.Get() ))
);

N_CMDARGTYPE_NEW_TYPE_POINTER(nString *, "s",
    (value->Set( cmd->In()->GetS())),
    ((value)?(cmd->Out()->SetS( value->Get())):(cmd->Out()->SetS( 0 )))
);

N_CMDARGTYPE_NEW_TYPE(vector2, "ff",
    (value.x = cmd->In()->GetF(), value.y = cmd->In()->GetF() ),
    (cmd->Out()->SetF( value.x ), cmd->Out()->SetF( value.y ) )
);

N_CMDARGTYPE_NEW_TYPE_POINTER(vector2 *, "ff",
    (value->x = cmd->In()->GetF(), value->y = cmd->In()->GetF() ),
    (cmd->Out()->SetF( value->x ), cmd->Out()->SetF( value->y ) )
);

N_CMDARGTYPE_NEW_TYPE(vector3, "fff",
    (value.x = cmd->In()->GetF(), value.y = cmd->In()->GetF(), value.z = cmd->In()->GetF()),
    (cmd->Out()->SetF( value.x ), cmd->Out()->SetF( value.y ), cmd->Out()->SetF( value.z ))
);

N_CMDARGTYPE_NEW_TYPE_POINTER(vector3 *, "fff",
    (value->x = cmd->In()->GetF(), value->y = cmd->In()->GetF(), value->z = cmd->In()->GetF()),
    (cmd->Out()->SetF( value->x ), cmd->Out()->SetF( value->y ), cmd->Out()->SetF( value->z ))
);

N_CMDARGTYPE_NEW_TYPE(vector4, "ffff",
    (value.x = cmd->In()->GetF(), value.y = cmd->In()->GetF(), value.z = cmd->In()->GetF(), value.w = cmd->In()->GetF()),
    (cmd->Out()->SetF( value.x ), cmd->Out()->SetF( value.y ), cmd->Out()->SetF( value.z ), cmd->Out()->SetF( value.w ))
);

N_CMDARGTYPE_NEW_TYPE_POINTER(vector4 *, "ffff",
    (value->x = cmd->In()->GetF(), value->y = cmd->In()->GetF(), value->z = cmd->In()->GetF(), value->w = cmd->In()->GetF()),
    (cmd->Out()->SetF( value->x ), cmd->Out()->SetF( value->y ), cmd->Out()->SetF( value->z ), cmd->Out()->SetF( value->w ))
);

N_CMDARGTYPE_NEW_TYPE(quaternion, "ffff",
    (value.x = cmd->In()->GetF(), value.y = cmd->In()->GetF(), value.z = cmd->In()->GetF(), value.w = cmd->In()->GetF()),
    (cmd->Out()->SetF( value.x ), cmd->Out()->SetF( value.y ), cmd->Out()->SetF( value.z ), cmd->Out()->SetF( value.w ))
);

N_CMDARGTYPE_NEW_TYPE_POINTER(quaternion *, "ffff",
    (value->x = cmd->In()->GetF(), value->y = cmd->In()->GetF(), value->z = cmd->In()->GetF(), value->w = cmd->In()->GetF()),
    (cmd->Out()->SetF( value->x ), cmd->Out()->SetF( value->y ), cmd->Out()->SetF( value->z ), cmd->Out()->SetF( value->w ))
);

N_CMDARGTYPE_NEW_TYPE(polar2, "ff",
    (value.rho = cmd->In()->GetF(), value.theta = cmd->In()->GetF() ),
    (cmd->Out()->SetF( value.rho ), cmd->Out()->SetF( value.theta ) )
);

N_CMDARGTYPE_NEW_TYPE(nTime, "f", 
    (value = (nTime) cmd->In()->GetF()), (cmd->Out()->SetF((float) value))
);

/**
    More could be defined:
    line2 ff ff
    line3 fff fff
    bbox3 fff fff
    plane ffff
    polar2 ff
    nFloat4, nFloat3, nFloat2, float2, float3, float4
*/

#endif
