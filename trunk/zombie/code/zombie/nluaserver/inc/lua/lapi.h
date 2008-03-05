/*
** $Id: lapi.h,v 1.2 2004/03/26 00:39:30 enlight Exp $
** Auxiliary functions from Lua API
** See Copyright Notice in lua.h
*/

#ifndef lapi_h
#define lapi_h

#include "lobject.h"

void luaA_pushobject (lua_State *L, const TObject *o);

#endif
