#ifndef N_NFLOATMAP_H
#define N_NFLOATMAP_H
/*-----------------------------------------------------------------------------
    @file nfloatmap.h
    @class nFloatMap
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre
    @author Juan Jose Luna

    @brief nFloatMap Class to handle generic maps of a float channel

    nFloatMap is used for rendering terrains, but it must be independent of 
    the terrain rendering algorithm. It also incorporates methods for heightmap 
    editing.

    A floatmap is an image / map where heights are stored (also called hexels).
    The hexels are used to get the y coordinates. x and z coordinates are
    represented by a parametrized regular grid, which are used to access the
    heightmap image and extract the height value.

    There are two coordinate spaces in the floatmap:

    * Heightmap coordinates. (x, z) are integer (gridScale not applied), and 
    height is represented by a float in the range 0..1 
    (unscaled coordinates)

    * Local coordinates. float x, float y, float z. All scales and offsets are
    applied. There is a simple scale and offset applied to the height 
    (scaled coordinates)

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "util/nstring.h"
#include "resource/nresource.h"
#include "mathlib/rectangle.h"
#include "mathlib/bbox.h"
#include "mathlib/triangle.h"

//------------------------------------------------------------------------------
class nTexture2;

//------------------------------------------------------------------------------
/**
    Class nFloatMap stores a rectangular map of float values
*/
class nFloatMap : public nResource
{

public:
    /// usage flags
    enum Usage
    {
        CreateEmpty = (1<<0),   // don't load from disk, instead create empty floatmap
    };

    /// constructor
    nFloatMap();

    /// initialize, create and set to baseLevel in heightmap coordinates
    void FillHeight(float baseLevel);

    /// Load float map resource
    virtual bool LoadResource();
    /// Unload float map resource
    virtual void UnloadResource();
    /// check if asynchronous loading is allowed
    virtual bool CanLoadAsync() const;
    /// load floatmap properties
    bool LoadProperties();

    /// alloc the buffer containing the nfloatmap hexels
    bool Alloc();

    /// load a float map from H16 format
    bool LoadFloatMapH16();
    /// load a float map from text T16 format
    bool LoadFloatMapT16();
    /// Loads height data from a graphic image file
    bool LoadFromTexture();

    /// save a floatmap
    virtual bool Save();

    /// save a float map in H16 format
    bool SaveFloatMapH16();
    /// save a float map in text T16 format
    bool SaveFloatMapT16();

    /// set combination of usage flags
    void SetUsage(ushort useFlags);
    /// get usage flags combination
    ushort GetUsage() const;

    /// set grid size (number of hexels in each direction)
    void SetSize(int size);
    /// get size (number of hexels in each direction) of the terrain
    int GetSize() const;

    /// set grid scale factor (grid interval between vertices)
    void SetGridScale(float scale);
    /// get grid scale factor (grid interval between vertices)
    float GetGridScale() const;

    /// set height scale factor
    void SetHeightScale(float scale);
    /// get height scale factor
    float GetHeightScale() const;
    
    /// set height base offset
    void SetHeightOffset(float offset);
    /// get height base offset
    float GetHeightOffset() const;

    // Get max height of the map
    float GetMaxHeight() const;

    /// Get floatmap side size in meters
    float GetExtent();

    /// get height in HeightMap coordinates (range 0..1)
    float GetHeightHC(int x, int z) const;
    /// set height in heightMmap coordinates
    void SetHeightHC(int x, int z, float hc);
    /// get height in local coordinates (local heightmap coordinates)
    float GetHeightLC(int x, int z) const;
    /// set height in local coordinates
    void SetHeightLC(int x, int z, float hc);

    /// Transform H to L coordinates
    float HC2LC(float hc) const;
    /// Transform L to H coordinates
    float LC2HC(float lc) const;
    /// Transform rectangle in local coordinates to heightmap coordinates
    void LC2HC(const rectangle & r, int & x1, int & z1, int & x2, int & z2) const;

    /// Get the height and normal in local coordinates given a 2D point in float
    bool GetHeightNormal(float x, float z, float& h, vector3& normal);
    /// Get only the height in local coordinates given a 2D point
    bool GetHeight (float x, float z, float& h) const;
    /// Get normal for heightmap given a point (ix, iz) in integer
    void GetNormal(int ix, int iz, vector3 * normal);

    /// Get surface area rectangle in local coordinates
    void GetSurfaceLC(rectangle& r) const;

