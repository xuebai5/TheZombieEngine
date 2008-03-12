/**
   @file n3dscustomattributes.h
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/
#ifndef N3DSCUSTOMATTRIBUTES_H
#define N3DSCUSTOMATTRIBUTES_H
#include "util/narray.h"
class nString;


class n3dsCustomAttributes 
{
public:
    /// get the string in the node attributes
    static bool GetParamString( INode * const node, const nString &attribute, const nString &param, TSTR & value );
    /// get the integer in the node attributes
    static bool GetParamInt( INode * const node, const nString &attribute, const nString &param, int & value );
    /// get the array of nodes in the node attributes
    static bool GetParamNodes( INode * const node, const nString & attribute, const nString & param, nArray<INode*> & value );
};

#endif//N3DSCUSTOMATTRIBUTES_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
