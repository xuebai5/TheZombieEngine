#ifndef N_TAG_H
#define N_TAG_H
//------------------------------------------------------------------------------
/**
    @class nTag
    @ingroup NebulaDataTypes
    @author Zombie     
    @brief A tag to symbolize a string.

    (C) Conjurer Services, S.A.    
*/

#include "kernel/ntypes.h"
#include "kernel/ncrc.h"

//------------------------------------------------------------------------------
class nTag 
{
public:
    /// default constructor
    nTag();

    /// constructor by const char
    nTag( const char* txt );

    /// constructor by nString
    nTag( const nString& txt );

    /// copy constructor
    nTag( const nTag& tag );

    /// constructor by crc
    nTag( const uint crc );

    /// assignament operator
    nTag& operator = ( const nTag& tag );

    /// assignament operator by crc
    nTag& operator = ( const uint crc );

    /// operator equal
    bool operator == ( const nTag& tag ) const;

    /// operator different
    bool operator != ( const nTag& tag ) const;

    /// operator to be used with key maps
    int KeyMap() const;

//#ifndef NGAME
    /// recovers the text string
    const nString& GetText() const;
//#endif

private:

    /// CRC result
    uint crc;      

    /// computes CRC
    uint ComputeCRC( const char* txt );

//#ifndef NGAME
    /// stores the text for debug pourposes
    nString text;
#ifndef NGAME
    /// sets a flag to check that the CRC tag's algorithem works fine.
    static const bool CheckCRC = false;
#endif

    /// a CRC checker class
    static nCRC crcChecker;
};

//------------------------------------------------------------------------------
/**
    Dafault constructor
*/
inline
nTag::nTag() : crc(0)
{   
#ifndef NGAME
    this->text = "";
#endif
}

//------------------------------------------------------------------------------
/**
    Constructor by nString
*/
inline
nTag::nTag( const nString& txt ) 
{   
    this->crc = this->ComputeCRC( txt.Get() );

#ifndef NGAME
    this->text = txt;
#endif
}

//------------------------------------------------------------------------------
/**
    Copy constructor
*/
inline
nTag::nTag( const nTag& tag )
{
    this->crc = tag.crc;

#ifndef NGAME
    this->text = tag.text;
#endif
}

//------------------------------------------------------------------------------
/**
    Constructor by crc
*/
inline
nTag::nTag( const uint crc )
{
    this->crc = crc;
#ifndef NGAME
    this->text = "";
#endif
}

//------------------------------------------------------------------------------
/**
    Assignament operator
*/
inline
nTag& nTag::operator = ( const nTag& tag )
{
    this->crc = tag.crc;
#ifndef NGAME
    this->text = tag.text;
#endif
    return *this;
}

//------------------------------------------------------------------------------
/**
    Assignament operator by crc
*/
inline
nTag& nTag::operator = ( const uint crc )
{
    this->crc = crc;
#ifndef NGAME
    this->text = "";
#endif
    return *this;
}

//------------------------------------------------------------------------------
/**
    Operator equal
*/
inline
bool nTag::operator == ( const nTag& tag ) const
{
#ifndef NGAME
#ifndef __NEBULA_NO_ASSERT__
    bool check(CheckCRC);

    if( check )
    {
        bool txtResult( tag.text == this->text );
        bool crcResult( tag.crc == this->crc );
        n_assert( txtResult == crcResult );
    }
#endif
#endif
    return this->crc == tag.crc;
}

//------------------------------------------------------------------------------
/**
    Operator different
*/
inline
bool nTag::operator != ( const nTag& tag ) const
{
#ifndef NGAME
#ifndef __NEBULA_NO_ASSERT__
    bool check(CheckCRC);

    if( check )
    {
        bool txtResult( tag.text != this->text );
        bool crcResult( tag.crc != this->crc );
        n_assert( txtResult == crcResult );
    }
#endif
#endif
    return this->crc != tag.crc;
}

//------------------------------------------------------------------------------
/**
    Computes CRC
*/
inline
uint nTag::ComputeCRC( const char* txt )
{
    return crcChecker.ChecksumNullTerminated( txt );
}

//------------------------------------------------------------------------------
/**
    Returns a key map.
*/
inline
int nTag::KeyMap() const
{
    return int(this->crc);
}

//------------------------------------------------------------------------------
/**
    Constructor by const char.
*/
inline
nTag::nTag( const char* txt )
{
    this->crc = this->ComputeCRC( txt );

#ifndef NGAME
    this->text = txt;
#endif    
}

//#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Recovers the text string.
*/
inline
const nString& nTag::GetText() const
{
    return this->text;
}
//#endif

#endif