    /// Get bounding boxes
    void CalculateBoundingBoxHC(int x0, int z0, int x1, int z1, bbox3 & boundingBox) const;
    /// Get bounding box
    void CalculateBoundingBoxLC(const rectangle& r, bbox3 & boundingBox) const;

    /// Intersect floatmap with a ray    
    bool Intersect(const line3& line, float& t, int& x, int& z) const;

    /// Gets the heightMap buffer
	float *GetHeightMap(void) const;

    /// get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();

    /// set dirty state
    void SetDirty();
    /// return true if resource is dirty
    bool IsDirty();

    /// save floatmap properties
    bool SaveProperties();

    /// Check intersection of a line with two triangles of the terrain
    bool IntersectWithTwoTriangles(const line3& line, float& t, int x, int z) const;

protected:

    /// destructor
    virtual ~nFloatMap();

    /// dealloc the heightmap buffer
    void Dealloc();

    /// Adjust the size of the floatmap/heightmap
    virtual int AdjustHeightMapSize(int srcsize);

    /// get the filename of the properties file for the float map
    nString GetPropertiesFilename();

    /// usage flags
    ushort usage;
    /// Number of map points
    ushort size;
    /// Grid scale
    float gridScale;
    /// Height offset
    float heightOffset;
    /// Height scaling
    float heightScale;
    /// Max height, equal to heightOffset + heightScale
    float maxHeight;

    /// buffer to heightmapinfo
    float * heightMap;

    // true when floatmap has been changed after loading
    bool dirty;
};

//------------------------------------------------------------------------------
/**
    @brief Return the height in position (x,z) in local coordinates.
    @return the height in position (x,z) in local coordinates.
*/
inline
float
nFloatMap::GetHeightLC(int x, int z) const
{
    n_assert(x >= 0 && x < this->size);
    n_assert(z >= 0 && z < this->size);
    return this->heightMap[z * this->size + x];
}

//------------------------------------------------------------------------------
/**
    @brief Get the normal in local coordinates given a float 2D point
    @param ix X coordinate of 2d point
    @param iz Z coordinate of 2d point
    @param normal Returned height value in LC on success, trash otherwise    
*/
inline
void 
nFloatMap::GetNormal(int ix, int iz, vector3 * normal)
{
/*
    float h = this->GetHeightLC(ix, iz);
    float xp = ((ix + 1) >= this->size) ? h : this->GetHeightLC(ix + 1, iz);
    float xm = (ix == 0) ? h : this->GetHeightLC(ix - 1, iz);
    float zp = ((iz + 1) >= this->size) ? h : this->GetHeightLC(ix, iz + 1);
    float zm = (iz == 0) ? h : this->GetHeightLC(ix, iz - 1);

    // calculate two vectors
    //vector3 v1(2.0f, xp - xm, 0.0f);
    //vector3 v2(0.0f, zp - zm, 2.0f);
    //vector3 normal = v1 * v2;

    normal->set(xm - xp, 2.0f, zm - zp);
*/
    int z1 = (iz + 1) % this->GetSize();
    int z_1 = (iz + this->GetSize() - 1) % this->GetSize();
    int x1 = (ix + 1) % this->GetSize();
    int x_1 = (ix + this->GetSize()- 1) % this->GetSize();

    float s00 = this->GetHeightLC( x_1, z_1 );
    float s01 = this->GetHeightLC( ix, z_1 );
    float s02 = this->GetHeightLC( x1, z_1 );

    float s10 = this->GetHeightLC( x_1, iz );
    float s12 = this->GetHeightLC( x1, iz );

    float s20 = this->GetHeightLC( x_1, z1 );
    float s21 = this->GetHeightLC( ix, z1 );
    float s22 = this->GetHeightLC( x1, z1 );

    // Compute -dx using Sobel:
    //
    // 1 0 -1
    // 2 0 -2
    // 1 0 -1 
    float sobelX = s00 + 2.0f * s10 + s20 - s02 - 2.0f * s12 - s22;

    // Compute -dy using Sobel:
    //
    //  1  2  1
    //  0  0  0
    // -1 -2 -1 
    float sobelZ = s00 + 2.0f * s01 + s02 - s20 - 2 * s21 - s22;

    normal->set( sobelX,  8.0f * this->GetGridScale()  , sobelZ);
}

//------------------------------------------------------------------------------
#endif N_NFLOATMAP_H
