#ifndef N_NGEOMIPMAPVIEWPORT_H
#define N_NGEOMIPMAPVIEWPORT_H
/*-----------------------------------------------------------------------------
    @file ngeomipmapviewport.h
    @class nGeoMipMapViewPort
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    @brief class to handle per viewport/camera global geomipmap terrain 
    information.

    (C) 2004 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
class nViewport;
class nCamera2;

//------------------------------------------------------------------------------
/**
    Calculate and store per viewport/camera parameters.
*/
class nGeoMipMapViewport
{
public:
    /// constructor
    nGeoMipMapViewport();
    /// destructor
    ~nGeoMipMapViewport();

    void SetSceneGraph(nSceneGraph *, nEntityObject * entityObject);
    nSceneGraph * GetSceneGraph();

    bool IsFirstAttach();
    void Attach(int sumMaterialLOD);
    void AttachCell();
    bool Apply();
    void Render();
    void RenderCell();
    bool IsFirstRender();
    bool IsLastRender();

    /// Calculate C factor
    float CalculateC2Factor(nCamera2 & camera, nViewport & viewport);
    void UpdateViewport();
    /// Set minimum lod level (best)
    void SetMinLODLevel(int min);
    /// Return minimum lod level (best)
    int GetMinLODLevel() const;
    /// Set maximum lod level (worst)
    void SetMaxLODLevel(int max);
    /// Return maximum lod level (worst)
    int GetMaxLODLevel() const;

    /// Get C2 factor
    float GetC2(void) const;
    /// Get C2 factor inverted (1.0f / C2)
    float GetC2Inv(void) const;

    // Get camera position
    const vector3 & GetCameraPos(void) const;
    // Get camera pos in block coordinates as array of 3 integers (bx, by, bz)
    const int * GetCameraBlockPos() const;

    int GetNumAttached();
    int GetNumRendered();
    int GetNumAttachedCells();
    int GetNumRenderedCells();

protected:

    void CalculateCameraBlockPos(const vector3 & cameraPos);

    // scenegraph associated to the viewport
    nSceneGraph * sceneGraph;
    nEntityObject * callingEntity;

    bool attachStarted;
    bool renderStarted;
    int numAttached;
    int numAttachedCells;
    int numRendered;
    int numRenderedCells;

    ncTerrainGMMClass * terrainGMMClass;

    /// C squared constant
    float C2;
    /// 1.0f / C2
    float C2Inv;
    /// camera position
    vector3 cameraPos;
    /// block camera position
    int cameraBlockPos[3];
    /// minimum LOD level (best)
    int minLOD;
    /// maximum LOD level (worst)
    int maxLOD;

};

//------------------------------------------------------------------------------
/**
    Get C squared factor
*/
inline
float 
nGeoMipMapViewport::GetC2(void) const
{
    return this->C2;
}

//------------------------------------------------------------------------------
/**
    Get C squared factor
*/
inline
float 
nGeoMipMapViewport::GetC2Inv(void) const
{
    return this->C2Inv;
}

//------------------------------------------------------------------------------
/**
    Get C squared factor
*/
inline
const vector3 &
nGeoMipMapViewport::GetCameraPos(void) const
{
    return this->cameraPos;
}

//------------------------------------------------------------------------------
/**
    Set minimum LOD level to render for this viewport ()
*/
inline
void
nGeoMipMapViewport::SetMinLODLevel(int min)
{
    this->minLOD = min;
}

//------------------------------------------------------------------------------
/**
    @return minimum LOD level to render for this viewport ()
*/
inline
int
nGeoMipMapViewport::GetMinLODLevel() const
{
    return this->minLOD;
}

//------------------------------------------------------------------------------
/**
    Set maximum LOD level to render for this viewport ()
*/
inline
void
nGeoMipMapViewport::SetMaxLODLevel(int max)
{
    this->maxLOD = max;
}

//------------------------------------------------------------------------------
/**
    @return maximum LOD level to render for this viewport ()
*/
inline
int
nGeoMipMapViewport::GetMaxLODLevel() const
{
    return this->maxLOD;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGeoMipMapViewport::GetNumAttached()
{
    return this->numAttached;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGeoMipMapViewport::GetNumRendered()
{
    return this->numRendered;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGeoMipMapViewport::GetNumAttachedCells()
{
    return this->numAttachedCells;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGeoMipMapViewport::GetNumRenderedCells()
{
    return this->numRenderedCells;
}

//------------------------------------------------------------------------------
#endif
