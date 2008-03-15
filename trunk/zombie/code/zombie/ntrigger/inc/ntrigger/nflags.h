#ifndef N_FLAGS_H
#define N_FLAGS_H

//------------------------------------------------------------------------------
/**
    @class nFlags
    @ingroup NebulaTriggerSystem

    Bitfield of flags for any amount of bits.

    This class reserves enough space to store the desired number of bits and
    exposes bit operations to work with them as flags.

    (C) 2006 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
class nFlags
{
public:
    /// Default constructor
    nFlags();
    /// Destructor
    ~nFlags();
    /// Init the class to hold at least numFlags flags
    void Init( int numFlags );
    /// Set the value of a flag to true
    void EnableFlag( int flagIndex );
    /// Set the value of a flag to false
    void DisableFlag( int flagIndex );
    /// Set the value of a flag
    void SetFlag( int flagIndex, bool value );
    /// Return true if the specified flag is enabled, false otherwise
    bool IsFlagEnabled( int flagIndex ) const;

private:
    /// Smallest flags container unit
    typedef unsigned char Bitfield;

    /// Get the bitfield where the specified flag is in
    Bitfield& GetBitfield( int flagIndex ) const;
    /// Get a bitfield with only the specified flag enabled
    Bitfield GetBitfieldMask( int flagIndex ) const;

    /// Flags are contained in this array
    Bitfield* flags;
    /// Number of bits used to define the flag index within a bitfield
	static const unsigned int BitsPerLocalIndex = 2 + sizeof(Bitfield);
    /// Mask to get those bits that define the flag index within a bitfield
	static const Bitfield LocalIndexMask = (~0UL) >> ( 8*sizeof(unsigned long) - (2 + sizeof(Bitfield)) );

#ifndef __NEBULA_NO_ASSERT__
    int numFlags;
#endif
};

//------------------------------------------------------------------------------
/**
    Set the value of a flag to true
*/
inline
void nFlags::EnableFlag( int flagIndex )
{
    this->GetBitfield( flagIndex ) |= this->GetBitfieldMask( flagIndex );
}

//------------------------------------------------------------------------------
/**
    Set the value of a flag to false
*/
inline
void nFlags::DisableFlag( int flagIndex )
{
    this->GetBitfield( flagIndex ) &= ~this->GetBitfieldMask( flagIndex );
}

//------------------------------------------------------------------------------
/**
    Set the value of a flag
*/
inline
void nFlags::SetFlag( int flagIndex, bool value )
{
    if ( value )
    {
        this->EnableFlag( flagIndex );
    }
    else
    {
        this->DisableFlag( flagIndex );
    }
}

//------------------------------------------------------------------------------
/**
    Return true if the specified flag is enabled, false otherwise
*/
inline
bool nFlags::IsFlagEnabled( int flagIndex ) const
{
    n_assert( this->flags );
    n_assert( flagIndex >= 0 && flagIndex < this->numFlags );

    return ( this->GetBitfield( flagIndex ) & this->GetBitfieldMask( flagIndex ) ) != 0;
}

//------------------------------------------------------------------------------
/**
    Get the bitfield where the specified flag is in
*/
inline
nFlags::Bitfield& nFlags::GetBitfield( int flagIndex ) const
{
    n_assert( this->flags );
    n_assert( flagIndex >= 0 && flagIndex < this->numFlags );

    return this->flags[ flagIndex >> this->BitsPerLocalIndex ];
}

//------------------------------------------------------------------------------
/**
    Get a bitfield with only the specified flag enabled
*/
inline
nFlags::Bitfield nFlags::GetBitfieldMask( int flagIndex ) const
{
    return 1 << ( Bitfield(flagIndex) & this->LocalIndexMask );
}

#endif
