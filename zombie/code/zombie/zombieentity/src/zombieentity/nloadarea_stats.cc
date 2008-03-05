#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nloadarea_stats.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#ifndef NGAME
#include "zombieentity/nloadarea.h"
#include "zombieentity/nloaderserver.h"

#include "kernel/nfileserver2.h"

#include "nscene/ncsceneclass.h"
#include "nscene/ncscenelodclass.h"
#include "nscene/nabstractshadernode.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nshapenode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/ninstshapenode.h"

#include "gfx2/nmesh2.h"
#include "gfx2/ntexture2.h"

const char *nLoadArea::statsCounterStrings[NumStatsCounters] =
{
    "StatsNumEntities",
    "StatsNumClasses",
    "StatsNumMeshes",
    "StatsMeshSize",
    "StatsNumTextures",
    "StatsTextureSize",
};

///helper lists to track which classes have been already visited
nArray<nEntityClass*> visitedClasses;
nArray<nTexture2*> visitedTextures;
nArray<nMesh2*> visitedMeshes;

//------------------------------------------------------------------------------
/**
    a qsort() hook for sorting textures by size
*/
int
__cdecl
TextureSorter(const void *i1, const void *i2)
{
    nTexture2* texture1 = (nTexture2*) *((nTexture2**)i1);
    nTexture2* texture2 = (nTexture2*) *((nTexture2**)i2);

    int diff = texture1->GetWidth() - texture2->GetWidth();
    if (!diff)
    {
        diff = texture1->GetHeight() - texture2->GetHeight();
    }

    return diff;
}

