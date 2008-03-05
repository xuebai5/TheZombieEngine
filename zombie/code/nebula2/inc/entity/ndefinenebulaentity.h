//------------------------------------------------------------------------------
/**
    @ingroup NebulaEntitySystem
    @brief  define the macro nNebulaEntity for define and declare entitobjects
    @author Cristobal Castillo

    (c) 2005 Conjurer Services, S.A.
*/

// Not included this file in precompiled headers

#ifdef N_DECLARE_NEBULA_ENTITY  // define nNebulaEntity()  for declare entityObject

    #undef nNebulaEntity
    #define nNebulaEntity(EO,EOS,NCEO,CEO,NCEOEDIT,CEOEDIT,EC,ECS,NCEC,CEC,NCECEDIT,CECEDIT) 

#else // N_DECLARE_NEBULA_ENTITY
#ifdef N_DEFINE_NEBULA_ENTITY // define nNebulaEntity()  for define entityObject

    #undef nNebulaEntity
    #define nNebulaEntity(EO,EOS,NCEO,CEO,NCEOEDIT,CEOEDIT,EC,ECS,NCEC,CEC,NCECEDIT,CECEDIT) \
        NENTITYCLASS_DEFINE(EC,ECS,NCEC,CEC,NCECEDIT,CECEDIT) \
        NENTITYOBJECT_DEFINE(EO,EOS,EC,NCEO,CEO,NCEOEDIT,CEOEDIT)

#else
    //#error need define N_DECLARE_NEBULA_ENTITY or N_DEFINE_NEBULA_ENTITY
    #define nNebulaEntity(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12)
#endif // N_DECLARE_NEBULA_ENTITY
#endif //N_DECLARE_NEBULA_ENTITY
