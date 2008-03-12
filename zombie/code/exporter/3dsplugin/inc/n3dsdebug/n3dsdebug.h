#ifndef N_3DS_DEBUG_H
#define N_3DS_DEBUG_H
//------------------------------------------------------------------------------
/**
    @class n3dsSkeleton
    @ingroup n3dsMaxExporterKernel
    @brief n3dsExporter tool for debug, is a wrapper of max functios

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

/// return the INode* name
const char* n3dsDebugINodeGetName( INode* node);
/// return the IGame name
const char* n3dsDebugIGameNodeGetName( IGameNode* node);
/// Return the material name
const char* n3dsDebugMtlGetName( Mtl* mat );

#endif