//------------------------------------------------------------------------------
/**
    a qsort() hook for sorting textures by size
*/
int
__cdecl
MeshSorter(const void *i1, const void *i2)
{
    nMesh2* mesh1 = (nMesh2*) *((nMesh2**)i1);
    nMesh2* mesh2 = (nMesh2*) *((nMesh2**)i2);

    int diff = mesh1->GetNumIndices() - mesh2->GetNumIndices();

    return diff;
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::GetStatsCountersFromArrays()
{
    this->statsCounters[StatsNumClasses] = visitedClasses.Size();
    this->statsCounters[StatsNumMeshes] = visitedMeshes.Size();
    this->statsCounters[StatsNumTextures] = visitedTextures.Size();
    
    this->statsCounters[StatsMeshSize] = 0;
    for (int index = 0; index < visitedMeshes.Size(); ++index)
    {
        this->statsCounters[StatsMeshSize] += visitedMeshes[index]->GetByteSize();
    }

    this->statsCounters[StatsTextureSize] = 0;
    for (int index = 0; index < visitedTextures.Size(); ++index)
    {
        this->statsCounters[StatsTextureSize] += visitedTextures[index]->GetByteSize();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::GetDebugStatsFromClass(nEntityClass* entityClass)
{
    if (visitedClasses.Find(entityClass))
    {
        return;
    }

    visitedClasses.Append(entityClass);

    //make sure the class is loaded
    ncLoaderClass* loaderClass = entityClass->GetComponent<ncLoaderClass>();
    if (loaderClass && loaderClass->AreResourcesValid())
    {
        //traverse down all scene hierarchies looking for different geometries and textures
        ncSceneLodClass* sceneLodClass = entityClass->GetComponent<ncSceneLodClass>();
        if (sceneLodClass && sceneLodClass->GetNumLevels() > 0)
        {
            for (int level = 0; level < sceneLodClass->GetNumLevels(); ++level)
            {
                for (int index = 0; index < sceneLodClass->GetNumLevelRoots(level); ++index)
                {
                    this->GetDebugStatsFromNode(sceneLodClass->GetLevelRoot(level, index));
                }
            }
        }
        else
        {
            ncSceneClass* sceneClass = entityClass->GetComponent<ncSceneClass>();
            if (sceneClass && sceneClass->IsValid())
            {
                this->GetDebugStatsFromNode(sceneClass->GetRootNode());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::GetDebugStatsFromNode(nSceneNode* sceneNode)
{
    static nClass* abstractShaderNodeClass(kernelServer->FindClass("nabstractshadernode"));
    static nClass* geometryNodeClass(kernelServer->FindClass("ngeometrynode"));
    static nClass* shapeNodeClass(kernelServer->FindClass("nshapenode"));
    static nClass* instShapeNodeClass(kernelServer->FindClass("ninstshapenode"));

    if (!sceneNode)
    {
        return;
    }

    if (sceneNode->IsA(abstractShaderNodeClass))
    {
        //retrieve texture statistics from this node
        nAbstractShaderNode* textureNode = static_cast<nAbstractShaderNode*>(sceneNode);
        for (int tex = 0; tex < textureNode->GetNumTextures(); ++tex)
        {
            nShaderState::Param param = textureNode->GetTextureParamAt(tex);
            if (textureNode->GetShaderParams().IsParameterValid(param))
            {
                nTexture2* texture = textureNode->GetShaderParams().GetArg(param).GetTexture();
                if (texture && !visitedTextures.Find(texture))
                {
                    visitedTextures.Append(texture);
                }
            }
        }
    }

    if (sceneNode->IsA(geometryNodeClass))
    {
        //retrieve texture stats from surface node
        nSurfaceNode* surfaceNode = static_cast<nGeometryNode*>(sceneNode)->GetSurfaceNode();
        if (surfaceNode)
        {
            this->GetDebugStatsFromNode(surfaceNode);
        }

        //retrieve mesh stats from streamed nodes
        nGeometryNode* streamNode = static_cast<nGeometryNode*>(sceneNode)->GetStreamGeometry();
        if (streamNode)
        {
            if (streamNode->IsA(instShapeNodeClass))
            {
                nMesh2* instMesh = static_cast<nInstShapeNode*>(streamNode)->GetMeshResource();
                if (instMesh && !visitedMeshes.Find(instMesh))
                {
                    visitedMeshes.Append(instMesh);
                }
            }
        }
        else
        {
            //retrieve mesh stats from shape node
            if (sceneNode->IsA(shapeNodeClass))
            {
                nMesh2* mesh = static_cast<nShapeNode*>(sceneNode)->GetMeshResource();
                if (mesh && !visitedMeshes.Find(mesh))
                {
                    visitedMeshes.Append(mesh);
                }
            }
        }
    }

    //get stats from child nodes
    for (nSceneNode* curChild = static_cast<nSceneNode*>(sceneNode->GetHead());
         curChild;
         curChild = static_cast<nSceneNode*>(curChild->GetSucc()))
    {
        this->GetDebugStatsFromNode(curChild);
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nLoadArea::StringToStatsCounter(const char *counterName)
{
    for (int index = 0; index < NumStatsCounters; ++index)
    {
        if (strcmp(statsCounterStrings[index], counterName) == 0)
        {
            return index;
        }
    }
    
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::ResetStatsCounters()
{
    //reset lists of elements already visited
    visitedClasses.Reset();
    visitedTextures.Reset();
    visitedMeshes.Reset();

    //reset counters
    for (int index = 0; index < NumStatsCounters; ++index)
    {
        this->statsCounters[index] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoadArea::SetAreaStatsCounter(const char *name, int value)
{
    int index = StringToStatsCounter(name);

    if (index != -1)
    {
        this->statsCounters[index] = value;
    }
}

//------------------------------------------------------------------------------
/**
    Create debug statistics (size and complexity, resource usage) for the
    area. Implement in subclasses to traverse all cells in the area.
*/
void
nLoadArea::CreateDebugStats()
{
    this->statsValid = true;
}

//------------------------------------------------------------------------------
/**
    save debug stats for the area, if they were generated
    this will be useful to retrieve stats from not yet loaded areas
*/
bool
nLoadArea::LogDebugStats()
{
    //if the area is loaded, compute the debug stats now;
    //if not, reuse the ones that were persisted
    if (this->IsLoaded())
    {
        this->CreateDebugStats();
    }

    if (this->statsValid)
    {
        nString msg;
        msg.Format("Stats for the loading area: '%s'\n\n", this->GetName());
        for (int index = 0; index < NumStatsCounters; ++index)
        {
            msg.Append(statsCounterStrings[index]);
            msg.Append("=");
            msg.Append(this->statsCounters[index]);
            msg.Append("\n");
        }

        n_message(msg.Get());

        //sort arrays of collected resources
        visitedTextures.QSort(TextureSorter);
        visitedMeshes.QSort(MeshSorter);

        int curWidth = -1;
        int curHeight = -1;
        int numTextures = 0;

        //log collected textures
        for (int index = 0; index < visitedTextures.Size(); ++index)
        {
            nTexture2* curTexture = visitedTextures[index];
            bool newSize = curTexture->GetWidth() != curWidth && curTexture->GetHeight() != curHeight;

            if ((newSize || index == visitedTextures.Size() - 1) && numTextures)
            {
                NLOG(loaderserver, (0 | nLoaderServer::NLOGSTATS,
                     "There were %d textures of width: %d, height: %d", numTextures, curWidth, curHeight))
                numTextures = 0;
            }

            NLOGCOND(loaderserver, newSize, (1 | nLoaderServer::NLOGSTATS,
                     "Texture width: %d, height: %d", curTexture->GetWidth(), curTexture->GetHeight()));

            curWidth = curTexture->GetWidth();
            curHeight = curTexture->GetHeight();

            NLOG(loaderserver, (1 | nLoaderServer::NLOGSTATS, "Texture filename: %s", curTexture->GetFilename().Get()))

            ++numTextures;
        }

        //log collected meshes, group in intervals of exponential sizes
        int numMeshes = 0;
        int prevSize = 0;
        int curSize = 20;
        
        for (int index = 0; index < visitedMeshes.Size(); ++index)
        {
            nMesh2* curMesh = visitedMeshes[index];

            bool newSize = int(curMesh->GetNumIndices() / 3) > curSize;
            if ((newSize || index == visitedMeshes.Size() - 1) && numMeshes)
            {
                NLOG(loaderserver, (0 | nLoaderServer::NLOGSTATS,
                     "There were %d meshes with %d to %d triangles", numMeshes, prevSize, curSize))

                prevSize = curSize;
                curSize *= 5;
                numMeshes = 0;
            }

            NLOG(loaderserver, (1 | nLoaderServer::NLOGSTATS,
                 "Mesh filename: %s, size: %d indices (%d triangles), vertexWidth: %d", 
                 curMesh->GetFilename().Get(), curMesh->GetNumIndices(), int(curMesh->GetNumIndices() / 3), curMesh->GetVertexWidth()))

            ++numMeshes;
        }
    }

    return this->statsValid;
}

//------------------------------------------------------------------------------
/**
    save debug stats for the area, if they were generated
    this will be useful to retrieve stats from not yet loaded areas
*/
void
nLoadArea::LoadDebugStats(const char *fileName)
{
    kernelServer->PushCwd(this);
    kernelServer->Load(fileName, false);
    kernelServer->PopCwd();

    this->statsValid = true;
}

//------------------------------------------------------------------------------
/**
    save debug stats for the area, if they were generated
    this will be useful to retrieve stats from not yet loaded areas
*/
void
nLoadArea::SaveDebugStats(const char *fileName)
{
    if (!this->statsValid)
    {
        return;
    }

    nFileServer2* fileServer = kernelServer->GetFileServer();

    //create the "level:areas/debug/" path if it doesn't exist
    nString debugPath(fileName);
    if (!fileServer->DirectoryExists(debugPath.ExtractDirName()))
    {
        fileServer->MakePath(debugPath.ExtractDirName());
    }

    nPersistServer *ps = kernelServer->GetPersistServer();
    n_assert(ps);

    // save area stats
    nCmd* cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, fileName))
    {
        int index;
        for (index = 0; index < NumStatsCounters; ++index)
        {
            ps->Put(this, 'MSIS', statsCounterStrings[index], this->statsCounters[index]);
        }

        ps->EndObject(true);
    }
}

#endif //NGAME
