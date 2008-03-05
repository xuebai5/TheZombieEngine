#ifndef N_NBYTEMAP_H
#define N_NBYTEMAP_H
//------------------------------------------------------------------------------
/**
    @file nbytemap.h
    @class nByteMap
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre
    @author Juan Jose Luna

    @brief nByteMap Class to handle generic maps of a byte channel

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "resource/nresource.h"

//------------------------------------------------------------------------------
/**
    Class nByteMap stores a rectangular map of byte values
*/
class nByteMap : public nResource
{

public:
    /// usage flags
    enum Usage
    {
        CreateEmpty = (1<<0),   // don't load from disk, instead create empty bytemap
    };

    /// constructor
    nByteMap();

    /// initialize, create and set to value
    void FillValue(nuint8 value);

    /// Load float map resource
    virtual bool LoadResource();
    /// Unload float map resource
    virtual void UnloadResource();
    /// check if asynchronous loading is allowed
    virtual bool CanLoadAsync() const;

    /// alloc the buffer containing the bytes
    bool Alloc();

    /// load a bytemap from H8 format
    bool LoadByteMapH8();
    /// load a bytemap from text T8 format
    bool LoadByteMapT8();
    /// Load a bytemap from a graphic image file
    bool LoadFromTexture();
    /// Load a bytemap through OpenIL library
    bool LoadByteMapOpenIL();
    
    /// save a bytemap
    virtual bool Save();

    /// save a bytemap in H8 format
    bool SaveByteMapH8();
    /// save a bytemap in text T8 format
    bool SaveByteMapT8();

    /// set combination of usage flags
    void SetUsage(ushort useFlags);
    /// get usage flags combination
    ushort GetUsage() const;

    /// set grid size (number of byte values in each direction)
    void SetSize(int size);
    /// get size (number of byte values in each direction)
    int GetSize() const;

    /// get byte value for coordinates x, y
    nuint8 GetValue(int x, int y) const;
    /// set byte value v for coordinates x, y
    void SetValue(int x, int y, nuint8 v);

    /// Gets the byteMap buffer
	nuint8 *GetByteMap(void) const;

    /// get an estimated byte size of the resource data (for memory statistics)
    virtual int GetByteSize();

    /// set dirty state
    void SetDirty();
    /// clean the dirty state
    void CleanDirty();
    /// return true if resource is dirty
    bool IsDirty();
    /// set user dirty state
    void SetUserDirty();
    /// clean user the dirty state
    void CleanUserDirty();
    /// return true if resource is dirty
    bool IsUserDirty();

#ifndef NGAME
    /// Calculate accumulator of all values in bytemap
    void CalculateAccumulator();
    /// return the current accumulator value for the bytemap
    unsigned int GetAccumulator() const;
    /// return the value for all bytemap
    bool  GetUniqueValue(nuint8& val);
#endif

protected:

    friend class nByteMapBuilder;

    /// destructor
    virtual ~nByteMap();

    /// dealloc the bytemap buffer
    void Dealloc();

    /// usage flags
    ushort usage;
    /// Number of map points
    ushort size;
    /// buffer with all the bytes in the map
    nuint8 * byteMap;

#ifndef NGAME
    /// Accumulates all the values in the bytemap
    unsigned int accumulator;
#endif

    // true when bytemap has been changed after loading
    bool dirty;
    bool userDirty;
};

//------------------------------------------------------------------------------
#endif N_NBYTEMAP_H
