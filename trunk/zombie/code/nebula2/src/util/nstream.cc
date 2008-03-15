//------------------------------------------------------------------------------
//  @file nstream.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "util/nstream.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
nstream::nstream() : 
    write (true),
    buffer(0),
    index (0),
    size  (0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nstream::~nstream()
{
    this->Reset();
}

//------------------------------------------------------------------------------
/**
    Reset 
*/
void
nstream::Reset()
{
    if ( this->buffer )
    {
        n_delete (this->buffer);
        this->buffer = 0;
        this->index = 0;
        this->write = true;
    }
}

//------------------------------------------------------------------------------
/**
    UpdateInt
*/
void
nstream::UpdateInt (int& value)
{
    if ( this->write )
    {
        this->SetInt (value);
    }
    else
    {
        value = this->GetInt();
    }
}

//------------------------------------------------------------------------------
/**
    UpdateFloat
*/
void
nstream::UpdateFloat (float& value)
{
    if ( this->write )
    {
        this->SetFloat (value);
    }
    else
    {
        value = this->GetFloat();
    }
}

//------------------------------------------------------------------------------
/**
    UpdateByte
*/
void
nstream::UpdateByte (char& value)
{
    if ( this->write ) 
    {
        this->SetByte (value);
    }
    else
    {
        value = this->GetByte();
    }
}

//------------------------------------------------------------------------------
/**
    UpdateBool
*/
void
nstream::UpdateBool (bool& value)
{
    if ( this->write ) 
    {
        this->SetBool (value);
    }
    else
    {
        value = this->GetBool();
    }
}

//------------------------------------------------------------------------------
/**
    UpdateString
*/
void
nstream::UpdateString (char* value)
{
    if ( this->write )
    {
        this->SetString (value);
    }
    else
    {
        this->GetString (value);
    }
}

//------------------------------------------------------------------------------
/**
    UpdateString
*/
void
nstream::UpdateString (const char * value)
{
    n_assert( this->write );

    if( this->write )
    {
        this->SetString (value);
    }
}

//------------------------------------------------------------------------------
/**
    UpdateVector3
*/
void
nstream::UpdateVector3 (vector3& value)
{
    if ( this->write )
    {
        this->SetVector3 (value);
    }
    else
    {
        this->GetVector3 (value);
    }
}

//------------------------------------------------------------------------------
/**
    UpdateVector4
*/
void
nstream::UpdateVector4 (vector4& value)
{
    if ( this->write )
    {
        this->SetVector4 (value);
    }
    else
    {
        this->GetVector4 (value);
    }
}

//------------------------------------------------------------------------------
/**
    UpdateSize
*/
void
nstream::UpdateSize (int size)
{
    if ( !this->buffer )
    {
        this->buffer = n_new (char)[INCREMENTAL_SIZE];
        this->size  = INCREMENTAL_SIZE;
        this->index = 0;
    }
    else
    if ( this->size <= this->index + size )
    {
        this->size += INCREMENTAL_SIZE;
        n_realloc (this->buffer, this->size);
    }
}

//------------------------------------------------------------------------------
/**
    SetInt
*/
void
nstream::SetInt (int value)
{
    int increment = sizeof(int);
    this->UpdateSize (increment);

    char* p = this->buffer + this->index;
    
    memcpy (p, (char*)&value, increment);
    this->index += increment;
}

//------------------------------------------------------------------------------
/**
    GetInt
*/
int
nstream::GetInt()
{
    int increment = sizeof(int);
    char* p = this->buffer + this->index;
    int* value = (int*)p;
    
    this->index += increment;
    
    return *value;
}

//------------------------------------------------------------------------------
/**
    SetFloat
*/
void
nstream::SetFloat (float value)
{
    int increment = sizeof(float);
    this->UpdateSize (increment);

    char* p = this->buffer + this->index;

    memcpy (p, (char*)&value, increment);
    this->index += increment;
}

//------------------------------------------------------------------------------
/**
    GetFloat
*/
float
nstream::GetFloat()
{
    int increment = sizeof(float);
    char* p = this->buffer + this->index;
    float* value = (float*)p;

    this->index += increment;

    return *value;
}

//------------------------------------------------------------------------------
/**
    SetByte
*/
void
nstream::SetByte (char value)
{
    int increment = sizeof(char);
    this->UpdateSize (increment); 

    char* p = this->buffer + this->index;
   
    memcpy (p, (char*)&value, increment);
    this->index += increment;
}

//------------------------------------------------------------------------------
/**
    GetByte
*/
char
nstream::GetByte()
{
    int increment = sizeof(char);    
    char* p = (char*) this->buffer + this->index;
    char* value = (char*)p;

    this->index += increment;

    return *value;
}

//------------------------------------------------------------------------------
/**
    SetBool
*/
void
nstream::SetBool (bool value)
{
    int increment = sizeof(bool);
    this->UpdateSize (increment); 

    char* p = this->buffer + this->index;
   
    memcpy (p, (char*)&value, increment);
    this->index += increment;
}

//------------------------------------------------------------------------------
/**
    GetBool
*/
bool
nstream::GetBool()
{
    int increment = sizeof(bool);    
    bool* p = (bool*) this->buffer + this->index;
    bool* value = (bool*)p;

    this->index += increment;

    return *value;
}

//------------------------------------------------------------------------------
/**
    SetString
*/
void
nstream::SetString (const char* value)
{
    int increment = static_cast<int> (strlen(value) + 1);
    this->UpdateSize (increment);

    this->SetInt (increment);

    char* p = this->buffer + this->index;

    memcpy (p, value, increment);
    this->index += increment;
}

//------------------------------------------------------------------------------
/**
    GetString
*/
void
nstream::GetString (char* value)
{
    int increment = this->GetInt();    
    const char* p = this->buffer + this->index;

    strcpy (value, p);

    this->index += increment;
}

//------------------------------------------------------------------------------
/**
    SetVector3
*/
void
nstream::SetVector3 (const vector3& value)
{
    this->SetFloat (value.x);
    this->SetFloat (value.y);
    this->SetFloat (value.z);
}

//------------------------------------------------------------------------------
/**
    GetVector3
*/
void 
nstream::GetVector3 (vector3& value)
{
   value.x = this->GetFloat();
   value.y = this->GetFloat();
   value.z = this->GetFloat();
}

//------------------------------------------------------------------------------
/**
    SetVector4
*/
void
nstream::SetVector4 (const vector4& value)
{
    this->SetFloat (value.x);
    this->SetFloat (value.y);
    this->SetFloat (value.z);
    this->SetFloat (value.w);
}

//------------------------------------------------------------------------------
/**
    GetVector4
*/
void
nstream::GetVector4 (vector4& value)
{
    value.x = this->GetFloat();
    value.y = this->GetFloat();
    value.z = this->GetFloat();
    value.w = this->GetFloat();
}

//------------------------------------------------------------------------------
/**
    GetBufferSize
*/
int
nstream::GetBufferSize() const
{
    return this->index;
}

//------------------------------------------------------------------------------
/**
    GetBuffer
*/
char*
nstream::GetBuffer() const
{
    return this->buffer;
}

//------------------------------------------------------------------------------
/**
    SetBuffer
*/
void
nstream::SetBuffer(int size, const char* buffer)
{
    this->Reset();
    if (size > 0)
    {
        this->UpdateSize (size);
        memcpy (this->buffer, buffer, size);
    }
}

//------------------------------------------------------------------------------
/**
    SetExternBuffer
*/
void
nstream::SetExternBuffer(int size, const char * buffer)
{
    this->size = size;
    this->buffer = const_cast<char*>( buffer );
}