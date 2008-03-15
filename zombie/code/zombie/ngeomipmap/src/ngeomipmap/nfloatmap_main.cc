//------------------------------------------------------------------------------
//  nfloatmap_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "loki/static_check.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nFloatMap, "nresource");

//------------------------------------------------------------------------------
/**
    @brief Allocate memory buffer for the heightmap data. A previous call to SetSize is 
    required.
    @return true if succesful, otherwise false
*/
bool 
nFloatMap::Alloc()
{
    n_assert(!this->IsLoaded());
    n_assert(this->size > 0);
    n_assert(this->heightMap == NULL);

    // allocate buffer
    this->heightMap = n_new_array(float, this->size * this->size);

    // make dirty
    this->SetDirty();

    return (0 != this->heightMap);
}

//------------------------------------------------------------------------------
/**
    @brief Free the memory buffer used for floatmap data.
*/
void 
nFloatMap::Dealloc()
{
    if (this->heightMap)
	{
        n_delete_array(this->heightMap);
        this->heightMap = 0;
        this->SetState(Unloaded);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Initialize all height values in the heightmap to the given height in LC
    @param baseLevel Height to set floatmap to.
*/
void 
nFloatMap::FillHeight(float baseLevel)
{
    n_assert(this->heightMap);
    n_assert(this->size > 0);

    if (this->IsLoaded())
	{
        for(int i = 0;i < (this->size * this->size); i++)
		{
            this->heightMap[i] = baseLevel;
        }

        // make dirty
        this->SetDirty();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Default constructor
*/
nFloatMap::nFloatMap() :
    usage(0),
    size(0), 
    gridScale(1.0f), 
    heightOffset(0.0f), 
    heightScale(1.0f),
    maxHeight(1.0f),
    heightMap(0),
    dirty(false)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
    @brief Destructor
*/
nFloatMap::~nFloatMap()
{
   if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set the size of the terrain. For example if size is 1024, the heightmap 
    allocated will be 1024x1024.
    @param size The size
*/
void
nFloatMap::SetSize(int size)
{
    // check size is at least 1 x 1
    n_assert(size >= 1 && size < ushort(~0));
    this->size = static_cast<ushort> (size);
}

//------------------------------------------------------------------------------
/**
    @brief Get the floatmap size
    @return the terrain size
*/
int
nFloatMap::GetSize() const
{
    return this->size;
}
      
//------------------------------------------------------------------------------
/**
    @brief Return the height given heightmap coordinates
    @return the height in position (x,z) in heightmap coordinates.
*/
float
nFloatMap::GetHeightHC(int x, int z) const
{
    n_assert(x >= 0 && x < this->size);
    n_assert(z >= 0 && z < this->size);
    return this->LC2HC( this->heightMap[z * this->size + x] );
}

//------------------------------------------------------------------------------
/**
    @brief Sets the height in position (x,z) in heightmap coordinates.
*/
void
nFloatMap::SetHeightHC(int x, int z, float h)
{
    n_assert(x >= 0 && x < this->size);
    n_assert(z >= 0 && z < this->size);
    heightMap[z * this->size + x] = this->HC2LC( h );
}

//------------------------------------------------------------------------------
/**
    @brief Sets the height in position (x,z) in local coordinates.
*/
void
nFloatMap::SetHeightLC(int x, int z, float h)
{
    n_assert(x >= 0 && x < this->size);
    n_assert(z >= 0 && z < this->size);
    heightMap[z * this->size + x] = h;
}

//------------------------------------------------------------------------------
/**
    @brief Return height value transformed in local coordinates
    @param h height in heightmap coordinates
    @return height value transformed in local coordinates
*/
float
nFloatMap::HC2LC(float h) const
{
    return  h * this->heightScale + this->heightOffset;
}

//------------------------------------------------------------------------------
/**
    @brief Return height value transformed in heightmap coordinates
    @param h height in local coordinates
    @return height value transformed in heightmap coordinates
*/
float
nFloatMap::LC2HC(float h) const
{
    return  (h - this->heightOffset) / this->heightScale;
}

//------------------------------------------------------------------------------
/**
	@brief Convert rectangle r from local coordinates to heightmap coordinates.
    @param r in parameter rectangle in local coordinates
    @param x1 out parameter min x coordinate
    @param z1 out parameter min z coordinate
    @param x2 out parameter max x coordinate
    @param z2 out parameter max z coordinate
*/
void 
nFloatMap::LC2HC(const rectangle & r, int & x1, int & z1, int & x2, int & z2) const
{
    n_assert(r.v0.x < r.v1.x);
    n_assert(r.v0.y < r.v1.y);

    x1 = static_cast<int> ( floorf(r.v0.x / this->gridScale) );
    z1 = static_cast<int> ( floorf(r.v0.y / this->gridScale) );
    x2 = static_cast<int> ( ceilf(r.v1.x / this->gridScale) );
    z2 = static_cast<int> ( ceilf(r.v1.y / this->gridScale) );
}

//------------------------------------------------------------------------------
/**
    @brief Get the height and normal in local coordinates given a float 2D point
    @param x X coordinate of 2d point
    @param z Z coordinate of 2d point
    @param h Returned height value in LC on success, trash otherwise
    @param normal Returned normal vector (normalized) on success, unchanged otherwise
    @return true if the point lays in the heightmap, false otherwise
*/
bool 
nFloatMap::GetHeightNormal(float x, float z, float &h, vector3& normal)
{
    // Get point coordinates in HC and check if the point is inside
    float uX = x / GetGridScale();
    float uZ = z / GetGridScale();

    if ( uX >= 0.0f && uZ >= 0.0f && uX < ( this->GetSize() - 1 ) && uZ < ( this->GetSize() - 1 ) )
    {
        // Get point coordinates unitary relative to the pair of triangles
        int intX = int(uX);
        uX -= intX;
        int intZ = int(uZ);
        uZ -= intZ;

        triangle tri;

        // Check in wich of both triangles the point lays
        if ( uX + uZ < 1.0f ) {
            tri.set(vector3(0.0f, GetHeightLC(intX, intZ), 0.0f),
                    vector3(0.0f, GetHeightLC(intX, intZ + 1), 1.0f),
                    vector3(1.0f, GetHeightLC(intX + 1, intZ), 0.0f));
        } else
        {
            tri.set(vector3(1.0f, GetHeightLC(intX + 1, intZ), 0.0f),
                    vector3(0.0f, GetHeightLC(intX, intZ + 1), 1.0f),
                    vector3(1.0f, GetHeightLC(intX + 1, intZ + 1), 1.0f));
        }
        
        normal.set( tri.normal() );

        line3 line( vector3( uX, this->GetHeightOffset() + this->GetHeightScale() + 1.0f, uZ),
                    vector3( uX, this->GetHeightOffset() - 1.0f, uZ ) );
        if (tri.intersect( line, h ) )
        {
            h = line.ipol( h ).y;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Get the height and normal in local coordinates given a float 2D point
    @param x X coordinate of 2d point
    @param z Z coordinate of 2d point
    @param h Returned height value in LC on success, trash otherwise    
    @return true if the point lays in the heightmap, false otherwise
*/
bool 
nFloatMap::GetHeight (float x, float z, float &h) const
{
    // Get point coordinates in HC and check if the point is inside
    float uX = x / GetGridScale();
    float uZ = z / GetGridScale();

    if ( uX >= 0.0f && uZ >= 0.0f && uX < ( this->GetSize() - 1 ) && uZ < ( this->GetSize() - 1 ) )
    {
        // Get point coordinates unitary relative to the pair of triangles
        int intX = int(uX);
        uX -= intX;
        int intZ = int(uZ);
        uZ -= intZ;

        triangle tri;

        // Check in wich of both triangles the point lays
        if ( uX + uZ < 1.0f ) {
            tri.set(vector3(0.0f, GetHeightLC(intX, intZ), 0.0f),
                vector3(0.0f, GetHeightLC(intX, intZ + 1), 1.0f),
                vector3(1.0f, GetHeightLC(intX + 1, intZ), 0.0f));
        } else
        {
            tri.set(vector3(1.0f, GetHeightLC(intX + 1, intZ), 0.0f),
                vector3(0.0f, GetHeightLC(intX, intZ + 1), 1.0f),
                vector3(1.0f, GetHeightLC(intX + 1, intZ + 1), 1.0f));
        }

        line3 line( vector3( uX, this->GetHeightOffset() + this->GetHeightScale() + 1.0f, uZ),
            vector3( uX, this->GetHeightOffset() - 1.0f, uZ ) );
        if (tri.intersect( line, h ) )
        {
            h = line.ipol( h ).y;
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Get surface area rectangle in local coordinates. Returns the extents of the heightmap on the (x,z) plane.
    @param r rectangle to store surface parameters
*/
void
nFloatMap::GetSurfaceLC(rectangle & r) const
{
    n_assert2( this->size, "Information not available." );

    r.v0.x = 0;
    r.v0.y = 0;
    r.v1.x = this->gridScale * (this->size-1);
    r.v1.y = this->gridScale * (this->size-1);
}

//------------------------------------------------------------------------------
/**
    Calculate the bounding box for a rectangle of the heightmap
*/
void
nFloatMap::CalculateBoundingBoxHC(int x0, int z0, int x1, int z1, bbox3 & boundingBox) const
{
    n_assert(x0 >= 0 && x0 < this->GetSize());
    n_assert(z0 >= 0 && z0 < this->GetSize());
    n_assert(x1 >= 0 && x1 < this->GetSize());
    n_assert(z1 >= 0 && z1 < this->GetSize());
    n_assert(x0 <= x1 && z0 <= z1);

    float gridScale = this->GetGridScale();

    // calculates the bounding box by visiting each point of the heightmap
    boundingBox.begin_extend();
    for(int z = z0;z <= z1;z++)   
    {
        float vz = static_cast<float> (z) * gridScale;
        for(int x = x0;x <= x1;x++)    
        {
            float vx = static_cast<float> (x) * gridScale;
            float vy = this->GetHeightLC(x, z);
            boundingBox.extend(vx, vy, vz);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Calculate the bounding box for a rectangle given in LC
*/

void
nFloatMap::CalculateBoundingBoxLC(const rectangle & r, bbox3 & boundingBox) const
{
    int x0, z0, x1, z1;
    this->LC2HC(r, x0, z0, x1, z1);
    this->CalculateBoundingBoxHC(x0, z0, x1, z1, boundingBox);
}


//------------------------------------------------------------------------------
/**
    @brief Calculate the intersection with a line segment (in local heightmap coordinates)
    @param line The pick ray
    @param ipos Closest point of intersection if successful, trash otherwise
    @param x X coordinate of intersection if succesful, trash otherwise
    @param z Z coordinate of intersection if succesful, trash otherwise
    @return true if an intersection occurs
*/
bool
nFloatMap::Intersect(const line3& line, float& t, int &x, int &z) const
{
#if 1
    n_assert( this->size >= 2 );

    float gscale = this->gridScale;
    float hscale = this->heightScale;
    float hoffset = this->heightOffset;

    vector3 intersPos;

    // Get maximum bounding box and intersect the ray with it
    vector3 center( gscale * ( this->size - 1 ) / 2.0f, hoffset + hscale / 2.0f, gscale * ( this->size - 1) / 2.0f );
    vector3 extents( gscale * ( this->size - 1 ), hscale, gscale * ( this->size - 1) );
    bbox3 box( center, extents );

    // If ray doesn't intersect that box, it can't intersect the terrain
    if ( ! box.intersect( line, intersPos ) )
    {
        return false;
    }

    // Leave end of line the same
    vector3 lineEnd = line.b + line.m;

    // Setup variables for a scan of the heightmap squares that the line crosses
    int x0 = int( line.b.x  / gscale );
    int z0 = int( line.b.z / gscale );

    float lineT = 0.0f;
    float incLineT = gscale / ( 4.0f * vector3(line.m.x,0.0f,line.m.z).len() );

    bool firstTime = true;
    while ( firstTime || lineT < 1.0f )
    {
        firstTime = false;

        // If point is inside map...
        if ( x0 >= 0 && z0 >= 0 && x0 < size - 1 && z0 < size - 1 )
        {
            // If there is intersection...
            if ( IntersectWithTwoTriangles( line, t, x0, z0 ) )
            {
                // Return point and int coords of intersection
                x = x0;
                z = z0;
                return true;
            }
        }

        vector3 curPos = line.ipol( lineT );
        x0 = int( curPos.x / gscale );
        z0 = int( curPos.z / gscale );
        lineT += incLineT;
    }

    return false;

#else
    //Old implementation. Works, but slow.
    //Checks the ray against all the terrain triangles for intersections, and if any, returns the closer.

    n_assert(this->size >= 2);

    float gscale = this->gridScale;
    float hscale = this->heightScale;
    float hoffset = this->heightOffset;
    float vx0 = 0, vz0 = 0, vx1 = gscale , vz1 = gscale;

    float *p0, *p1, *p2, *p3;
    p0 = this->heightMap;
    p1 = p0 + 1;
    p3 = p0 + this->size;
    p2 = p3 + 1;

    triangle tri;

    float mint = FLT_MAX;
    float triIntersPos;
    bool foundInters = false;
    
    for (int j=0; j < this->size - 1; j++)
    {
        for (int i=0; i < this->size - 1; i++)
        {
            // First triangle
            tri.set(vector3(vx0, *p0 * hscale + hoffset, vz0),
                    vector3(vx0, *p3 * hscale + hoffset, vz1),
                    vector3(vx1, *p1 * hscale + hoffset, vz0));
            if (tri.intersect(line, triIntersPos))
            {
                foundInters = true;
                if (mint > triIntersPos)
                {
                    mint = triIntersPos;
                    x = i;
                    z = j;
                }
            }

            // Second triangle
            tri.set(vector3(vx1, *p1 * hscale + hoffset, vz0),
                    vector3(vx0, *p3 * hscale + hoffset, vz1),
                    vector3(vx1, *p2 * hscale + hoffset, vz1));
            if (tri.intersect(line, triIntersPos))
            {
                foundInters = true;
                if (mint > triIntersPos)
                {
                    mint = triIntersPos;
                    x = i+1;
                    z = j+1;
                }
            }
            p0++;
            p1++;
            p2++;
            p3++;

            vx0 = vx1;
            vx1 += gscale;

        }
        p0++;
        p1++;
        p2++;
        p3++;
        vx0 = 0;
        vx1 = gscale;
        vz0 = vz1;
        vz1 += gscale;
    }

    if (foundInters)
    {
        ipos = line.b + line.m * mint;

        return true;
    }
    return false;
#endif
}




//------------------------------------------------------------------------------
/**
    @brief Return the heightmap buffer
    @return the heightmap buffer
*/
float *
nFloatMap::GetHeightMap(void) const
{
    return this->heightMap;
}

//------------------------------------------------------------------------------
/**
    @brief Set the grid scale (distance between vertices in x and z coordinates)
    @param The grid scale 
*/
void 
nFloatMap::SetGridScale(float scale)
{
    n_assert( scale > 0 );
    this->gridScale = scale;
}

//------------------------------------------------------------------------------
/**
    @brief Return the grid scale (distance between vertices in x and z coordinates)
    @return the grid scale
*/
float 
nFloatMap::GetGridScale() const
{
    return this->gridScale;
}

//------------------------------------------------------------------------------
/**
    @brief Set the height scale.
    It's the factor used to multiply the hexel value in heightmap coordinates (between 0..1) and obtain the height value in local coordinates.
*/
void
nFloatMap::SetHeightScale(float scale)
{
    n_assert( scale > 0 );
    this->heightScale = scale;
    this->maxHeight = this->heightOffset + this->heightScale;
}
                            
//------------------------------------------------------------------------------
/**
    @brief Return the height scale.
    @return the height scale.
*/
float 
nFloatMap::GetHeightScale() const
{
    return this->heightScale;
}

//------------------------------------------------------------------------------
/**
    @brief Set the height offset.
    The height offset is a constant added to all heights and is applied in the transformation from heightmap coordinates to local coordinates.
*/
void
nFloatMap::SetHeightOffset(float offset)
{
    this->heightOffset = offset;
    this->maxHeight = this->heightOffset + this->heightScale;
}
                            
//------------------------------------------------------------------------------
/**
    @brief Return the height offset
    @return the height offset
*/
float 
nFloatMap::GetHeightOffset() const
{
    return this->heightOffset;
}

//------------------------------------------------------------------------------
/**
    @brief Get max height of the map
    @return Max height of the map
*/
float 
nFloatMap::GetMaxHeight() const
{
    return this->heightOffset + this->heightScale;
}

//------------------------------------------------------------------------------
/**
    @brief Get floatmap side size in meters
    @return Map side size in meters
*/
float
nFloatMap::GetExtent()
{
    return ( this->GetSize() - 1 ) * this->GetGridScale();
}

//------------------------------------------------------------------------------
/**
    @brief set combination of usage flags
*/
void 
nFloatMap::SetUsage(ushort useFlags)
{
    this->usage = useFlags;
}

//------------------------------------------------------------------------------
/**
    @brief Return usage flags combination
    @return usage flags combination
*/
ushort 
nFloatMap::GetUsage() const
{
    return this->usage;
}

//------------------------------------------------------------------------------
/**
    @brief Return space used by the resource
    @return space used by the resource
*/
int 
nFloatMap::GetByteSize()
{
    if (this->IsLoaded())
    {
        return this->size * this->size * sizeof(float);
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @brief Set dirty flag
*/
void 
nFloatMap::SetDirty()
{
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
    @brief Get dirty flag
    @return Dirty flag
*/
bool 
nFloatMap::IsDirty()
{
    return this->dirty;
}

//------------------------------------------------------------------------------
/**
    @brief Check intersection of a line with two triangles of the terrain
    @param line The pick ray
    @param t Ray parameter of closest point of intersection (if successful, trash otherwise)
    @param x X coordinate of the pair of triangles to be tested ( 0 <= x <= size - 2 )
    @param z Z coordinate of the pair of triangles to be tested ( 0 <= z <= size - 2 )
    @return true if an intersection occurs
*/
bool
nFloatMap::IntersectWithTwoTriangles(const line3& line, float& t, int x, int z) const
{
    n_assert( x >= 0 && x < size - 1 && z >= 0 && z < size - 1 )

    float vx0 = x * this->gridScale;
    float vx1 = ( x + 1 ) * this->gridScale;
    float vz0 = z * this->gridScale;
    float vz1 = ( z + 1 ) * this->gridScale;

    triangle tri;
    float mint = 2.0f;
    bool foundInters = false;

    // First triangle
    tri.set(vector3(vx0, GetHeightLC( x, z ), vz0),
            vector3(vx0, GetHeightLC( x, z + 1 ), vz1),
            vector3(vx1, GetHeightLC( x + 1, z ), vz0));
    
    if ( tri.intersect(line, t) )
    {
        foundInters = true;
        mint = t;
    }

    // Second triangle
    tri.set(vector3(vx1, GetHeightLC( x + 1, z ), vz0),
            vector3(vx0, GetHeightLC( x, z + 1), vz1),
            vector3(vx1, GetHeightLC( x + 1, z + 1), vz1));

    if ( tri.intersect(line, t) )
    {
        foundInters = true;
        if ( mint > t )
        {
            return true;
        }
    }

    return foundInters;
}

//------------------------------------------------------------------------------
/**
	@brief Loads height data from a graphic image (texture) file
    @param fileName Full path of the texture file
    
    This function loads a texture and gets the red channel as the height. Compressed
    dds formats and 16 bit float channels are not supported.

    If the heightmap is not valid, SetSize() will be called so that the texture can fit in the
    heightmap, and after that, Alloc() will be called. Otherwise the heightmap will be filled only
    with the portion covered by the texture.

*/
bool
nFloatMap::LoadFromTexture()
{
    nString extension = this->GetFilename().GetExtension();

    if ( extension != "dds" && extension != "bmp" && extension != "tga" && extension != "png" && extension != "ppm" && extension != "pfm" && extension != "dib" && extension != "jpg" && extension != "jpeg" )
    {
        return false;
    }

    nTexture2* newTexture = nGfxServer2::Instance()->NewTexture(0);
    newTexture->SetFilename(this->GetFilename().Get());

    if ( ! newTexture->Load() )
    {
        return false;
    }

    if (! newTexture->IsLoaded())
    {
        newTexture->Release();
        return false;
    }
    
    int format = newTexture->GetFormat();

    // Get the following format-dependent parameters
    // Mask and shift for the red channel
    int mask, shift;
    // Range of the red channel. 0.0f == 32 bit float value
    float range;
    // Pixel byte size
    int pixsize;
    switch (format)
    {
        case nTexture2::X8R8G8B8:
            //Intentionally left blank
        case nTexture2::A8R8G8B8:
            mask = 0x00FF0000;
            shift = 16;
            range = 256.0f;
            pixsize = 4;
            break;
        case nTexture2::R5G6B5:
            mask = 0x0000F800;
            shift = 11;
            range = 32.0f;
            pixsize = 2;
            break;
        case nTexture2::A1R5G5B5:
            mask = 0x00007C00;
            shift = 10;
            range = 32.0f;
            pixsize = 2;
            break;
        case nTexture2::A4R4G4B4:
            mask = 0x00000F00;
            shift = 8;
            range = 16.0f;
            pixsize = 2;
            break;
        case nTexture2::P8:
            mask = 0x000000FF;
            shift = 0;
            range = 256.0f;
            pixsize = 1;
        case nTexture2::G16R16:                     // 32 bit: 16 bit Green, 16 bit Red
            mask = 0x0000FFFF;
            shift = 0;
            range = 65536.0f;
            pixsize = 4;
            break;
        case nTexture2::R32F:                       // 32 bit float: red only
            mask = 0;
            shift = 0;
            range = 0.0f;
            pixsize = 4;
            break;
        case nTexture2::G32R32F:                    // 64 bit float: 32 bit red: 32 bit green
            mask = 0;
            shift = 0;
            range = 0.0f;
            pixsize = 8;
            break;
        case nTexture2::A32B32G32R32F:              // 128 bit float: 32 bit rgba each
            mask = 0;
            shift = 0;
            range = 0.0f;
            pixsize = 16;
            break;
        case nTexture2::A8:
            mask = 0x000000FF;
            shift = 0;
            range = 256.0f;
            pixsize = 1;
            break;
        default:
            // Texture format not supported
            return false;
    }

    nTexture2::LockInfo lockInfo;
    if (newTexture->Lock(nTexture2::ReadOnly, 0, lockInfo))
    {

        nuint8* line = (nuint8*) lockInfo.surfPointer;
        int ni = newTexture->GetWidth();
        int nj = newTexture->GetHeight();
        int remainder = lockInfo.surfPitch - ni * pixsize;

        if ( !this->IsLoaded() )
        {
            this->SetSize( max(ni, nj) );
            this->Alloc();
        }

        for (int j = 0; j < nj; j++) {
            for (int i = 0; i < ni; i++) {

                float h;

                if (mask)
                {
                    nint32 pixvalue;
                    if (pixsize == 1)
                    {
                        pixvalue = *line;
                    } else if (pixsize == 2)
                    {
                        pixvalue = *((nint16*) line);
                    } else
                    {
                        pixvalue = *((nint32*) line);
                    }

                    pixvalue = (pixvalue & mask) >> shift;
                    h = pixvalue / range;
                } else
                {
                    h = *((float*)line);
                }

                SetHeightHC(i, j, h);

                line += pixsize;
            }
            line += remainder;
        }

        newTexture->Unlock(0);

    } else
    {
        newTexture->Release();
        return false;
    }

    newTexture->Release();

    this->dirty = false;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFloatMap::LoadResource()
{
    n_assert(!this->IsLoaded());
    bool success = false;

    if (this->GetUsage() == nFloatMap::CreateEmpty)
    {
        success = this->Alloc();
        if ( success )
        {
            this->SetState(Valid);
            this->FillHeight(0.0f);
        }
        this->SetUsage(0);
    }
    else 
    {
        this->LoadProperties();
        if (this->GetFilename().CheckExtension("h16"))
        {
            success = this->LoadFloatMapH16();
        }
        else if (this->GetFilename().CheckExtension("t16"))
        {
            success = this->LoadFloatMapT16();
        }
        else 
        {
            success = this->LoadFromTexture();
        }

        if (success)
        {
            this->SetState(Valid);
        }
        else
        {
            this->SetState(Unloaded);
        }

        this->dirty = false;
    }

   return success;
}

//------------------------------------------------------------------------------
/**
    Unload the resource from memory
    @return true if the load was succesful, otherwise false
*/
void 
nFloatMap::UnloadResource()
{
    n_assert(this->IsLoaded());
    this->Dealloc();
}

//------------------------------------------------------------------------------
/**
*/
bool
nFloatMap::Save()
{
    bool success = false;

    if (!this->GetFilename().IsEmpty())
    {
        if (this->SaveProperties())
        {
            if (this->GetFilename().CheckExtension("h16"))
            {
                success = this->SaveFloatMapH16();
                this->SetReloadOnChange(false);
            }
            else if (this->GetFilename().CheckExtension("t16"))
            {
                success = this->SaveFloatMapT16();
                this->SetReloadOnChange(false);
            }
        }
    }

    // clean dirty state
    this->dirty = false;

    return success;
}

//------------------------------------------------------------------------------
/**
    The properties of a floatmap resource are stored as a n2 file with the
    same name as the texture data, but changing the extension to n2.
*/
nString
nFloatMap::GetPropertiesFilename()
{
    n_assert(!this->GetFilename().IsEmpty());
    nString filename = this->GetFilename().Get();
    filename.StripExtension();
    filename.Append(".n2");
    return filename;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFloatMap::LoadProperties()
{
    // read properties
    kernelServer->PushCwd(this);
    nObject * obj = kernelServer->Load(this->GetPropertiesFilename().Get(), false);
    kernelServer->PopCwd();
    return (0 != obj);
}

//------------------------------------------------------------------------------
/**
*/
bool
nFloatMap::SaveProperties()
{
    nPersistServer* ps = kernelServer->GetPersistServer();
    n_assert(ps);

    // save configuration with cmd BeginNewObjectConfig cmd
    bool retval = false;
    nCmd * cmd = ps->GetCmd(this, 'THIS');
    if (ps->BeginObjectWithCmd(this, cmd, this->GetPropertiesFilename().Get())) 
    {
        ps->Put(this, 'SEGS', this->GetGridScale());
        ps->Put(this, 'SEHS', this->GetHeightScale());
        ps->Put(this, 'SEHO', this->GetHeightOffset());
        ps->EndObject(true);
        retval = true;
    }

    return retval;
}

//------------------------------------------------------------------------------
/**
    Load heightmap format 16 bit file (hard-coded)
    @return true if the load was succesful, otherwise false
*/
bool
nFloatMap::LoadFloatMapH16()
{
    bool status = false;
    n_assert(0 == this->heightMap);

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (hmfile->Open( this->GetFilename().Get(), "r"))
    {
        // total file size
        int filesize = hmfile->GetSize();

        // resolution in x or z direction (assumed squared and 2 byte format)
        int srcsize = static_cast<int> ( (sqrtf(static_cast<float> (filesize/2))) );
        STATIC_CHECK(sizeof(ushort) == 2, ushort_assumed_to_be_16_bits);

        // adjust size
        int dstsize = this->AdjustHeightMapSize(srcsize);

        // load all in temporal buffer
        ushort * filedata = n_new_array(ushort, filesize);
        n_assert(filedata);

        // read file contents and close file
        int nbytes = hmfile->Read (filedata, filesize);
        hmfile->Close();

        // convert to local format if everything was read
        if (nbytes == filesize)
        {
            this->SetSize( dstsize );
            this->Alloc();
            for(int z = 0;z < dstsize;z++)
            {
                for(int x = 0;x < dstsize;x++)
                {
                    float val = static_cast<float> (filedata[min(z,srcsize-1) * srcsize + min(x,srcsize-1)]) / 65535.0f;
                    this->SetHeightHC(x, z, val);
                }                
            }

            status = true;
        }

        // deallocate buffer
        n_delete_array( filedata );
    }

    hmfile->Release();

    return status;
}

//------------------------------------------------------------------------------
/**
    Load heightmap format text 16 bit file (hard-coded)
    @return true if the load was succesful, otherwise false
*/
bool
nFloatMap::LoadFloatMapT16()
{
    bool status = true;
    n_assert(0 == this->heightMap);

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (!hmfile->Open( this->GetFilename().Get(), "r"))
    {
        status = false;
        return status;
    }

    // total file size
    int filesize = hmfile->GetSize();
    char * filebuffer = n_new_array(char, filesize + 1);
    n_assert(filebuffer);

    // read the whole file
    int nbytes = hmfile->Read(filebuffer, filesize);

    // release file resources
    hmfile->Close();
    hmfile->Release();

    if (nbytes != filesize)
    {
        n_delete_array(filebuffer);
        status = false;
        return status;
    }

    // make the buffer null terminated
    filebuffer[nbytes] = 0;

    // pointer to file contents
    char * ptr = filebuffer;

    // parse header: get size of the floatmap
    int size = 0;
    if (!strncmp(ptr, "size ", 5))
    {
        ptr += 5;
        if (sscanf(ptr, "%x", &size) > 0 && size <= 0)
        {
            status = false;
        }
        // next line
        ptr = strchr(ptr, '\n');
        if (ptr)
        {
            ptr++;
        }
    }
    else
    {
        status = false;
    }

    // get contents of the floatmap
    if (status)
    {
        // table for hex conversion
        int hex2val[256];

        // fill -1 for invalid values
        for(int i = 0;i < 256;i++)
        {
            hex2val[i] = -1;
        }

        // fill valid values for numbers
        hex2val['0'] = 0;
        hex2val['1'] = 1;
        hex2val['2'] = 2;
        hex2val['3'] = 3;
        hex2val['4'] = 4;
        hex2val['5'] = 5;
        hex2val['6'] = 6;
        hex2val['7'] = 7;
        hex2val['8'] = 8;
        hex2val['9'] = 9;
        hex2val['a'] = 10;
        hex2val['b'] = 11;
        hex2val['c'] = 12;
        hex2val['d'] = 13;
        hex2val['e'] = 14;
        hex2val['f'] = 15;
        hex2val['A'] = 10;
        hex2val['B'] = 11;
        hex2val['C'] = 12;
        hex2val['D'] = 13;
        hex2val['E'] = 14;
        hex2val['F'] = 15;

        // -2 for end of value
        hex2val['\n'] = -2;
        hex2val['\r'] = -2;

        this->SetSize( size );
        this->Alloc();
        for(int z = 0;status && z < size;z++)           
        {
            for(int x = 0;status && x < size;x++)
            {
                int val = 0;
                while(hex2val[*ptr] >= 0)
                {
                    val = val * 16 + hex2val[*ptr];
                    ptr++;
                }

                this->SetHeightHC(x, z, static_cast<float> (val / 65535.0f));

                if (hex2val[*ptr] == -2)
                {
                    ptr++;
                }
                else
                {
                    status = false;
                }
            }
        }
    }

    n_delete_array(filebuffer);

    return status;
}

//------------------------------------------------------------------------------
/**
    Save heightmap format 16 bit file (hard-coded)
    @return true if the load was succesful, otherwise false
*/
bool
nFloatMap::SaveFloatMapH16()
{
    bool status = false;

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (hmfile->Open( this->GetFilename().Get(), "w"))
    {
        // resolution in x or z direction (assumed squared and 2 byte format)
        int size = this->GetSize();

        // total file size
        int filesize = size * size * sizeof(ushort);
        STATIC_CHECK(sizeof(ushort) == 2, ushort_assumed_to_be_16_bits);

        // load all in temporal buffer
        ushort * filedata = n_new_array(ushort, filesize);
        n_assert(filedata);

        // convert to local format if everything was read
        for(int z = 0;z < size;z++)           
        {
            for(int x = 0;x < size;x++)
            {
                filedata[z * size + x] = static_cast<ushort> (this->GetHeightHC(x, z) * 65535.0f);
            }                
        }

        // read file contents and close file
        int nbytes = hmfile->Write(filedata, filesize);
        status = (nbytes == filesize);
        hmfile->Close();

        // deallocate buffer
        n_delete_array( filedata );
    }

    hmfile->Release();

    return status;
}

//------------------------------------------------------------------------------
/**
*/
bool
nFloatMap::SaveFloatMapT16()
{
    bool status = true;

    // get file server
    nFileServer2 * fs = kernelServer->GetFileServer();
    n_assert(fs);

    // translate filename through the file server
    nFile * hmfile = fs->NewFileObject();
    n_assert(hmfile);

    if (!hmfile->Open( this->GetFilename().Get(), "w"))
    {
        return false;
    }

    int mapSize = this->GetSize();
    int fileSize = ( mapSize * mapSize ) * 5 + 10;
    char * memBuf = n_new_array(char, fileSize );
    n_assert( memBuf );
    int totalBytes = 0;

    // save the string 'size xxxx' where xxxx is the size of the heightmap in hex notation
    memBuf[ totalBytes++ ] = 's';
    memBuf[ totalBytes++ ] = 'i';
    memBuf[ totalBytes++ ] = 'z';
    memBuf[ totalBytes++ ] = 'e';
    memBuf[ totalBytes++ ] = ' ';
    int mask = 0xF000;
    for (int d = 12; d >= 0; d -= 4)
    {
        char v = static_cast <char> ( ( mapSize & mask ) >> d );
        mask = mask >> 4;

        if ( v < 10 )
        {
            memBuf[ totalBytes++ ] = '0' + v;
        }
        else
        {
            memBuf[ totalBytes++ ] = 65 + v - 10;
        }
    }
    memBuf[ totalBytes++ ] = '\n';

    // save each height value as a 4 hex digit terminated by a CR
    for(int z = 0; z < mapSize; z++)
    {
        for(int x = 0; x < mapSize; x++)
        {
            nuint16 h = static_cast<nuint16> (this->GetHeightHC(x, z) * 65535.0f);

            // save the 4 characters of the 16 bit number in the file bufer
            mask = 0xF000;
            //for (int c = 0; c < 4; c++)
            for (int d = 12; d >= 0; d -= 4)
            {
                //int d = ( 4 * ( 3 - c ) );
                char v = static_cast <char> ( ( h & mask ) >> d );
                mask = mask >> 4;

                if ( v < 10 )
                {
                    memBuf[ totalBytes++ ] = '0' + v;
                }
                else
                {
                    memBuf[ totalBytes++ ] = 65 + v - 10;
                }
            }
            memBuf[ totalBytes++ ] = '\n';
        }                
    }

    N_IFDEF_ASSERTS(int nbytes =) 
    hmfile->Write( memBuf, totalBytes );
    n_assert( nbytes == totalBytes );

    // close file
    hmfile->Close();

    // release the file
    hmfile->Release();

    // release the buffer
    n_delete_array( memBuf );

    return status;
}

//------------------------------------------------------------------------------
/**
    Adjust load heightmap size
*/
int
nFloatMap::AdjustHeightMapSize(int srcsize)
{
    // check if is not power of 2 + 1
    return srcsize;

    /*
    @todo: must check that size is power of 2?
    // check if is not power of 2 + 1
    int dstsize = srcsize;
    if (((dstsize - 1) & (dstsize - 2)))
    {
        int val = static_cast<int> (ceilf(logf(static_cast<float> (srcsize)) / logf(2.0f)));
        dstsize = static_cast<int> (powf(2, static_cast<float> (val)) + 1);
    }
    return dstsize;
    */
}

//------------------------------------------------------------------------------
bool
nFloatMap::CanLoadAsync() const
{
    return true;
}

//------------------------------------------------------------------------------