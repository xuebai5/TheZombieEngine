//-----------------------------------------------------------------------------
//  nphymeshload_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphymeshload.h"

//-----------------------------------------------------------------------------
/**
    open file and read header data

    @param fs   fileserver to be used to perform the fileoperations

    history:
        - 29-Sep-2004   Zombie         created
*/
bool nPhyMeshLoad::Open(nFileServer2* fs)
{
    /// Bug in the loader code
    this->groupArray.Clear();

    bool result( nN3d2Loader::Open( fs ) );

    this->normalsPresent = this->vertexWidth > 3;

    return result;
}

//-----------------------------------------------------------------------------
/**
    Returns if there's normals info present

    @return true/false

    history:
        - 02-Feb-2006   Zombie         created
*/
const bool nPhyMeshLoad::IsNormalsPresent() const
{
    return this->normalsPresent;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
