//------------------------------------------------------------------------------
//  nd3d9server_cmds.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchndirect3d9.h"
#include "gfx2/nd3d9server.h"

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
nD3D9Server::SetDrawEnabled(const bool value)
{
    this->drawEnabled = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nD3D9Server::GetDrawEnabled() const
{
    return this->drawEnabled;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Server::SetDrawLines(const bool value)
{
    this->drawLines = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nD3D9Server::GetDrawLines() const
{
    return this->drawLines;
}

//------------------------------------------------------------------------------
/**
*/
void
nD3D9Server::SetDrawText(const bool value)
{
    this->drawText = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nD3D9Server::GetDrawText() const
{
    return this->drawText;
}
#endif //NGAME
