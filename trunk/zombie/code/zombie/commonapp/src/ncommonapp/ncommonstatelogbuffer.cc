#include "precompiled/pchncommonapp.h"
//------------------------------------------------------------------------------
//  ncommonstatelogbuffer.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonstatelogbuffer.h"
#include "ncommonapp/ncommonstate.h"
#include "napplication/napplication.h"

//------------------------------------------------------------------------------
/*
*/
nCommonStateLogBuffer::nCommonStateLogBuffer()
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
nCommonStateLogBuffer::~nCommonStateLogBuffer()
{
    // empty
}

//------------------------------------------------------------------------------
/*
*/
void
nCommonStateLogBuffer::AddLine( const char* log )
{
    if ( this->enabled )
    {
        nString curStateName = nApplication::Instance()->GetCurrentState();
        nAppState * curState = nApplication::Instance()->FindState( curStateName );
        if ( curState && curState->IsA("ncommonstate") )
        {
            nCommonState * tragState = static_cast<nCommonState *>( curState );

            for(int i = 0;i < this->strings.Size();i++)
            {
                tragState->UpdateOnScreenLog( this->strings.At(i).Get() );
            }
            this->strings.Clear();

            tragState->UpdateOnScreenLog( log );
        }
        else
        {
            this->strings.Append(log);
        }
    }
}

