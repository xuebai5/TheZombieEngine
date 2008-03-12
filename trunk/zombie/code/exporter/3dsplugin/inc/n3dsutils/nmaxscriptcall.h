//-----------------------------------------------------------------------------
//  nmaxscriptcall.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXSCRIPTCALL_H
#define N_MAXSCRIPTCALL_H

class Value; //define in maxscript but not include theis header if it is not necesary
bool nMaxScriptCall(const char* script);
bool nMaxScriptCall(const char* script, Value*& res);

bool nMaxScriptGetValue(const char*script, char* &val);
bool nMaxScriptGetValue(const char*script, int   &val);
bool nMaxScriptGetValue(const char*script, float   &val);
bool nMaxScriptGetValue(const char*script, bool  &val);




#endif
