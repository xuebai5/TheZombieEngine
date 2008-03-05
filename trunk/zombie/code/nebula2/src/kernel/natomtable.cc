//------------------------------------------------------------------------------
//  natomtable.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/natomtable.h"

nAtomTable* nAtomTable::Singleton = 0;

//------------------------------------------------------------------------------
/**
    nAtomTable constructor. Note that nAtomTable is a Singleton.
*/
nAtomTable::nAtomTable() :
    hashMap(2048, 4)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
    nAtomTable destructor.
*/
nAtomTable::~nAtomTable()
{
    n_assert(0 != Singleton);
    Singleton = 0;
}

