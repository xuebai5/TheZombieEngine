//------------------------------------------------------------------------------
//  ndebugcross_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugtext.h"

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nDebugText::nDebugText():
    x(0.f),
    y(0.f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugText::~nDebugText()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nDebugText::SetScreenPos( float  x, float y )
{
    this->x = x;
    this->y = y;
}

//------------------------------------------------------------------------------
/**
*/
void 
nDebugText::SetText( const nString& string)
{
    this->text = string;
}

//------------------------------------------------------------------------------
/**
*/
void 
nDebugText::SetText( const char* string)
{
    this->text.Set(string);
}

//------------------------------------------------------------------------------
/**
*/
void __cdecl
nDebugText::Format(const char* fmtString, ...)
{
    va_list argList;
    va_start(argList, fmtString);
    this->text.FormatWithArgs(fmtString, argList);
    va_end(argList);
}


//------------------------------------------------------------------------------
/**
    @param gfxServer graphics server
*/
void
nDebugText::Draw( nGfxServer2 * const /*gfxServer*/ )
{
    vector4 color;
    this->GetColor( color );
    nGfxServer2::Instance()->Text( text.Get(), color , x, y);
}

//------------------------------------------------------------------------------
