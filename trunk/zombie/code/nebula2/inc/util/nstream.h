#ifndef N_STREAM_H
#define N_STREAM_H

//-------------------------------------------------------------------
/**
    @class nstream    

    defines an in/out stream for general purpose
*/

class nstream
{
public:
    /// Constructor
    nstream();
    /// Destructor
    ~nstream();

    /// Set type
    void SetWrite (bool write);
    /// Get type
    bool GetWrite() const;
    /// Reset the buffer
    void Reset();

    /// Updates differents types in/from the stream
    void    UpdateInt      (int& value);
    void    UpdateFloat    (float& value);
    void    UpdateByte     (char& value);
    void    UpdateBool     (bool& value);
    void    UpdateString   (char* value);
    void    UpdateString   (const char* value);
    void    UpdateVector3  (vector3& value);
    void    UpdateVector4  (vector4& value);

    // Plug & play your own type if you need it!

    // I don't need more types, but a way to persist the stream
    int GetBufferSize() const;
    char* GetBuffer() const;
    void SetBuffer (int size, const char* buffer);
    void SetExternBuffer (int size, const char* buffer);

private:
    enum 
    {
        INCREMENTAL_SIZE = 256,
    };

    // Update size of the buffer
    void    UpdateSize  (int size);

    // Auxiliary functions for read/write bits of information into/from the stream
    void    SetInt      (int value);
    int     GetInt      ();

    void    SetFloat    (float value);
    float   GetFloat    ();

    void    SetByte     (char value);
    char    GetByte     ();

    void    SetBool     (bool value);
    bool    GetBool     ();

    void    SetString   (const char* value);
    void    GetString   (char* value);

    void    SetVector3  (const vector3& value);
    void    GetVector3  (vector3& value);

    void    SetVector4  (const vector4& value);
    void    GetVector4  (vector4& value);

    /// Says if the stream is for reading or writting
    bool write;

    /// The buffer of the stream
    char* buffer;
    int   size;
    int   index;
};

//------------------------------------------------------------------------------
/**
    SetType
*/
inline
void
nstream::SetWrite (bool write)
{
    this->write = write;
    this->index = 0;
}

//------------------------------------------------------------------------------
/**
    GetWrite
*/
inline
bool
nstream::GetWrite() const
{
    return this->write;
}

#endif