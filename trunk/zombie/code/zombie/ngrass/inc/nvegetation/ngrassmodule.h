#ifndef N_GRASSMODULE_H
#define N_GRASSMODULE_H
#include "kernel/ntypes.h"
#include "kernel/nlogclass.h"
//------------------------------------------------------------------------------
/**
    @namespace nGrassModule
    @ingroup ncTerrainVegetation
    @author Cristobal Castillo
    
    @brief This nameSpace is for common variable

    (C) 2006 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
namespace nGrassModule
{
    enum GrasLog
    {
        PROFILE = NLOG1,
        CREATION = NLOG2
    };


#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    /// the life time of debug text
    extern const float lifeTimeDebugText;

    struct profile 
    {
        profile(uint lastFrameId, const char* name);
        uint lastFrameId;
        float time;
        float accumTime;
        uint count;
        uint totalCount;
        uint countFrames;
        void AddTime(float time);
        const char* name;
    };
    void AddMeshLoadTime(float time);
    void AddMeshGenerationTime(float time);
    void AddSeedLoadTime(float time);

    const profile& GetMeshLoadTime();
    const profile& GetSeedLoadTime();
    const profile& GetMeshGenerationTime();

#endif
#endif
};

#endif //!N_GRASSMODULE_H
