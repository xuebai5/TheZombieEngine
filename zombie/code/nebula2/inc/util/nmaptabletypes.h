#ifndef N_NMAPTABLETYPES_H
#define N_NMAPTABLETYPES_H
//------------------------------------------------------------------------------
/**
   @file nmaptabletypes.h
   @brief Declaration of most common nMapTable 
   @brief $Id$ 
   
   This file is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#include "util/nmaptable.h"
#include "util/nstring.h"
#include "util/natom.h"

//------------------------------------------------------------------------------
inline
unsigned int 
nStringHash(const nString & name)
{
    unsigned int val = 0;
    const int length = name.Length();
    const char * string = name.Get();
    for ( int i = 0; i < length ; ++i )
    {
        val ^= string[i] << ( (i & 0x07) << 2 );
    }
    return val;
}

//------------------------------------------------------------------------------
struct nMapTablenStringHash
{
    unsigned int operator()(const nString & op) const
    {
        return nStringHash(op);
    }    
};

struct nMapTablenStringCmp
{
public:
    bool Less(const nString& elm0, const nString& elm1) const { return strcmp(elm0.Get() , elm1.Get() ) < 0; } 
    bool Equal( const nString& elm0, const nString& elm1) const { return elm0 == elm1; } 
};

//------------------------------------------------------------------------------
struct nMapTableVoidPtrHash
{
    unsigned int operator()(const void *op) const
    {
        unsigned int hash= static_cast<unsigned int>(size_t(op));
        unsigned int val =  hash % 97;
        hash /=97;
        hash ^= val << 24;
        return hash;
    }    
};

//------------------------------------------------------------------------------
struct nMapTablenAtomHash
{
    unsigned int operator()( const nAtom & op ) const
    {
        return op.AsKey();
    }
};

struct nMapTablenAtomCmp
{
    bool Less( const nAtom & elm0, const nAtom & elm1 ) const { 
        return ( strcmp( elm0.AsChar(), elm1.AsChar() ) < 0 ); }
    bool Equal( const nAtom & elm0, const nAtom & elm1 ) const { return ( elm0 == elm1 ); }
};

//------------------------------------------------------------------------------
template<class Data> class nMapTableTypes
{
public:
    typedef typename nMapTable<Data> Int;
    typedef typename nMapTable<Data, nString, nMapTablenStringHash, nMapTablenStringCmp> NString;
    typedef typename nMapTable<Data, void *, nMapTableVoidPtrHash> VoidPtr;
    typedef typename nMapTable<Data, nAtom, nMapTablenAtomHash, nMapTablenAtomCmp> NAtom;
};

//------------------------------------------------------------------------------
#endif//NMAPTABLETYPES_H
