#ifndef N_PHYMESHLOAD_H
#define N_PHYMESHLOAD_H

//-----------------------------------------------------------------------------
/**
    @class nPhyMeshLoad
    @ingroup NebulaPhysicsSystem
    @brief Helper function to load n3d2 files data.

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------

#include "gfx2\nn3d2loader.h"

//-----------------------------------------------------------------------------

class nPhyMeshLoad : public nN3d2Loader {
public:
    /// open file and read header data
    bool Open(nFileServer2* fs);

    /// returns if there's normals info present
    const bool IsNormalsPresent() const;

private:
    /// stores if the mesh has normals information
    bool normalsPresent;
};

#endif