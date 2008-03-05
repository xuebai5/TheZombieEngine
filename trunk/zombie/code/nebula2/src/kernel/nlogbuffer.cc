//------------------------------------------------------------------------------
//  nlogbuffer.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nlogbuffer.h"

//------------------------------------------------------------------------------
/*
*/
nLogBuffer::nLogBuffer( int maxSize ):
    maxSize(maxSize),
    size(0)
{}

//------------------------------------------------------------------------------
/*
*/
nLogBuffer::~nLogBuffer()
{
    Clear();
}

//------------------------------------------------------------------------------
/*
*/
void
nLogBuffer::AddLine( const char* log )
{
    this->logList.AddTail( n_new(nStrNode(log)) );
    ++(this->size);
    if ( this->size > maxSize )
    {
        n_delete( this->logList.RemHead() );
        --(this->size);
    }
}

//------------------------------------------------------------------------------
/*
*/
void
nLogBuffer::Clear()
{
    while ( !this->logList.IsEmpty() )
    {
        n_delete( this->logList.RemHead() );
    }
    this->size = 0;
}
